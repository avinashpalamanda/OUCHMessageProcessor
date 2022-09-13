#include "MessageConverterUtility.h"
#include <sstream>
#include <iostream>

namespace MessageUtility
{
    /*Get message length from Message Type*/
    const int MessageLengthFromMessageType(const char sMessageType)
    {
        switch(sMessageType)
        {
            case 'A':
                return 68;
            case 'S':
                return 13;
            case 'U':
                return 82;
            case 'E':
                return 43;
            case 'C':
                return 31;
        }
        return 0;
    }

    /*Get message start index from packet start index*/
    const int MessageStartIndexFromPacketIndex(const int iPacketStartIndex)
    {
        return (iPacketStartIndex + _iPacketHeaderSize);
    }


    /*Get Input stream from packet header*/
    const int GetInputStream(const std::vector<char> &streamInVector, const int iPacketStartIndex)
    {
        std::stringstream ss;
        ss << static_cast<int>(streamInVector[iPacketStartIndex]) << static_cast<int>(streamInVector[iPacketStartIndex+1]);
        return stoi(ss.str());
    }

    /*Get Packet length from Packet header*/
    const int GetPacketLength(const std::vector<char> &streamInVector, const int iPacketStartIndex)
    {
        std::stringstream ss;
        ss << static_cast<int>(streamInVector[iPacketStartIndex+2]) << static_cast<int>(streamInVector[iPacketStartIndex+3]) <<
            static_cast<int>(streamInVector[iPacketStartIndex+4]) << static_cast<int>(streamInVector[iPacketStartIndex+5]);
        return stoi(ss.str());
    }

    /*Get Message Length from packet*/
    const int GetMessageLength(const std::vector<char> &streamInVector, const int iPacketStartIndex)
    {
        std::stringstream ss;
        auto iMessageStartIndex = iPacketStartIndex + _iPacketHeaderSize;
        ss << static_cast<int>(streamInVector[iMessageStartIndex]) << static_cast<int>(streamInVector[iMessageStartIndex+1]);
        return stoi(ss.str());
    }

    /*Get message structure from message type*/
    std::unique_ptr<stMessage> CreateMessageStFromMessageType(enMessageType eMessageType)
    {
        std::unique_ptr<stMessage> message;
        switch(eMessageType)
        {
            case eSystemMessage:
                message.reset(new stSystemMessage());
            case eAck:
                message.reset(new stAckMessage());
            case eReplace:
                message.reset(new stReplaceMessage());
            case eCancel:
                message.reset(new stCancelMessage());
            case eExecution:            
                message.reset(new stExecutionMessage());
            default:
                break;
        }
        return message;
    }

    /*From message stream to message structure*/
    std::unique_ptr<stMessage> MessageStreamTostPacketMessage(const std::vector<char> &result, int index, const unsigned int iStream)
    {
        auto iActualMessageIndex = index + _iMessageLengthSize;
        auto packetType =  result[iActualMessageIndex++];
        auto messageType = static_cast<enMessageType>(result[iActualMessageIndex++]);
        
        auto processedMessage = CreateMessageStFromMessageType(messageType);
        processedMessage->_PacketType = packetType;      
        processedMessage->_MessageType = messageType;

        switch(messageType)
        {
            case eSystemMessage:
            case eAck:
            case eReplace:
            case eCancel:
                break;
            case eExecution:
            {
                auto iExecutedSharesIndex = index +26;
                std::stringstream ss;
                ss << static_cast<int>(result[iExecutedSharesIndex]) << static_cast<int>(result[iExecutedSharesIndex+1]) 
                    <<  static_cast<int>(result[iExecutedSharesIndex+2]) <<  static_cast<int>(result[iExecutedSharesIndex+3]);
                static_cast<stExecutionMessage*>(processedMessage.get())->dExecutedShares = stoi(ss.str());
                break;
            }
            default:
                break;
        }


        return processedMessage;
    }
}