#ifndef PML_SAPSERVER_SAPHANDLER_H
#define PML_SAPSERVER_SAPHANDLER_H

#include "sapdlldefine.h"

namespace pml::sap
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
}

#endif