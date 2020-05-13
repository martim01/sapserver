#include <iostream>
#include <sstream>
#include <string>
#include "asio.hpp"
#include <cmath>
#include <chrono>
#include "namedtype.h"
#include <mutex>

namespace pml
{

    class Sender
    {
        public:

            Sender(asio::io_context& io_context, const IpAddress& outboundIpAddress, const asio::ip::address& multicast_address, unsigned short nPort);

            void SetDelay(std::chrono::milliseconds delay);
            void SetSDP(const std::string& sSDP);
            void Run();
            void Remove();

        protected:
        private:
            void do_send();
            void do_timeout();
            void CreateMessage(const std::string& sSDP);

            const std::chrono::milliseconds& GetDelay();
            const std::vector<uint8_t> GetMessage();

            IpAddress m_outboundIpAddress;
            asio::ip::udp::endpoint m_endpoint;
            asio::ip::udp::socket m_socket;
            asio::steady_timer m_timer;

            unsigned short m_nSequence;
            std::chrono::milliseconds m_delay;

            std::vector<uint8_t> m_vMessage;
            std::mutex m_mutex;
            unsigned long m_nMessageVersion;

            std::string m_sUpdatedSDP;
            static const std::string STR_MIME;
    };

};

