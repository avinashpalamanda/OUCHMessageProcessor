#pragma once

#include <vector>
#include <unordered_map>
#include "MessageHeader.h"

namespace MessageUtility
{
    constexpr int _iMessageLengthSize = 2;
    constexpr int _iPacketHeaderSize = 6;

    const int MessageLengthFromMessageType(const char sMessageType);
    const int MessageStartIndexFromPacketIndex(const int iPacketStartIndex);

    const unsigned int GetInputStream(const std::vector<char> &streamInVector, const int iPacketStartIndex);
    const unsigned int GetPacketLength(const std::vector<char> &streamInVector, const int iPacketStartIndex);
    const unsigned int GetMessageLength(const std::vector<char> &streamInVector, const int iPacketStartIndex);
    stMessage MessageStreamTostPacketMessage(const std::vector<char> &streamInVector, int index, const unsigned int iStream);    
};