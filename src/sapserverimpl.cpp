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
    Log::Get(Log::LOG_DEBUG) << "SapServer\t" << "AddSender: " << localAddress.Get() << std::endl;

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
    Log::Get(Log::LOG_DEBUG) << "SapServer\t" << "SetSenderDelay: " << localAddress.Get();
    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->SetDelay(delay);
        Log::Get(Log::LOG_DEBUG) << delay.count() << "ms" << std::endl;
    }
    else
    {
        Log::Get(Log::LOG_WARN) << "Sender not found" << std::endl;
    }

}

void SapServerImpl::SetSenderSDP(const IpAddress& localAddress, const std::string& sSDP)
{
    Log::Get(Log::LOG_DEBUG) << "SapServer\t" << "SetSenderDelay: " << localAddress.Get();

    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->SetSDP(sSDP);
        Log::Get(Log::LOG_DEBUG) << sSDP << std::endl;
    }
    else
    {
        Log::Get(Log::LOG_WARN) << "Sender not found" << std::endl;
    }
}

void SapServerImpl::RemoveSender(const IpAddress& localAddress)
{
    Log::Get(Log::LOG_DEBUG) << "SapServer\t" << "RemoveSender: " << localAddress.Get();
    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->Remove();
        m_mSenders.erase(localAddress.Get());
        Log::Get(Log::LOG_DEBUG) << std::endl;
    }
    else
    {
        Log::Get(Log::LOG_WARN) << "Sender not found" << std::endl;
    }
}

void SapServerImpl::RemoveAllSenders()
{
    Log::Get(Log::LOG_DEBUG) << "SapServer\t" << "RemoveAllSenders" << std::endl;
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
        Log::Get(Log::LOG_INFO) << "SapServer\t" << "Restart context" << std::endl;
        m_context.restart();
    }

    std::thread t([this]()
    {
        try
        {
            Log::Get(Log::LOG_INFO) << "SapServer\t" << "Run context" << std::endl;
            m_context.run();
        }
        catch (const std::exception& e)
        {
            Log::Get(Log::LOG_ERROR) << "SapServer\t Faled to run context: " << e.what() << std::endl;
        }
    });

    t.detach();
}


void SapServerImpl::Stop()
{
    Log::Get(Log::LOG_INFO) << "SapServer\t" << "Stop context" << std::endl;
    m_context.stop();
}

bool SapServerImpl::IsStopped()
{
    return m_context.stopped();
}


void SapServerImpl::AddReceiver(const IpAddress& multicastAddress)
{
    Log::Get(Log::LOG_DEBUG) << "SapServer\t" << "AddReceiver: " << multicastAddress.Get() << std::endl;

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
    Log::Get(Log::LOG_DEBUG) << "SapServer\t" << "RemoveReceiver: " << multicastAddress.Get() << std::endl;
    m_mReceivers.erase(multicastAddress.Get());
}
