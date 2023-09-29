#include <iostream>
#include "sapserver.h"
#include <chrono>
#include <thread>
#include "handler.h"

using namespace std;

int main()
{

    pml::sap::Server ss(std::make_shared<pml::StdHandler>());
    std::cout << "AddReceiver" << std::endl;
    ss.AddReceiver(IpAddress("239.255.255.255"));
    ss.Run();
    getchar();
    std::cout << "AddSender" << std::endl;
    ss.AddSender(IpAddress("192.168.0.218"), std::chrono::milliseconds(5000), "v=0\r\no=- 1423986 1423994 IN IP4 192.168.0.218\r\ns=AOIP44-serial-1614 : 2\r\nc=IN IP4 239.65.45.154/32\r\nt=0 0\r\na=keywds:Dante\r\nm=audio 5004 RTP/AVP 97\r\ni=2 channels: TxChan 0, TxChan 1\r\na=recvonly\r\na=rtpmap:97 L24/48000/2\r\na=ptime:1\r\na=ts-refclk:ptp=IEEE1588-2008:00-00-00-FF-FE-00-00-00:0\r\na=mediaclk:direct=142410716");
    getchar();
    std::cout << "Change SDP" << std::endl;
    ss.SetSenderSDP(IpAddress("192.168.0.218"), "v=0\r\no=- 1423986 1423994 IN IP4 192.168.0.218\r\ns=UpdatedTest : 2\r\nc=IN IP4 239.65.45.154/32\r\nt=0 0\r\na=keywds:Dante\r\nm=audio 5004 RTP/AVP 97\r\ni=2 channels: TxChan 0, TxChan 1\r\na=recvonly\r\na=rtpmap:97 L24/48000/2\r\na=ptime:1\r\na=ts-refclk:ptp=IEEE1588-2008:00-00-00-FF-FE-00-00-00:0\r\na=mediaclk:direct=142410716");
    getchar();
    std::cout << "RemoveSender" << std::endl;

    ss.RemoveSender(IpAddress("192.168.0.218"));
    getchar();


    return 0;
}
