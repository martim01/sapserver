#include "saphandler.h"
#include <iostream>

#include "sap.h"
#include "sapparser.h"



using namespace pml::sap;

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
