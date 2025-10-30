#include "sapserver.h"

#include "sapserverimpl.h"


using namespace pml::sap;

Server::Server(std::shared_ptr<Handler> pHandler, bool bThreaded) :
    m_pImpl(std::unique_ptr<SapServerImpl>(new SapServerImpl(pHandler, bThreaded)))
{

}

Server::~Server()
{
}

void Server::AddSender(const IpAddress& localAddress, std::chrono::milliseconds delay, const std::string& sSDP)
{
   m_pImpl->AddSender(localAddress, delay,sSDP);
}

void Server::AddSdp(const IpAddress& localAddress, const std::string& sSDP)
{
    m_pImpl->AddSdp(localAddress, sSDP);
}

void Server::RemoveSdp(const IpAddress& localAddress, const std::string& sSDP)
{
    m_pImpl->RemoveSdp(localAddress, sSDP);
}

void Server::SetSenderDelay(const IpAddress& localAddress, std::chrono::milliseconds delay)
{
    m_pImpl->SetSenderDelay(localAddress, delay);
}


void Server::RemoveSender(const IpAddress& localAddress)
{
    m_pImpl->RemoveSender(localAddress);
}

void Server::RemoveAllSenders()
{
    m_pImpl->RemoveAllSenders();
}

void Server::Run()
{
    m_pImpl->Run();
}


void Server::Stop()
{
    m_pImpl->Stop();
}

bool Server::IsStopped()
{
    return m_pImpl->IsStopped();
}


void Server::AddReceiver(const IpAddress& multicastAddress, const IpAddress& listenAddress)
{
    m_pImpl->AddReceiver(multicastAddress, listenAddress);
}

void Server::RemoveReceiver(const IpAddress& multicastAddress)
{
    m_pImpl->RemoveReceiver(multicastAddress);
}
