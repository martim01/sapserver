#include "sapserverimpl.h"
#include "sapsender.h"
#include "sapparser.h"
#include "saphandler.h"
#include "sapreceiver.h"
#include "log.h"

using namespace pml;

SapServerImpl::SapServerImpl(std::shared_ptr<Handler> pHandler) : m_pHandler(pHandler)
{

}

SapServerImpl::~SapServerImpl()
{
    RemoveAllSenders();
}

void SapServerImpl::AddSender(const IpAddress& localAddress, std::chrono::milliseconds delay, const std::string& sSDP)
{
    Log(LOG_DEBUG) << "SapServer\t" << "AddSender: " << localAddress.Get();

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
    Log(LOG_DEBUG) << "SapServer\t" << "SetSenderDelay: " << localAddress.Get();
    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->SetDelay(delay);
        Log(LOG_DEBUG) << delay.count() << "ms";
    }
    else
    {
        Log(LOG_WARN) << "Sender not found";
    }

}

void SapServerImpl::SetSenderSDP(const IpAddress& localAddress, const std::string& sSDP)
{
    Log(LOG_DEBUG) << "SapServer\t" << "SetSenderDelay: " << localAddress.Get();

    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->SetSDP(sSDP);
        Log(LOG_DEBUG) << sSDP;
    }
    else
    {
        Log(LOG_WARN) << "Sender not found";
    }
}

void SapServerImpl::RemoveSender(const IpAddress& localAddress)
{
    Log(LOG_DEBUG) << "SapServer\t" << "RemoveSender: " << localAddress.Get();
    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->Remove();
        m_mSenders.erase(localAddress.Get());
        Log(LOG_DEBUG);
    }
    else
    {
        Log(LOG_WARN) << "Sender not found";
    }
}

void SapServerImpl::RemoveAllSenders()
{
    Log(LOG_DEBUG) << "SapServer\t" << "RemoveAllSenders";
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
        Log(LOG_INFO) << "SapServer\t" << "Restart context";
        m_context.restart();
    }

    std::thread t([this]()
    {
        try
        {
            Log(LOG_INFO) << "SapServer\t" << "Run context";
            m_context.run();
        }
        catch (const std::exception& e)
        {
            Log(LOG_ERROR) << "SapServer\t Faled to run context: " << e.what();
        }
    });

    t.detach();
}


void SapServerImpl::Stop()
{
    Log(LOG_INFO) << "SapServer\t" << "Stop context";
    m_context.stop();
}

bool SapServerImpl::IsStopped()
{
    return m_context.stopped();
}


void SapServerImpl::AddReceiver(const IpAddress& multicastAddress)
{
    Log(LOG_DEBUG) << "SapServer\t" << "AddReceiver: " << multicastAddress.Get();

    auto pairReceiver = m_mReceivers.insert(std::make_pair(multicastAddress.Get(), std::unique_ptr<Receiver>( new Receiver(m_context, std::make_shared<Parser>(m_pHandler)))));

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
    Log(LOG_DEBUG) << "SapServer\t" << "RemoveReceiver: " << multicastAddress.Get();
    m_mReceivers.erase(multicastAddress.Get());
}
