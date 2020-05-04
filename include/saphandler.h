#pragma once
#include "sapdlldefine.h"

namespace sapserver
{
    struct sap;

    class SAP_EXPORT Handler
    {
        public:

            virtual void SapMessage(const sap& aMessage)=0;
    };

    class SAP_EXPORT StdHandler : public Handler
    {
        public:
            void SapMessage(const sap& aMessage) override;
    };
};
