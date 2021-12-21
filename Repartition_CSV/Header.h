#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#define MAC_LENGH 17

#include <iostream>
#include <string>
#include <vector>
#include <winsock.h>
#include <mysql.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <atlbase.h>
#include <regex>
#include <dirent.h>
#include <errno.h>


class vm
{
public:
    vm();
    ~vm();
    void setID_service(MYSQL *connexion, std::string VMOrg, std::string VMOrgFullName);
    void setUUID(std::string VMUUID);
    void setUID(std::string VMUID);
    void setvmName(std::string VMvmName);
    void sethostName(std::string VMhostName);
    void setM_OS(MYSQL *connexion, std::string VMM_OS);
    void setM_SLA(MYSQL *connexion, std::string VMM_SLA);
    void setguestOSCustomization(std::string VMguestOSCustomization);
    void setM_HWVersion(MYSQL *connexion, std::string VMM_HWVersion);
    void UP(MYSQL *connexion);
    std::string getID_vm();

private:
    std::string ID_service;
    std::string UUID;
    std::string UID;
    std::string M_OS;
    std::string vmName;
    std::string hostName;
    std::string M_SLA;
    std::string guestOSCustomization;
    std::string M_HWVersion;
    std::string vmware_Tools;
    std::string ID_vm;
};


class disk
{
public:
    disk();
    ~disk();
    void setcapacity_disk(MYSQL *connexion, std::string diskcapacity_disk);
    void setM_type_disk(MYSQL *connexion, std::string diskM_type_disk);
    void UP(MYSQL *connexion, char* rreport);

private:
    std::string capacity_disk;
    std::string M_type_disk;
    std::string M_unit;
};


class ram
{
public:
    ram();
    ~ram();
    void setcapacity_ram(MYSQL *connexion, std::string ramcapacity_ram);
    void UP(MYSQL *connexion, char* rreport);

private:
    std::string capacity_ram;
    std::string M_unit;
};


class cpu
{
public:
    cpu();
    ~cpu();
    void setnbrCore(std::string cpunbrCore);
    void setM_model_cpu(MYSQL *connexion, std::string cpuM_model_cpu);
    void UP(MYSQL *connexion, char* rreport);

private:
    std::string nbrCore;
    std::string M_model_cpu;
};


class backup
{
public:
    backup();
    ~backup();
    void setM_type_backup(MYSQL *connexion, std::string backupM_type_backup);
    void UP(MYSQL *connexion, char* rreport);

private:
    std::string M_type_backup;
};


class network
{
public:
    network();
    ~network();
    void setIP_address(std::string networkIP_address);
    void setMAC_address(std::string networkMAC_address);
    void UP(MYSQL *connexion, char* rreport);

private:
    std::string MAC_address;
    std::string IP_address;
};


class report
{
public:
    report();
    ~report();
    void setID_vm(vm& Lvm);
    void setRptDateHour(std::string reportRptDate, std::string reportRptHour);
    void setvApp(std::string reportvApp);
    void setPowerOn(std::string reportPowerOn);
    void UP(MYSQL *connexion, disk Ldisk ,ram Lram, cpu Lcpu, backup Lbackup, network Lnetwork);

private:
    std::string ID_vm;
    std::string RptDateHour;
    std::string vApp;
    std::string PowerOn;
};


void exploration(MYSQL *connexion, const char* chemin);

std::vector<std::string> creation(std::string line);

void repartition(MYSQL *connexion, std::vector<std::string>& tableau, vm& Lvm, report& Lreport, disk& Ldisk, ram& Lram, cpu& Lcpu, backup& Lbackup, network& Lnetwork);

void insertion(MYSQL *connexion, vm& Lvm, report& Lreport, disk& Ldisk, ram& Lram, cpu& Lcpu, backup& Lbackup, network& Lnetwork);


#endif // !HEADER_H_INCLUDED