#ifndef SAPSERVER_DISCOVER_HANDLERTOFILE_H
#define SAPSERVER_DISCOVER_HANDLERTOFILE_H

#include "saphandler.h"

namespace pml::sap
{
    class HandlerToFile : public Handler
    {
        public:
            void SapMessage(const sap& aMessage) final;
    };
}

#endif