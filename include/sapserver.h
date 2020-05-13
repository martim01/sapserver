#pragma once
#include "sapdlldefine.h"
#include "namedtype.h"
#include <memory>
#include <chrono>

namespace pml
{
    class Handler;
    class SapServerImpl;

    class SAP_EXPORT SapServer
    {
        public:
            SapServer(std::shared_ptr<Handler> pHandler);
            ~SapServer();
            void Run();


            void AddSender(const IpAddress& localAddress, std::chrono::milliseconds delay, const std::string& sSDP);
            void SetSenderDelay(const IpAddress& localAddress, std::chrono::milliseconds delay);
            void SetSenderSDP(const IpAddress& localAddress, const std::string& sSDP);
            void RemoveSender(const IpAddress& localAddress);
            void RemoveAllSenders();

            void AddReceiver(const IpAddress& multicastAddress);
            void RemoveReceiver(const IpAddress& multicastAddress);

            void Stop();
            bool IsStopped();

        private:
            std::unique_ptr<SapServerImpl> m_pImpl;
        };
};
