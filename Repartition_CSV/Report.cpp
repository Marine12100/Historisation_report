#include "Report.h"

using namespace std;

Report::Report() : m_ID_vm(""), m_RptDateHour(""), m_PowerOn("")
{
}

Report::~Report()
{
}

void Report::setID_vm(Vm& Lvm)
{
    m_ID_vm = Lvm.getID_vm();
}

void Report::setRptDateHour(std::string reportRptDate, std::string reportRptHour)
{
    string jour = reportRptDate.substr(0, 2);
    string mois = reportRptDate.substr(3, 2);
    string annee = reportRptDate.substr(8, 2);

    m_RptDateHour = annee + "/" + mois + "/" + jour + " " + reportRptHour;
}

void Report::setvApp(std::string reportvApp)
{
    m_vApp = reportvApp;
}

void Report::setPowerOn(std::string reportPowerOn)
{
    if (reportPowerOn == "PoweredOn")
    {
        reportPowerOn = "1";
    }
    else if (reportPowerOn == "PoweredOff")
    {
        reportPowerOn = "0";
    }
    else
    {
        reportPowerOn = "0";
    }

    m_PowerOn = reportPowerOn;
}

void Report::UP(MYSQL* connexion, Disk Ldisk, Ram Lram, Cpu Lcpu, Backup Lbackup, Network Lnetwork)
{
    /***** SELECT ID_REPORT *****/

    char qSELECT_ID_REPORT[500] = "";
    char rID_REPORT[500] = "";

    sprintf_s(qSELECT_ID_REPORT, "SELECT ID_report FROM report WHERE ID_vm = '%s' AND RptDateHour = '%s' AND vApp = '%s' AND PowerOn = '%s';", m_ID_vm.c_str(), m_RptDateHour.c_str(), m_vApp.c_str(), m_PowerOn.c_str());
    mysql_query(connexion, qSELECT_ID_REPORT);

    MYSQL_RES* RES_ID_REPORT = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_REPORT = NULL;

    while (ROW_ID_REPORT = mysql_fetch_row(RES_ID_REPORT))
    {
        unsigned long* L_ID_REPORT = mysql_fetch_lengths(RES_ID_REPORT);

        for (int i = 0; i < 1; i++)
            sprintf_s(rID_REPORT, "%.*s", (int)L_ID_REPORT[i], ROW_ID_REPORT[i] ? ROW_ID_REPORT[i] : "NULL");
    }

    if (mysql_num_rows(RES_ID_REPORT) == 0) // SI REPORT INEXISTANT
    {
        /***** INSERT REPORT *****/

        char qINSERT_REPORT[500] = "";

        sprintf_s(qINSERT_REPORT, "INSERT INTO report (ID_vm, RptDateHour, vApp, PowerOn) VALUES ('%s', '%s', '%s', '%s');", m_ID_vm.c_str(), m_RptDateHour.c_str(), m_vApp.c_str(), m_PowerOn.c_str());
        mysql_query(connexion, qINSERT_REPORT);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_REPORT << endl;
            exit(0);
        }

        /***** SELECT NEW_ID_REPORT *****/

        mysql_query(connexion, qSELECT_ID_REPORT);

        MYSQL_RES* RES_NEW_ID_REPORT = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_ID_REPORT = NULL;

        while (ROW_NEW_ID_REPORT = mysql_fetch_row(RES_NEW_ID_REPORT))
        {
            unsigned long* L_NEW_ID_REPORT = mysql_fetch_lengths(RES_NEW_ID_REPORT);

            for (int i = 0; i < 1; i++)
                sprintf_s(rID_REPORT, "%.*s", (int)L_NEW_ID_REPORT[i], ROW_NEW_ID_REPORT[i] ? ROW_NEW_ID_REPORT[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_ID_REPORT) == 0) // SI REQUETE VIDE
        {
            cout << "Rapport correspondant inexistant" << endl;
            cout << qSELECT_ID_REPORT << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_ID_REPORT);

        /***** APPEL DES FONCTIONS ****/

        Ldisk.Disk::UP(connexion, rID_REPORT);

        Lram.Ram::UP(connexion, rID_REPORT);

        Lcpu.Cpu::UP(connexion, rID_REPORT);

        Lbackup.Backup::UP(connexion, rID_REPORT);

        Lnetwork.Network::UP(connexion, rID_REPORT);
    }

    mysql_free_result(RES_ID_REPORT);
}