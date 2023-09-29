#include <iostream>
#include <sstream>
#include <string>
#include "asio.hpp"
#include <cmath>
#include <chrono>
#include "sapnamedtype.h"
#include <mutex>
#include <list>

namespace pml
{
    namespace sap
    {
        class Sender
        {
            public:

                Sender(asio::io_context& io_context, const IpAddress& outboundIpAddress, const asio::ip::address& multicast_address, unsigned short nPort);
                ~Sender();

                void SetDelay(std::chrono::milliseconds delay);
                void Run();
                            
                void AddSdp(const std::string& sSDP);
                void RemoveSdp(const std::string& sSDP);

            protected:
            private:
                void do_send();
                void do_timeout();
                std::vector<uint8_t> CreateMessage();

                bool CheckForMessages();
                

                const std::chrono::milliseconds& GetDelay();
                
                void CalculateGap();

                IpAddress m_outboundIpAddress;
                asio::ip::udp::endpoint m_endpoint;
                asio::ip::udp::socket m_socket;
                asio::steady_timer m_timer;

                std::chrono::milliseconds m_delay;
                std::chrono::milliseconds m_gap;


                std::mutex m_mutex;
                
                std::string m_sUpdatedSDP;
                static const std::string STR_MIME;

                struct sdpMessage
                {
                    std::string sSdp;
                    unsigned long nMessageVersion = 0;
                    bool bRemove = false;
                };

                std::vector<uint8_t> CreateMessage(sdpMessage& msg);

                std::list<sdpMessage> m_lstSdp;
                std::list<sdpMessage>::iterator m_itToSend;
                
        };
    }
}

