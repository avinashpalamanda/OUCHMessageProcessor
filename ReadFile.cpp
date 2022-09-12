#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "MessageHeader.h"
#include "MessageConverterUtility.h"
#include "ProcessDataFromFile.h"

int main()
{
    std::string inputFile("input/OUCHLMM2.incoming.packets");
    ProcessDataFromFile messageProcessor(inputFile);
    messageProcessor.ProcessData();
    messageProcessor.PrintData();
    return 0;
}