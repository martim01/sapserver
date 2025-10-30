#include "handlertofile.h"

#include <sstream>

void pml::sap::HandlerToFile::SapMessage(const sap& aMessage)
{
    std::stringstream ss;

    ss << "v=" << aMessage.nVersion << "\n"
       << "o=" << 
}