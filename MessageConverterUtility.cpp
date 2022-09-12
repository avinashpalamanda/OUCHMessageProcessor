#include "MessageConverterUtility.h"
#include <sstream>

namespace MessageUtility
{
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

    const int MessageStartIndexFromPacketIndex(const int iPacketStartIndex)
    {
        return (iPacketStartIndex + _iPacketHeaderSize);
    }


    const unsigned int GetInputStream(const std::vector<char> &streamInVector, const int iPacketStartIndex)
    {
        std::stringstream ss;
        ss << static_cast<int>(streamInVector[iPacketStartIndex]) << static_cast<int>(streamInVector[iPacketStartIndex+1]);
        return stoi(ss.str());
    }

    const unsigned int GetPacketLength(const std::vector<char> &streamInVector, const int iPacketStartIndex)
    {
        std::stringstream ss;
        ss << static_cast<int>(streamInVector[iPacketStartIndex+2]) << static_cast<int>(streamInVector[iPacketStartIndex+3]) <<
            static_cast<int>(streamInVector[iPacketStartIndex+4]) << static_cast<int>(streamInVector[iPacketStartIndex+5]);
        return stoi(ss.str());
    }

    const unsigned int GetMessageLength(const std::vector<char> &streamInVector, const int iPacketStartIndex)
    {
        std::stringstream ss;
        auto iMessageStartIndex = iPacketStartIndex + _iPacketHeaderSize;
        ss << static_cast<int>(streamInVector[iMessageStartIndex]) << static_cast<int>(streamInVector[iMessageStartIndex+1]);
        return stoi(ss.str());
    }

    stMessage MessageStreamTostPacketMessage(const std::vector<char> &result, int index, const unsigned int iStream)
    {
        stMessage data;
        auto iActualMessageIndex = index + _iMessageLengthSize;
        data._MessageType = result[iActualMessageIndex++];
        data._PacketType = result[iActualMessageIndex++];
        return data;
    }
}