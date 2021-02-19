#include "sapreceiver.h"
#include "log.h"
#include "sapparser.h"

using namespace pml;

Receiver::Receiver(asio::io_context& io_context, std::shared_ptr<Parser> pParser) : m_socket(io_context), m_pParser(pParser)
{

}

void Receiver::Run(const asio::ip::address& listen_address, const asio::ip::address& multicast_address, unsigned int nPort)
{
    // Create the socket so that multiple may be bound to the same address.
    asio::ip::udp::endpoint listen_endpoint(listen_address, nPort);

    std::error_code ec;
    m_socket.open(listen_endpoint.protocol(), ec);
    if(!ec)
    {
        m_socket.set_option(asio::ip::udp::socket::reuse_address(true));
        m_socket.bind(listen_endpoint);

        // Join the multicast group.
        m_socket.set_option(asio::ip::multicast::join_group(multicast_address));

        do_receive();
    }
    else
    {
        Log(LOG_ERROR) << "SapServer\t" << "Receiver Run failed to open socket" << ec;
    }
}


void Receiver::do_receive()
{
    m_socket.async_receive_from(asio::buffer(m_data), m_sender_endpoint,
        [this](std::error_code ec, std::size_t length)
    {
        if (!ec)
        {
            m_pParser->ParseMessage(m_sender_endpoint.address().to_string(), std::vector<unsigned char>(m_data.begin(), m_data.begin()+length));
            do_receive();
        }
        else
        {
            Log(LOG_ERROR) << "SapServer\t" << "Receiver receive failed :" << ec;
        }
    });
}


