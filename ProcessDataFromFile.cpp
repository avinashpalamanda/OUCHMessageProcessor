#include "ProcessDataFromFile.h"
#include "MessageConverterUtility.h"
#include <iostream>
#include <fstream>
#include <unordered_set>

ProcessDataFromFile::ProcessDataFromFile(std::string &sFileName)
{
    std::ifstream ifs(sFileName, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type totalSize = ifs.tellg();
    _streamAsVector.resize(totalSize);
        
    ifs.seekg(0, std::ios::beg);
    ifs.read(&_streamAsVector[0], totalSize);
}

bool
ProcessDataFromFile::ProcessData()
{
    for (size_t iPacketIndex=0; iPacketIndex < _streamAsVector.size();)
    {           
        auto iStream = MessageUtility::GetInputStream(_streamAsVector, iPacketIndex);
        auto iPacketLength = MessageUtility::GetPacketLength(_streamAsVector, iPacketIndex);

        const auto iMessageStartIndex = MessageUtility::MessageStartIndexFromPacketIndex(iPacketIndex);
            
        if(CheckForExistingIncompleteMessage(iStream))
        {
            AppendIncompleteDataAndProcess(iStream, iPacketLength, iMessageStartIndex);
        }
        else if(IsIncompleteMessage(iPacketLength, iPacketIndex))
        {
            AddToIncompleteData(iStream, iPacketLength, iPacketIndex, iMessageStartIndex);
        }
        else
        {
            _ProcessedData[iStream].push_back(MessageUtility::MessageStreamTostPacketMessage(_streamAsVector, iMessageStartIndex, iStream));            
        }            
        iPacketIndex += iPacketLength + MessageUtility::_iPacketHeaderSize;
    }
    return true;
}

void
ProcessDataFromFile::PrintData()
{
    std::cout << "Stream" << std::endl;
    for (auto data : _ProcessedData)
    {
        std::cout << data.first << std::endl;
    }

    std::unordered_set<char> messageTypes;
    for (auto data : _ProcessedData)
    {
        auto& sMessages = data.second;
        for(auto message : sMessages)
            messageTypes.insert(message._PacketType);
    }

    std::cout << "Message Type" << std::endl;
    for (auto data : messageTypes)
    {
        std::cout << data << std::endl;
    }

    std::cout <<std::flush << std::endl;
}


bool
ProcessDataFromFile::CheckForExistingIncompleteMessage(const int iStream)
{
    return (_IncompleteMessage.find(iStream)!=_IncompleteMessage.end());
}

bool
ProcessDataFromFile::IsIncompleteMessage(const int iPacketLength, const int iPacketIndex)
{
    auto iMessageLength = MessageUtility::GetMessageLength(_streamAsVector, iPacketIndex);
    return (iMessageLength + MessageUtility::_iMessageLengthSize != iPacketLength);
}

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
                _ProcessedData[iStream].push_back(MessageUtility::MessageStreamTostPacketMessage(itr_PendingToBeProcessed->second.vPendingBytesToBeProcessed, 0, iStream));              
                _IncompleteMessage.erase(itr_PendingToBeProcessed);
            }
        }
    }
}

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
