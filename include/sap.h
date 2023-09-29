#pragma once
#include <string>
#include <vector>
#include "sapdlldefine.h"
#include <cstdint>
namespace pml
{
    namespace sap
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
            nMessageId(0){}

            uint8_t nVersion;
            bool bIpv6;
            bool bDeletionMessage;
            bool bEncrypted;
            bool bCompressed;
            uint32_t nAuthenticationLength;
            uint16_t nMessageId;
            std::string sOriginatingSource;
            std::string sSenderIp;
            std::string sMimeType;
            std::string sData;
            std::vector<uint8_t> vAuthentication;
        };
    }
}
