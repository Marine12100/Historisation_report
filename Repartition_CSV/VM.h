#pragma once

#include <string>
#include <mysql.h>
#include <iostream>

class Vm
{
public:
    Vm();
    ~Vm();
    void setID_service(MYSQL* connexion, std::string VmOrg, std::string VmOrgFullName);
    void setUUID(std::string VmUUID);
    void setUID(std::string VmUID);
    void setVmName(std::string VmvmName);
    void sethostName(std::string VmhostName);
    void setM_OS(MYSQL* connexion, std::string VmM_OS);
    void setM_SLA(MYSQL* connexion, std::string VmM_SLA);
    void setguestOSCustomization(std::string VmguestOSCustomization);
    void setM_HWVersion(MYSQL* connexion, std::string VmM_HWVersion);
    void UP(MYSQL* connexion);
    std::string getID_vm();

private:
    std::string m_ID_service;
    std::string m_UUID;
    std::string m_UID;
    std::string m_OS;
    std::string m_vmName;
    std::string m_hostName;
    std::string m_SLA;
    std::string m_guestOSCustomization;
    std::string m_HWVersion;
    std::string m_vmware_Tools;
    std::string m_ID_vm;
};