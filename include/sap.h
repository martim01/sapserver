#pragma once
#include <string>
#include "sapdlldefine.h"

namespace pml
{
    struct SAP_EXPORT sap
    {
        sap() :
        nVersion(0),
        bIpv6(false),
        bDeletionMessage(false),
        bEncrypted(false),
        bCompressed(false),
        nAuthenticationLength(0),
        nMessageId(0),
        nOriginatingSource(0){}

        uint8_t nVersion;
        bool bIpv6;
        bool bDeletionMessage;
        bool bEncrypted;
        bool bCompressed;
        uint8_t nAuthenticationLength;
        uint16_t nMessageId;
        uint32_t nOriginatingSource;

        std::string sSenderIp;
        std::string sMimeType;
        std::string sData;
    };
}
