#pragma once

#include <iostream>
#include <string>
#include <mysql.h>

class Cpu
{
public:
    Cpu();
    ~Cpu();
    void setnbrCore(std::string cpunbrCore);
    void setM_model_cpu(MYSQL* connexion, std::string cpuM_model_cpu);
    void UP(MYSQL* connexion, char* rreport);

private:
    std::string m_nbrCore;
    std::string m_model_cpu;
};