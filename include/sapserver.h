#pragma once
#include "sapdlldefine.h"
#include "sapnamedtype.h"
#include <memory>
#include <chrono>

namespace pml
{
    namespace sap
    {
        class Handler;
        class SapServerImpl;

        class SAP_EXPORT Server
        {
            public:
                Server(std::shared_ptr<Handler> pHandler, bool bThreaded=true);
                ~Server();
                void Run();


                void AddSender(const IpAddress& localAddress, std::chrono::milliseconds delay = std::chrono::milliseconds(30000), const std::string& sSDP="");

                void SetSenderDelay(const IpAddress& localAddress, std::chrono::milliseconds delay);
                void RemoveSender(const IpAddress& localAddress);
                void RemoveAllSenders();

                void AddSdp(const IpAddress& localAddress, const std::string& sSDP);
                void RemoveSdp(const IpAddress& localAddress, const std::string& sSDP);

                void AddReceiver(const IpAddress& multicastAddress, const IpAddress& listenAddress = IpAddress("0.0.0.0"));
                void RemoveReceiver(const IpAddress& multicastAddress);

                void Stop();
                bool IsStopped();

            private:
                std::unique_ptr<SapServerImpl> m_pImpl;
            };
    }
}
