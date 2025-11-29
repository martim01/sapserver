#include "sapserverimpl.h"

#include "log.h"

#include "saphandler.h"
#include "sapparser.h"
#include "sapreceiver.h"
#include "sapsender.h"

using namespace pml::sap;

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
    pml::log::log(pml::log::Level::kDebug, "pml::sapserver") <<  "AddSender: " << localAddress.Get();

    try
    {
        auto pairSender = m_mSenders.try_emplace(localAddress.Get(), std::unique_ptr<Sender>(new Sender( m_context, localAddress, asio::ip::make_address("239.255.255.255"), SAP_PORT)));

        pairSender.first->second->SetDelay(delay);
        if(sSDP.empty() == false)
        {
            pairSender.first->second->AddSdp(sSDP);
        }

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
        pml::log::log(pml::log::Level::kError, "pml::sapserver") << "Cannot add sender " << e.what();
    }
}

void SapServerImpl::AddSdp(const IpAddress& localAddress, const std::string& sSdp)
{
    pml::log::log(pml::log::Level::kDebug, "pml::sapserver") << "AddSdp\t" << localAddress << "\t" << sSdp;
    if(auto itSender = m_mSenders.find(localAddress.Get()); itSender != m_mSenders.end())
    {
        itSender->second->AddSdp(sSdp);
    }
    else
    {
        AddSender(localAddress, std::chrono::milliseconds(30000), sSdp);
    }
}

void SapServerImpl::RemoveSdp(const IpAddress& localAddress, const std::string& sSdp)
{
    if(auto itSender = m_mSenders.find(localAddress.Get()); itSender != m_mSenders.end())
    {
        itSender->second->RemoveSdp(sSdp);
    }
}

void SapServerImpl::SetSenderDelay(const IpAddress& localAddress, std::chrono::milliseconds delay)
{
    pml::log::log(pml::log::Level::kDebug, "pml::sapserver") << "SetSenderDelay: " << localAddress.Get();
    if(auto itSender = m_mSenders.find(localAddress.Get()); itSender != m_mSenders.end())
    {
        itSender->second->SetDelay(delay);
        pml::log::log(pml::log::Level::kDebug, "pml::sapserver") << delay.count() << "ms";
    }
    else
    {
        pml::log::log(pml::log::Level::kWarning, "pml::sapserver") << "Sender not found";
    }

}


void SapServerImpl::RemoveSender(const IpAddress& localAddress)
{
    pml::log::log(pml::log::Level::kDebug, "pml::sapserver") << "RemoveSender: " << localAddress.Get();
    if(auto itSender = m_mSenders.find(localAddress.Get()); itSender != m_mSenders.end())
    {
        m_mSenders.erase(localAddress.Get());
    }
    else
    {
        pml::log::log(pml::log::Level::kWarning, "pml::sapserver") << "Sender not found";
    }
}

void SapServerImpl::RemoveAllSenders()
{
    pml::log::log(pml::log::Level::kDebug, "pml::sapserver") << "RemoveAllSenders";
    m_mSenders.clear();

}


void SapServerImpl::Run()
{
    if(m_context.stopped())
    {
        pml::log::log(pml::log::Level::kInfo, "pml::sapserver") << "Restart context";
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
        pml::log::log(pml::log::Level::kInfo, "pml::sapserver") <<  "Run context";
        m_context.run();
        pml::log::log(pml::log::Level::kDebug, "pml::sapserver") << "Context stopped";
    }
    catch (const std::exception& e)
    {
        pml::log::log(pml::log::Level::kError, "pml::sapserver") << "Failed to run context: " << e.what();
    }
}
void SapServerImpl::Stop()
{
    pml::log::log(pml::log::Level::kInfo, "pml::sapserver") << "Stop context";
    m_context.stop();
    if(m_pThread)
    {
        m_pThread->join();
    }
    m_pThread = nullptr;
}

bool SapServerImpl::IsStopped()
{
    return m_context.stopped();
}


void SapServerImpl::AddReceiver(const IpAddress& multicastAddress, const IpAddress& listenAddress)
{
    pml::log::log(pml::log::Level::kDebug, "pml::sapserver") << "AddReceiver: " << multicastAddress.Get();

    auto pairReceiver = m_mReceivers.try_emplace(multicastAddress.Get(), std::unique_ptr<Receiver>( new Receiver(m_context, std::make_shared<Parser>(m_pHandler))));

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
    pml::log::log(pml::log::Level::kDebug, "pml::sapserver") <<  "RemoveReceiver: " << multicastAddress.Get();
    m_mReceivers.erase(multicastAddress.Get());
}

