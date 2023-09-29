#pragma once
#include <vector>
#include <memory>
#include <list>
#include "sap.h"

namespace pml
{
    namespace sap
    {
        class Handler;

        class Parser
        {
            public:
                Parser(std::shared_ptr<Handler> pHandler) : m_pHandler(pHandler){}

                void ParseMessage(const std::string& sSenderIp, std::vector<unsigned char> vMessage);

            protected:
                std::shared_ptr<Handler> m_pHandler;
        };
    }
}

