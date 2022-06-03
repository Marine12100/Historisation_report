#pragma once

#include <iostream>
#include <mysql.h>
#include <string>

class Backup
{
public:
    Backup();
    ~Backup();
    void setM_type_backup(MYSQL* connexion, std::string backupM_type_backup);
    void UP(MYSQL* connexion, char* rreport);

private:
    std::string m_type_backup;
};