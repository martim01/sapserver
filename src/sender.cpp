#include "sapsender.h"
#include <cmath>

#ifdef __GNU__
#include <linux/socket.h>
#endif // __GNU__
#include "log.h"

using namespace pml::sap;

const std::string Sender::STR_MIME = "application/sdp";

Sender::Sender(asio::io_context& io_context, const IpAddress& outboundIpAddress, const asio::ip::address& multicast_address,
        unsigned short nPort) :
          m_outboundIpAddress(outboundIpAddress),
          m_endpoint(multicast_address, nPort),
          m_socket(io_context, m_endpoint.protocol()),
          m_timer(io_context),
          m_delay(std::chrono::milliseconds(30000)),
          m_gap(m_delay)
{

}

Sender::~Sender()
{
    //@todo send a remove for all sdp
}
    



void Sender::Run()
{
    std::error_code ec;
    asio::ip::multicast::outbound_interface option(asio::ip::make_address_v4(m_outboundIpAddress.Get(), ec));
    if(!ec)
    {
        m_socket.set_option(option);
        do_send();
    }
    else
    {
        pmlLog(LOG_ERROR, "pml::sapserver") << "Sender Run option failed: " << ec.message() << "\t" << m_outboundIpAddress.Get();
    }
}
bool Sender::CheckForMessages()
{
    std::scoped_lock lg(m_mutex);
    return m_lstSdp.empty() == false;
}

void Sender::do_send()
{
    if(CheckForMessages())
    {
        m_socket.async_send_to(asio::buffer(CreateMessage()), m_endpoint,
        [this](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                do_timeout();
            }
            else
            {
                pmlLog(LOG_ERROR, "pml::sapserver") <<  "Sender Send failed: " << ec;
            }
        });
    }
    else
    {
        do_timeout();
    }
}


void Sender::AddSdp(const std::string& sSDP)
{
    pmlLog(pml::LOG_DEBUG, "pml::sapserver") << "AddSdp\t" << sSDP;
    for(auto& msg : m_lstSdp)
    {
        if(msg.sSdp == sSDP)
        {
            return;
        }
    }
    
    m_lstSdp.push_back({sSDP, 0, false});
    if(m_lstSdp.size() == 1)
    {
        m_itToSend = m_lstSdp.begin();
    }
    
    m_socket.send_to(asio::buffer(CreateMessage(m_lstSdp.back())), m_endpoint);

    CalculateGap();
}

void Sender::CalculateGap()
{
    if(m_lstSdp.empty() == false)
    {
        m_gap = std::chrono::milliseconds(m_delay.count() / m_lstSdp.size());
    }
    else
    {
        m_itToSend = m_lstSdp.end();
    }
}

void Sender::RemoveSdp(const std::string& sSDP)
{
    for(auto itSdp = m_lstSdp.begin(); itSdp != m_lstSdp.end(); ++itSdp)
    {
        if(itSdp->sSdp == sSDP)
        {
            itSdp->bRemove = true;
            m_socket.send_to(asio::buffer(CreateMessage(*itSdp)), m_endpoint);
            m_lstSdp.erase(itSdp);
            CalculateGap();
            break;
        }
    }
    
}

std::vector<uint8_t> Sender::CreateMessage()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    auto vMessage = CreateMessage(*m_itToSend);
    ++m_itToSend;
    if(m_itToSend == m_lstSdp.end())
    {
        m_itToSend = m_lstSdp.begin();
    }
    return vMessage;
}

std::vector<uint8_t> Sender::CreateMessage(Sender::sdpMessage& msg)
{
    msg.nMessageVersion++;
    

    std::vector<uint8_t> vMessage;
    vMessage.reserve(msg.sSdp.size()+12);


    unsigned long nIpServer = inet_addr (m_outboundIpAddress.Get().c_str()); //TODO automaticaly get it ?

    vMessage.push_back(0x20);
    vMessage.push_back(0x00);
    vMessage.push_back(msg.nMessageVersion & 0xff);
    vMessage.push_back(msg.nMessageVersion >> 8);
    vMessage.push_back(nIpServer & 0xFF);
    vMessage.push_back((nIpServer >> 8) & 0xFF);
    vMessage.push_back((nIpServer >> 16) & 0xFF);
    vMessage.push_back((nIpServer >> 24) & 0xFF);

    std::copy(STR_MIME.begin(), STR_MIME.end(), std::back_inserter(vMessage));
    vMessage.push_back(0x00); //null terminated string

    std::copy(msg.sSdp.begin(), msg.sSdp.end(), std::back_inserter(vMessage));
    vMessage.push_back(0x00); //null terminated string??

    if(msg.bRemove)
    {
        vMessage[0] |= 0x04;
    }
    return vMessage;
}


void Sender::do_timeout()
{
    m_timer.expires_after(m_gap);
    m_timer.async_wait(
    [this](std::error_code ec)
    {
        if (!ec)
        {
            do_send();
        }
        else
        {
            pmlLog(LOG_ERROR, "pml::sapserver") << "Sender Timeout failed: " << ec;
        }
    });
}

void Sender::SetDelay(std::chrono::milliseconds delay)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    m_delay = delay;
    CalculateGap();
}
