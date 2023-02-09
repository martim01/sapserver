#include "sapserverimpl.h"
#include "sapsender.h"
#include "sapparser.h"
#include "saphandler.h"
#include "sapreceiver.h"
#include "log.h"

using namespace pml;

SapServerImpl::SapServerImpl(std::shared_ptr<Handler> pHandler, bool bThreaded) : m_pHandler(pHandler),
m_bThreaded(bThreaded)
{

}

SapServerImpl::~SapServerImpl()
{
    RemoveAllSenders();
    Stop();
}

void SapServerImpl::AddSender(const IpAddress& localAddress, std::chrono::milliseconds delay, const std::string& sSDP)
{
    pmlLog(LOG_DEBUG) << "SapServer\t" << "AddSender: " << localAddress.Get();

    try
    {
        auto pairSender = m_mSenders.insert(std::make_pair(localAddress.Get(), std::unique_ptr<Sender>(new Sender( m_context, localAddress, asio::ip::make_address("239.255.255.255"), SAP_PORT))));

        pairSender.first->second->SetDelay(delay);
        pairSender.first->second->SetSDP(sSDP);

        if(pairSender.second)
        {
            pairSender.first->second->Run();
        }

        if(IsStopped() && m_bThreaded)
        {
            Run();
        }
    }
    catch(const std::system_error& e)
    {
        pmlLog(LOG_ERROR) << "SapServer: Cannot add sender " << e.what();
    }
}

void SapServerImpl::SetSenderDelay(const IpAddress& localAddress, std::chrono::milliseconds delay)
{
    pmlLog(LOG_DEBUG) << "SapServer\t" << "SetSenderDelay: " << localAddress.Get();
    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->SetDelay(delay);
        pmlLog(LOG_DEBUG) << delay.count() << "ms";
    }
    else
    {
        pmlLog(LOG_WARN) << "Sender not found";
    }

}

void SapServerImpl::SetSenderSDP(const IpAddress& localAddress, const std::string& sSDP)
{
    pmlLog(LOG_DEBUG) << "SapServer\t" << "SetSenderDelay: " << localAddress.Get();

    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->SetSDP(sSDP);
        pmlLog(LOG_DEBUG) << sSDP;
    }
    else
    {
        pmlLog(LOG_WARN) << "Sender not found";
    }
}

void SapServerImpl::RemoveSender(const IpAddress& localAddress)
{
    pmlLog(LOG_DEBUG) << "SapServer\t" << "RemoveSender: " << localAddress.Get();
    auto itSender = m_mSenders.find(localAddress.Get());
    if(itSender != m_mSenders.end())
    {
        itSender->second->Remove();
        m_mSenders.erase(localAddress.Get());
        pmlLog(LOG_DEBUG);
    }
    else
    {
        pmlLog(LOG_WARN) << "Sender not found";
    }
}

void SapServerImpl::RemoveAllSenders()
{
    pmlLog(LOG_DEBUG) << "SapServer\t" << "RemoveAllSenders";
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
        pmlLog(LOG_INFO) << "SapServer\t" << "Restart context";
        m_context.restart();
    }

    if(m_bThreaded && m_pThread == nullptr)
    {
        m_pThread = std::make_unique<std::thread>([this]() {    RunContext();  });
    }
    else
    {
        m_pThread = nullptr;
        RunContext();
    }
}


void SapServerImpl::RunContext()
{
    try
    {
        auto work = asio::require(m_context.get_executor(), asio::execution::outstanding_work.tracked);
        pmlLog(LOG_INFO) << "SapServer\t" << "Run context";
        m_context.run();
        pmlLog(pml::LOG_DEBUG) << "SapServer\tContext stopped";
    }
    catch (const std::exception& e)
    {
        pmlLog(LOG_ERROR) << "SapServer\t Failed to run context: " << e.what();
    }
}
void SapServerImpl::Stop()
{
    pmlLog(LOG_INFO) << "SapServer\t" << "Stop context";
    m_context.stop();
    if(m_pThread)
    {
        m_pThread->join();
    }
}

bool SapServerImpl::IsStopped()
{
    return m_context.stopped();
}


void SapServerImpl::AddReceiver(const IpAddress& multicastAddress, const IpAddress& listenAddress)
{
    pmlLog(LOG_DEBUG) << "SapServer\t" << "AddReceiver: " << multicastAddress.Get();

    auto pairReceiver = m_mReceivers.insert(std::make_pair(multicastAddress.Get(), std::unique_ptr<Receiver>( new Receiver(m_context, std::make_shared<Parser>(m_pHandler)))));

    if(pairReceiver.second)
    {
        pairReceiver.first->second->Run(asio::ip::make_address(listenAddress.Get()), asio::ip::make_address(multicastAddress.Get()), SAP_PORT);
    }
    if(IsStopped() && m_bThreaded)
    {
        Run();
    }
}

void SapServerImpl::RemoveReceiver(const IpAddress& multicastAddress)
{
    pmlLog(LOG_DEBUG) << "SapServer\t" << "RemoveReceiver: " << multicastAddress.Get();
    m_mReceivers.erase(multicastAddress.Get());
}

