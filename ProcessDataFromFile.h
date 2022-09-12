#pragma once
#include "MessageHeader.h"
#include <unordered_map>
#include <memory>

class ProcessDataFromFile
{
public:
    ProcessDataFromFile(std::string &sFileName);
    bool ProcessData();
    void PrintData();

private:

    bool CheckForExistingIncompleteMessage(const int iStream);
    bool IsIncompleteMessage(const int iPacketLength, const int iPacketIndex);
    void AppendIncompleteDataAndProcess(const int iStream, const int iPacketLength, const int iMessageStartIndex);
    void AddToIncompleteData(const int iStream, const int iPacketLength, const int iPacketIndex, const int iMessageStartIndex);

    std::vector<char>  _streamAsVector;
    std::unordered_map<unsigned int, std::vector<std::unique_ptr<stMessage>>> _ProcessedData;
    std::unordered_map<unsigned int, stIncompleteMessage> _IncompleteMessage;

    struct sMessageStatistics
    {
        int iAccepted = 0;
        int iSystemEvent = 0;
        int iReplaced = 0;
        int iCancelled = 0;
        int iExecuted = 0;
        double dTotalExecutedShares = 0;
    };

};
