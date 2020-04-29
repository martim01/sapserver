#pragma once

namespace sapserver
{
    class Message
    {
        public:
            Message(uint16_t nVersion, const std::string& sIp, const std::string& sSDP);
            ~Message();
            const std::vector<uint8_t>& GetFinalMessage() const
            {
                return m_vMsg;
            }
      private:
        std::vector<uint8_t> m_vMsg;
        uint16_t m_nVersion;
    }


};
