#include <string>
#include <vector>
#include <unordered_map>
#include "MessageHeader.h"

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
    std::unordered_map<unsigned int, std::vector<stMessage>> _ProcessedData;
    std::unordered_map<unsigned int, stIncompleteMessage> _IncompleteMessage;

};
