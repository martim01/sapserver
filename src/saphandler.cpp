#include "saphandler.h"
#include "parser.h"
#include <iostream>

using namespace sapserver;

void StdHandler::SapMessage(const sap& aMessage)
{
    if(aMessage.bDeletionMessage == false)
    {
        std::cout << "------ ADD SAP --------" << std::endl;
    }
    else
    {
        std::cout << "------ REM SAP --------" << std::endl;
    }
    std::cout << aMessage.sMimeType << std::endl;
    std::cout << aMessage.sData << std::endl;
    std::cout << "-----------------------" << std::endl;
}