#include "sapreceiver.h"
#include "log.h"
#include "sapparser.h"

using namespace pml::sap;

Receiver::Receiver(asio::io_context& io_context, std::shared_ptr<Parser> pParser) : m_socket(io_context), m_pParser(pParser)
{

}

void Receiver::Run(const asio::ip::address& listen_address, const asio::ip::address& multicast_address, unsigned int nPort)
{
    // Create the socket so that multiple may be bound to the same address.
    asio::ip::udp::endpoint listen_endpoint(asio::ip::make_address("0.0.0.0"), nPort);

    std::error_code ec;
    m_socket.open(listen_endpoint.protocol(), ec);
    if(!ec)
    {
        m_socket.set_option(asio::ip::udp::socket::reuse_address(true));
        m_socket.bind(listen_endpoint);

        // Join the multicast group.
        asio::ip::address_v4 addr = listen_address.to_v4();
        auto multiAddr = multicast_address.to_v4();
        m_socket.set_option(asio::ip::multicast::join_group(multiAddr, addr), ec);
        if(ec)
        {
            pmlLog(pml::LOG_CRITICAL, "pml::sapserver") << "Receiver [" << nPort << "]: Can't join group: " << ec;
        }
        else
        {
            do_receive();
        }
    }
    else
    {
        pmlLog(LOG_ERROR, "pml::sapserver") << "Receiver Run failed to open socket" << ec;
    }
}


void Receiver::do_receive()
{
    pmlLog(LOG_TRACE, "pml::sapserver") << "Receiver do_receive";

    m_socket.async_receive_from(asio::buffer(m_data), m_sender_endpoint,
        [this](std::error_code ec, std::size_t length)
    {
        if (!ec)
        {
            pmlLog(LOG_TRACE, "pml::sapserver") <<  "Receiver do_receive: received";
            m_pParser->ParseMessage(m_sender_endpoint.address().to_string(), std::vector<unsigned char>(m_data.begin(), m_data.begin()+length));
            do_receive();
        }
        else
        {
            pmlLog(LOG_ERROR, "pml::sapserver") <<  "Receiver receive failed :" << ec;
        }
    });
}


