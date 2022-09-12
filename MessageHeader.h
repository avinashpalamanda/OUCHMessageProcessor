#pragma once

#include <string>
#include <vector>

struct stMessage
{
    char _PacketType = 0;
    char _MessageType = 0;
    std::string _TimeStamp;
};

struct stIncompleteMessage
{
    unsigned int iNumberOfBytesRecieved;
    std::vector<char> vPendingBytesToBeProcessed;
};