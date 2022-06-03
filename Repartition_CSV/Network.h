#pragma once

#include <string>
#include <mysql.h>
#include <vector>
#include <iostream>
#include <sstream>

#define MAC_LENGH 17

class Network
{
public:
    Network();
    ~Network();
    void setIP_address(std::string networkIP_address);
    void setMAC_address(std::string networkMAC_address);
    void UP(MYSQL* connexion, char* rreport);

private:
    std::string m_MAC_address;
    std::string m_IP_address;
};