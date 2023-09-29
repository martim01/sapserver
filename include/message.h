#pragma once
#include <stdint.h>
#include <vector>
#include <string>

namespace sapserver
{
    namespace sap
    {
        class Message
        {
            public:
                Message(unsigned short nVersion, const std::string& sIp, const std::string& sSDP);
                ~Message();
                const std::vector<uint8_t>& GetFinalMessage() const
                {
                    return m_vMsg;
                }
        private:
            std::vector<uint8_t> m_vMsg;
            unsigned short m_nVersion;
        };

    }
}
