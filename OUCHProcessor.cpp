#include "ProcessDataFromFile.h"

int main()
{
    ProcessDataFromFile messageProcessor("input/OUCHLMM2.incoming.packets");
    messageProcessor.ProcessData();
    messageProcessor.PrintData();
    return 0;
}
