#include "sapsender.h"
#include <cmath>

#ifdef __GNU__
#include <linux/socket.h>
#endif // __GNU__
#include "log.h"

using namespace pml;

const std::string Sender::STR_MIME = "application/sdp";

Sender::Sender(asio::io_context& io_context, const IpAddress& outboundIpAddress, const asio::ip::address& multicast_address,
        unsigned short nPort) :
          m_outboundIpAddress(outboundIpAddress),
          m_endpoint(multicast_address, nPort),
          m_socket(io_context, m_endpoint.protocol()),
          m_timer(io_context),
          m_nSequence(0),
          m_delay(std::chrono::milliseconds(30000)),
          m_nMessageVersion(0)
{

}

void Sender::Run()
{
    std::error_code ec;
    asio::ip::multicast::outbound_interface option(asio::ip::address_v4::from_string(m_outboundIpAddress.Get(), ec));
    if(!ec)
    {
        m_socket.set_option(option);
        do_send();
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << "SapServer\t" << "Sender Run option failed: " << ec << std::endl;
        std::cout << ec << std::endl;
    }
}

void Sender::do_send()
{
    m_socket.async_send_to(asio::buffer(GetMessage()), m_endpoint,
    [this](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            do_timeout();
        }
        else
        {
            Log::Get(Log::LOG_ERROR) << "SapServer\t" << "Sender Send failed: " << ec << std::endl;
        }
    });
}

void Sender::Remove()
{
    if(m_vMessage.empty() == false)
    {
        m_vMessage[0] |= 0x04;
        m_socket.send_to(asio::buffer(GetMessage()), m_endpoint);
    }
}

void Sender::SetSDP(const std::string& sSDP)
{
    if(m_vMessage.size() != 0)
    {
        Remove();
    }
    std::lock_guard<std::mutex> lg(m_mutex);
    CreateMessage(sSDP);
}

void Sender::CreateMessage(const std::string& sSDP)
{
    ++m_nMessageVersion;

    m_vMessage.clear();


    unsigned long nIpServer = inet_addr (m_outboundIpAddress.Get().c_str()); //TODO automaticaly get it ?
    m_vMessage.resize(8);

    m_vMessage[0] = 0x20;


    m_vMessage[1] = 0x00;
    m_vMessage[2] = m_nMessageVersion & 0xff;
    m_vMessage[3] = m_nMessageVersion >> 8;
    m_vMessage[4] = nIpServer & 0xFF;
    m_vMessage[5] = (nIpServer >> 8) & 0xFF;
    m_vMessage[6] = (nIpServer >> 16) & 0xFF;
    m_vMessage[7] = (nIpServer >> 24) & 0xFF;

    std::copy(STR_MIME.begin(), STR_MIME.end(), std::back_inserter(m_vMessage));
    m_vMessage.push_back(0x00); //null terminated string

    std::copy(sSDP.begin(), sSDP.end(), std::back_inserter(m_vMessage));
    m_vMessage.push_back(0x00); //null terminated string??
}


void Sender::do_timeout()
{
    m_timer.expires_after(GetDelay());
    m_timer.async_wait(
    [this](std::error_code ec)
    {
        if (!ec)
        {
            do_send();
        }
        else
        {
            Log::Get(Log::LOG_ERROR) << "SapServer\t" << "Sender Timeout failed: " << ec << std::endl;
        }
    });
}

void Sender::SetDelay(std::chrono::milliseconds delay)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    m_delay = delay;
}


const std::chrono::milliseconds& Sender::GetDelay()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return m_delay;
}

const std::vector<uint8_t> Sender::GetMessage()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    return m_vMessage;
}
