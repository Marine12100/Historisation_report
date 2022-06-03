#include "Disk.h"

using namespace std;

Disk::Disk() : m_capacity_disk(""), m_type_disk(""), m_unit("")
{
}

Disk::~Disk()
{
}

void Disk::setcapacity_disk(MYSQL* connexion, std::string diskcapacity_disk)
{
    int capacity = stoi(diskcapacity_disk);
    string disk_unit = "";

    if (capacity <= 0)
    {
        capacity *= 1000;
        disk_unit = "Mo";
    }
    else if (capacity > 1000)
    {
        capacity /= 1000;
        disk_unit = "To";
    }
    else
    {
        disk_unit = "Go";
    }

    m_capacity_disk = to_string(capacity);

    /***** SELECT ID_metrics *****/

    char qSELECT_ID_metrics[500] = "";
    char rID_metrics[500] = "";

    sprintf_s(qSELECT_ID_metrics, "SELECT ID_metrics FROM metrics where type_metrics = 'unit' AND correspondence_metrics = '%s';", disk_unit.c_str());
    mysql_query(connexion, qSELECT_ID_metrics);

    MYSQL_RES* RES_ID_metrics = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_metrics = NULL;

    while (ROW_ID_metrics = mysql_fetch_row(RES_ID_metrics))
    {
        unsigned long* L_ID_metrics = mysql_fetch_lengths(RES_ID_metrics);

        for (int i = 0; i < 1; i++)
            sprintf_s(rID_metrics, "%.*s", (int)L_ID_metrics[i], ROW_ID_metrics[i] ? ROW_ID_metrics[i] : "NULL");
    }

    if (mysql_num_rows(RES_ID_metrics) == 0) // SI REQUETE VIDE
    {
        cout << "Metrique de la capacité du disk correspondant inexistant" << endl;
        cout << qSELECT_ID_metrics << endl;
        exit(0);
    }

    m_unit = rID_metrics;

    mysql_free_result(RES_ID_metrics);
}

void Disk::setM_type_disk(MYSQL* connexion, std::string diskM_type_disk)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_type_disk[500] = "";
    char rM_type_disk[500] = "";

    sprintf_s(qSELECT_M_type_disk, "SELECT ID_metrics FROM metrics where type_metrics = 'type_disk' AND correspondence_metrics = '%s';", diskM_type_disk.c_str());
    mysql_query(connexion, qSELECT_M_type_disk);

    MYSQL_RES* RES_M_type_disk = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_type_disk = NULL;

    while (ROW_M_type_disk = mysql_fetch_row(RES_M_type_disk))
    {
        unsigned long* L_M_type_disk = mysql_fetch_lengths(RES_M_type_disk);

        for (int i = 0; i < 1; i++)
            sprintf_s(rM_type_disk, "%.*s", (int)L_M_type_disk[i], ROW_M_type_disk[i] ? ROW_M_type_disk[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_type_disk) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_M_type_disk[500] = "";

        sprintf_s(qINSERT_M_type_disk, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('type_disk', '%s');", diskM_type_disk.c_str());
        mysql_query(connexion, qINSERT_M_type_disk);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_M_type_disk << endl;
            exit(0);
        }

        /***** SELECT NEW_ID_metrics *****/

        mysql_query(connexion, qSELECT_M_type_disk);

        MYSQL_RES* RES_NEW_M_type_disk = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_M_type_disk = NULL;

        while (ROW_NEW_M_type_disk = mysql_fetch_row(RES_NEW_M_type_disk))
        {
            unsigned long* L_NEW_M_type_disk = mysql_fetch_lengths(RES_NEW_M_type_disk);

            for (int i = 0; i < 1; i++)
                sprintf_s(rM_type_disk, "%.*s", (int)L_NEW_M_type_disk[i], ROW_NEW_M_type_disk[i] ? ROW_NEW_M_type_disk[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_type_disk) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique du type du disk correspondant inexistante" << endl;
            cout << qSELECT_M_type_disk << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_type_disk);
    }

    m_type_disk = rM_type_disk;

    mysql_free_result(RES_M_type_disk);
}

void Disk::UP(MYSQL* connexion, char* rreport)
{
    /***** INSERT disk *****/

    char qINSERT_disk[500] = "";

    sprintf_s(qINSERT_disk, "INSERT INTO disk (ID_report, capacity_disk, M_type_disk, M_unit) VALUES ('%s', '%s', '%s', '%s');", rreport, m_capacity_disk.c_str(), m_type_disk.c_str(), m_unit.c_str());
    mysql_query(connexion, qINSERT_disk);

    if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
    {
        fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
        cout << qINSERT_disk << endl;
        exit(0);
    }
}