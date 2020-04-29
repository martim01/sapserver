#include "sapserver.h"
#include "sapserverimpl.h"

using namespace sapserver;

SapServer::SapServer(std::shared_ptr<Handler> pHandler) :
    m_pImpl(std::unique_ptr<SapServerImpl>(new SapServerImpl(pHandler)))
{

}

SapServer::~SapServer()
{
}

void SapServer::AddSender(const IpAddress& localAddress, std::chrono::milliseconds delay, const std::string& sSDP)
{
   m_pImpl->AddSender(localAddress, delay,sSDP);
}

void SapServer::SetSenderDelay(const IpAddress& localAddress, std::chrono::milliseconds delay)
{
    m_pImpl->SetSenderDelay(localAddress, delay);
}

void SapServer::SetSenderSDP(const IpAddress& localAddress, const std::string& sSDP)
{
    m_pImpl->SetSenderSDP(localAddress, sSDP);
}

void SapServer::RemoveSender(const IpAddress& localAddress)
{
    m_pImpl->RemoveSender(localAddress);
}

void SapServer::RemoveAllSenders()
{
    m_pImpl->RemoveAllSenders();
}

void SapServer::Run()
{
    m_pImpl->Run();
}


void SapServer::Stop()
{
    m_pImpl->Stop();
}

bool SapServer::IsStopped()
{
    return m_pImpl->IsStopped();
}


void SapServer::AddReceiver(const IpAddress& multicastAddress)
{
    m_pImpl->AddReceiver(multicastAddress);
}

void SapServer::RemoveReceiver(const IpAddress& multicastAddress)
{
    m_pImpl->RemoveReceiver(multicastAddress);
}
