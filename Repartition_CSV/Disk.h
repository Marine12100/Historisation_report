#pragma once

#include <string>
#include <mysql.h>
#include <iostream>

class Disk
{
public:
    Disk();
    ~Disk();
    void setcapacity_disk(MYSQL* connexion, std::string diskcapacity_disk);
    void setM_type_disk(MYSQL* connexion, std::string diskM_type_disk);
    void UP(MYSQL* connexion, char* rreport);

private:
    std::string m_capacity_disk;
    std::string m_type_disk;
    std::string m_unit;
};