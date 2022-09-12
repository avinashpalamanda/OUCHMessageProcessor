#include <iostream>
#include "ProcessDataFromFile.h"

int main()
{
    std::string inputFile("input/OUCHLMM2.incoming.packets");
    ProcessDataFromFile messageProcessor(inputFile);
    messageProcessor.ProcessData();
    messageProcessor.PrintData();
    return 0;
}