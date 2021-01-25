#include "sapparser.h"
#include <algorithm>
#include "saphandler.h"
#include "log.h"
#include "asio.hpp"


using namespace pml;


void Parser::ParseMessage(const std::string& sSenderIp, std::vector<unsigned char> vMessage)
{
    if(vMessage.size() < 8)
    {
        Log::Get(Log::LOG_ERROR) << "SapServer\t" << "Parser SAP message to short" << std::endl;
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
    aMessage.nMessageId = (static_cast<uint16_t>(vMessage[3]) << 8) + vMessage[2];

    size_t nEndOfSource(8);
    if(aMessage.bIpv6 == false)
    {
        asio::ip::address_v4::bytes_type bytes;
        std::copy_n(vMessage.begin()+4, 4, bytes.begin());
        asio::ip::address_v4 addr(asio::ip::make_address_v4(bytes));

        aMessage.sOriginatingSource = addr.to_string();

        pml::Log::Get(pml::Log::LOG_TRACE) << "SAP: Sender=" << sSenderIp << " Source=" << aMessage.sOriginatingSource << std::endl;
        //aMessage.nOriginatingSource[0] =  (static_cast<uint32_t>(vMessage[7]) << 24) +
        //                                (static_cast<uint32_t>(vMessage[6]) << 16) +
         //                               (static_cast<uint32_t>(vMessage[5]) << 8) +
          //                              (static_cast<uint32_t>(vMessage[4]));
//
    }
    else
    {
        nEndOfSource = 20;

        asio::ip::address_v6::bytes_type bytes;
        std::copy_n(vMessage.begin()+4, 16, bytes.begin());
        asio::ip::address_v6 addr(asio::ip::make_address_v6(bytes));

        aMessage.sOriginatingSource = addr.to_string();

    }

    //extract the mime type -all chars up to \0
    auto itNull = std::find(vMessage.begin()+nEndOfSource, vMessage.end(), 0x00);
    aMessage.sMimeType = std::string(vMessage.begin()+8, itNull);
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
        Log::Get(Log::LOG_ERROR) << "SapServer\t" << "Parser SAP message no mime type" << std::endl;
    }
}
