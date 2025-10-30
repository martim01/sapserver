#ifndef PML_SAPSERVER_PARSER_H
#define PML_SAPSERVER_PARSER_H

#include <list>
#include <memory>
#include <vector>


#include "sap.h"

namespace pml::sap
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

#endif
