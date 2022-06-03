#pragma once

#include <mysql.h>
#include <string>
#include <iostream>

class Ram
{
public:
    Ram();
    ~Ram();
    void setcapacity_ram(MYSQL* connexion, std::string ramcapacity_ram);
    void UP(MYSQL* connexion, char* rreport);

private:
    std::string m_capacity_ram;
    std::string m_unit;
};