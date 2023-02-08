#pragma once
#include "asio.hpp"
#include "sapnamedtype.h"
#include <map>
#include <list>


namespace pml
{
    class Sender;
    class Receiver;
    class Handler;

    class SapServerImpl
    {
        public:
            SapServerImpl(std::shared_ptr<Handler> pHandler);
            ~SapServerImpl();
            void Run();


            void AddSender(const IpAddress& localAddress, std::chrono::milliseconds delay, const std::string& sSDP);
            void SetSenderDelay(const IpAddress& localAddress, std::chrono::milliseconds delay);
            void SetSenderSDP(const IpAddress& localAddress, const std::string& sSDP);
            void RemoveSender(const IpAddress& localAddress);
            void RemoveAllSenders();

            void AddReceiver(const IpAddress& multicastAddress, const IpAddress& listenAddress);
            void RemoveReceiver(const IpAddress& multicastAddress);



            void Stop();
            bool IsStopped();

        private:
            asio::io_context m_context;
            unsigned long m_nMessageVersion;

            std::map<std::string, std::unique_ptr<Sender> > m_mSenders;
            std::map<std::string, std::unique_ptr<Receiver> > m_mReceivers;

            std::shared_ptr<Handler> m_pHandler;
            static const uint16_t SAP_PORT = 9875;

            std::unique_ptr<std::thread> m_pThread = nullptr;
        };
};

