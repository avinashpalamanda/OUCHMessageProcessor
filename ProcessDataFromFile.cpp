#include "MessageHeader.h"
#include "ProcessDataFromFile.h"
#include "MessageConverterUtility.h"
#include <iostream>
#include <fstream>
#include <map>

ProcessDataFromFile::ProcessDataFromFile(const std::string &sFileName)
{
    std::ifstream ifs(sFileName, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type totalSize = ifs.tellg();
    _streamAsVector.resize(totalSize);
        
    ifs.seekg(0, std::ios::beg);
    ifs.read(&_streamAsVector[0], totalSize);
}

/*Process the data from file
 * 1) If stream as incomplete type collect the message and process it
 * 2) If the recieved message is incomplete, add it buffer to be processed later
 * 3) Recived full message process the data
 * */
bool
ProcessDataFromFile::ProcessData()
{
    for (size_t iPacketIndex=0; iPacketIndex < _streamAsVector.size();)
    {           
        auto iStream = MessageUtility::GetInputStream(_streamAsVector, iPacketIndex);
        auto iPacketLength = MessageUtility::GetPacketLength(_streamAsVector, iPacketIndex);

        const auto iMessageStartIndex = MessageUtility::MessageStartIndexFromPacketIndex(iPacketIndex);
            
        if(CheckForExistingIncompleteMessage(iStream)) // Check if the stream has incomplete file, if so proc
        {
            AppendIncompleteDataAndProcess(iStream, iPacketLength, iMessageStartIndex);
        }
        else if(IsIncompleteMessage(iPacketLength, iPacketIndex))
        {
            AddToIncompleteData(iStream, iPacketLength, iPacketIndex, iMessageStartIndex);
        }
        else
        {
            _ProcessedData[iStream].push_back(std::move(MessageUtility::MessageStreamTostPacketMessage(_streamAsVector, iMessageStartIndex, iStream)));            
        }            
        iPacketIndex += iPacketLength + MessageUtility::_iPacketHeaderSize;
    }
    return true;
}

/*Print Message Statistics*/
void
ProcessDataFromFile::PrintData()
{
    std::map<unsigned int, sMessageStatistics> mAllStreamStatistics;
    for (auto& streamMessages : _ProcessedData)
    {
        const auto& vMessages = streamMessages.second;
        auto& statistics = mAllStreamStatistics[streamMessages.first];
        for (const auto& message : vMessages)
        {
            switch(message->_MessageType)
            {
            case eSystemMessage:
                ++statistics.iSystemEvent;
                break;
            case eAck:
                ++statistics.iAccepted;
                break;
            case eReplace:
                ++statistics.iReplaced;
                break;
            case eCancel:
                ++statistics.iCancelled;
                break;
            case eExecution:
                ++statistics.iExecuted;
                statistics.dTotalExecutedShares = static_cast<stExecutionMessage*>(message.get())->dExecutedShares;
                break;
            case eInvalid:
            default:
                break;
            }
        }
    }

    for (auto statistics : mAllStreamStatistics) 
    {
        std::cout << "---------------------------------------------------------------------------" << std::endl;
        std::cout << "Stream " << statistics.first << std::endl;
        std::cout << "  Accepted:" << statistics.second.iAccepted <<" messages" << std::endl;
        std::cout << "  System Event: " << statistics.second.iSystemEvent <<" messages" << std::endl;
        std::cout << "  Replaced: " << statistics.second.iReplaced << " messages" << std::endl;
        std::cout << "  Cancelled: " << statistics.second.iCancelled <<" messages"<< std::endl;
        std::cout << "  Executed: " << statistics.second.iExecuted <<" messages: " <<
                    statistics.second.dTotalExecutedShares << " executed shares" << std::endl;

    }
}


/*Check if stream as incomplete message*/
bool
ProcessDataFromFile::CheckForExistingIncompleteMessage(const int iStream)
{
    return (_IncompleteMessage.find(iStream)!=_IncompleteMessage.end());
}

/*Check if message recieved is incomplete
 * Compare recieved packet length with expected packet length*/
bool
ProcessDataFromFile::IsIncompleteMessage(const int iPacketLength, const int iPacketIndex)
{
    auto iMessageLength = MessageUtility::GetMessageLength(_streamAsVector, iPacketIndex);
    return (iMessageLength + MessageUtility::_iMessageLengthSize != iPacketLength);
}

/*If stream as incomplete, add the new message to stream buffer and process it.*/
void
ProcessDataFromFile::AppendIncompleteDataAndProcess(const int iStream, const int iPacketLength, const int iMessageStartIndex)
{
    auto itr_PendingToBeProcessed = _IncompleteMessage.find(iStream);
    if (itr_PendingToBeProcessed != _IncompleteMessage.end())
    {
        itr_PendingToBeProcessed->second.iNumberOfBytesRecieved += iPacketLength;
        const auto iMessageEndIndex = iMessageStartIndex + iPacketLength;

        for(int msgPendingIndex = iMessageStartIndex; msgPendingIndex < iMessageEndIndex; ++msgPendingIndex)
            itr_PendingToBeProcessed->second.vPendingBytesToBeProcessed.push_back(_streamAsVector[msgPendingIndex]); 

        if(itr_PendingToBeProcessed->second.iNumberOfBytesRecieved >= 4) 
        {
            auto sMessageType = itr_PendingToBeProcessed->second.vPendingBytesToBeProcessed[3];
            if(MessageUtility::MessageLengthFromMessageType(sMessageType) == itr_PendingToBeProcessed->second.iNumberOfBytesRecieved)
            {
                _ProcessedData[iStream].push_back(std::move(MessageUtility::MessageStreamTostPacketMessage(itr_PendingToBeProcessed->second.vPendingBytesToBeProcessed, 0, iStream)));
                _IncompleteMessage.erase(itr_PendingToBeProcessed);
            }
        }
    }
}

/*If the message is incomplete, add it to buffer where the message will be processed later*/
void
ProcessDataFromFile::AddToIncompleteData(const int iStream, const int iPacketLength, const int iPacketIndex, const int iMessageStartIndex)
{
    stIncompleteMessage pendingMessage;
    pendingMessage.iNumberOfBytesRecieved = iPacketLength;                              
    auto iMessageEndIndex = iMessageStartIndex + iPacketLength;                
    for(int pendingIndex = iMessageStartIndex; pendingIndex < iMessageEndIndex; ++pendingIndex)
    {
        pendingMessage.vPendingBytesToBeProcessed.push_back(_streamAsVector[pendingIndex]);
    }
                        
    _IncompleteMessage[iStream] = pendingMessage;        
}
