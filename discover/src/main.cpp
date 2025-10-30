#include <chrono>
#include <iostream>
#include <thread>

#include "sapserver.h"
#include "handler.h"

using namespace std;

int main()
{

    pml::sap::Server ss(std::make_shared<pml::StdHandler>());
    std::cout << "AddReceiver" << std::endl;
    ss.AddReceiver(IpAddress("239.255.255.255"));
    ss.Run();
    getchar();

    return 0;
}
