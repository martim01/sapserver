#include "message.h"

using namespace sapserver;

Message::Message(unsigned short nVersion, const std::string& sIp, const std::string& sSDP) :
    m_nVersion(nVersion)
{

}

Message::~Message(void)
{
}


