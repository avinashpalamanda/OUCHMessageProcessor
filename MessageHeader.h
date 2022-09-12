#pragma once

#include <vector>
#include <string>

enum enMessageType {eSystemMessage = 'S', eAck = 'A', eReplace = 'U', eCancel = 'C', eExecution = 'E', eInvalid=0 };
struct stMessage
{
    char _PacketType = 0;
    enMessageType _MessageType=eInvalid;
    std::string _TimeStamp;
};

struct stSystemMessage : public stMessage{};

struct stAckMessage : public stMessage{};

struct stReplaceMessage : public stMessage{};

struct stCancelMessage : public stMessage{};

struct stExecutionMessage : public stMessage
{
    double dExecutedShares=0;
};


struct stIncompleteMessage
{
    int iNumberOfBytesRecieved;
    std::vector<char> vPendingBytesToBeProcessed;
};