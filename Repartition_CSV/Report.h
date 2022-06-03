#pragma once

#include <string>
#include <mysql.h>
#include "VM.h"
#include "Disk.h"
#include "Ram.h"
#include "Cpu.h"
#include "Backup.h"
#include "Network.h"

class Report
{
public:
    Report();
    ~Report();
    void setID_vm(Vm& Lvm);
    void setRptDateHour(std::string reportRptDate, std::string reportRptHour);
    void setvApp(std::string reportvApp);
    void setPowerOn(std::string reportPowerOn);
    void UP(MYSQL* connexion, Disk Ldisk, Ram Lram, Cpu Lcpu, Backup Lbackup, Network Lnetwork);

private:
    std::string m_ID_vm;
    std::string m_RptDateHour;
    std::string m_vApp;
    std::string m_PowerOn;
};