#pragma once

#include "MessageHeader.h"
#include <unordered_map>
#include <memory>


namespace MessageUtility
{
    constexpr int _iMessageLengthSize = 2;
    constexpr int _iPacketHeaderSize = 6;

    const int MessageLengthFromMessageType(const char sMessageType);
    const int MessageStartIndexFromPacketIndex(const int iPacketStartIndex);

    const int GetInputStream(const std::vector<char> &streamInVector, const int iPacketStartIndex);
    const int GetPacketLength(const std::vector<char> &streamInVector, const int iPacketStartIndex);
    const int GetMessageLength(const std::vector<char> &streamInVector, const int iPacketStartIndex);
    std::unique_ptr<stMessage> MessageStreamTostPacketMessage(const std::vector<char> &streamInVector, int index, const unsigned int iStream);    
};