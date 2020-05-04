#include "sapserverimpl.h"
#include "sender.h"
#include "parser.h"
#include "saphandler.h"
#include "receiver.h"

using namespace sapserver;

SapServerImpl::SapServerImpl(std::shared_ptr<Handler> pHandler) : m_pHandler(pHandler)
{

}

SapServerImpl::~SapServerImpl()
{
    RemoveAllSenders();
}

void SapServerImpl::AddSender(const IpAddress& localAddress, std::chrono::milliseconds delay, const std::string& sSDP)
{
    auto pairSender = m_mSenders.insert(std::make_pair(localAddress.Get(), std::unique_ptr<Sender>(new Sender( m_context, localAddress, asio::ip::make_address("239.255.255.255"), SAP_PORT))));

    pairSender.first->second->SetDelay(delay);
    pairSender.first->second->SetSDP(sSDP);

    if(pairSender.second)
    {
        pairSender.first->second->Run();
    }

    if(IsStopped())
    {
        Run();
    }
}

void SapServerImpl::SetSenderDelay(const IpAddress& localAddress, std::chrono::milliseconds delay)
{
    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->SetDelay(delay);
    }
}

void SapServerImpl::SetSenderSDP(const IpAddress& localAddress, const std::string& sSDP)
{
    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->SetSDP(sSDP);
    }
}

void SapServerImpl::RemoveSender(const IpAddress& localAddress)
{
    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->Remove();
        m_mSenders.erase(localAddress.Get());
    }
}

void SapServerImpl::RemoveAllSenders()
{
    for(auto itSender = m_mSenders.begin(); itSender != m_mSenders.end(); ++itSender)
    {
        itSender->second->Remove();
    }
    m_mSenders.clear();

}


void SapServerImpl::Run()
{
    if(m_context.stopped())
    {
        m_context.restart();
    }

    std::thread t([this]()
    {
        try
        {
            m_context.run();
        }
        catch (const std::exception& e)
        {
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Exception: " << e.what() << "\n";
        }
    });

    t.detach();
}


void SapServerImpl::Stop()
{
    m_context.stop();
}

bool SapServerImpl::IsStopped()
{
    return m_context.stopped();
}


void SapServerImpl::AddReceiver(const IpAddress& multicastAddress)
{
    auto pairReceiver = m_mReceivers.insert(std::make_pair(multicastAddress.Get(),
                                            std::unique_ptr<Receiver>( new Receiver(m_context, std::make_shared<Parser>(m_pHandler)))));

    if(pairReceiver.second)
    {
        pairReceiver.first->second->Run(asio::ip::make_address("0.0.0.0"), asio::ip::make_address(multicastAddress.Get()), SAP_PORT);
    }

    if(IsStopped())
    {
        Run();
    }
}

void SapServerImpl::RemoveReceiver(const IpAddress& multicastAddress)
{
    m_mReceivers.erase(multicastAddress.Get());
}
