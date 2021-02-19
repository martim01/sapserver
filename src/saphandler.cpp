#include "saphandler.h"
#include "sapparser.h"
#include "sap.h"
#include <iostream>

using namespace pml;

void StdHandler::SapMessage(const sap& aMessage)
{
    if(aMessage.bDeletionMessage == false)
    {
        std::cout << "------ ADD SAP --------";
    }
    else
    {
        std::cout << "------ REM SAP --------";
    }
    std::cout << aMessage.sMimeType;
    std::cout << aMessage.sData;
    std::cout << "-----------------------";
}
