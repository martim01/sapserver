#include "sapparser.h"
#include <algorithm>
#include "saphandler.h"
#include "log.h"
#include "asio.hpp"


using namespace pml::sap;


void Parser::ParseMessage(const std::string& sSenderIp, std::vector<unsigned char> vMessage)
{
    if(vMessage.size() < 8)
    {
        pmlLog(LOG_ERROR, "pml::sapserver") << "Parser SAP message to short";
        return;
    }

    sap aMessage;
    aMessage.sSenderIp = sSenderIp;

    aMessage.nVersion = (vMessage[0] & 0xE0);
    aMessage.bIpv6 = (vMessage[0] & 0x10);
    aMessage.bDeletionMessage = (vMessage[0] & 0x4);
    aMessage.bEncrypted = (vMessage[0] & 0x2);
    aMessage.bCompressed = (vMessage[0] & 0x1);

    aMessage.nAuthenticationLength = vMessage[1];
    aMessage.nAuthenticationLength*=4;  //convert from number of 32 bit words to number of bytes
    aMessage.nMessageId = (static_cast<uint16_t>(vMessage[3]) << 8) + vMessage[2];

    size_t nStartOfPayload(8);
    if(aMessage.bIpv6 == false)
    {
        asio::ip::address_v4::bytes_type bytes;
        std::copy_n(vMessage.begin()+4, 4, bytes.begin());
        asio::ip::address_v4 addr(asio::ip::make_address_v4(bytes));
        aMessage.sOriginatingSource = addr.to_string();
    }
    else
    {
        nStartOfPayload = 20;

        asio::ip::address_v6::bytes_type bytes;
        std::copy_n(vMessage.begin()+4, 16, bytes.begin());
        asio::ip::address_v6 addr(asio::ip::make_address_v6(bytes));
        aMessage.sOriginatingSource = addr.to_string();
    }

    //next is authentication data if any
    if(aMessage.nAuthenticationLength != 0)
    {
        std::copy(vMessage.begin()+nStartOfPayload, vMessage.begin()+nStartOfPayload+aMessage.nAuthenticationLength, std::back_inserter(aMessage.vAuthentication));
        nStartOfPayload += aMessage.nAuthenticationLength;
    }

    //extract the mime type -all chars up to \0
    auto itNull = std::find(vMessage.begin()+nStartOfPayload, vMessage.end(), 0x00);
    aMessage.sMimeType = std::string(vMessage.begin()+nStartOfPayload, itNull);
    if(itNull != vMessage.end())
    {
        ++itNull;
    }
    aMessage.sData =  std::string(itNull, vMessage.end());
    if(aMessage.sMimeType.empty() == false)
    {
        m_pHandler->SapMessage(aMessage);
    }
    else
    {
        pmlLog(LOG_ERROR, "pml::sapserver") <<  "Parser SAP message no mime type";
    }
}
