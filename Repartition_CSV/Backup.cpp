#include "Backup.h"

using namespace std;

Backup::Backup() : m_type_backup("")
{
}

Backup::~Backup()
{
}

void Backup::setM_type_backup(MYSQL* connexion, std::string backupM_type_backup)
{
    /***** SELECT M_type_backup *****/

    char qSELECT_M_type_backup[500] = "";
    char rM_type_backup[500] = "";

    sprintf_s(qSELECT_M_type_backup, "SELECT ID_metrics FROM metrics where type_metrics = 'type_backup' AND correspondence_metrics = '%s';", backupM_type_backup.c_str());
    mysql_query(connexion, qSELECT_M_type_backup);

    MYSQL_RES* RES_M_type_backup = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_type_backup = NULL;

    while (ROW_M_type_backup = mysql_fetch_row(RES_M_type_backup))
    {
        unsigned long* L_M_type_backup = mysql_fetch_lengths(RES_M_type_backup);

        for (int i = 0; i < 1; i++)
            sprintf_s(rM_type_backup, "%.*s", (int)L_M_type_backup[i], ROW_M_type_backup[i] ? ROW_M_type_backup[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_type_backup) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_M_type_backup[500] = "";

        sprintf_s(qINSERT_M_type_backup, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('type_backup', '%s');", backupM_type_backup.c_str());
        mysql_query(connexion, qINSERT_M_type_backup);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_M_type_backup << endl;
            exit(0);
        }

        /***** SELECT ID_metrics *****/

        mysql_query(connexion, qSELECT_M_type_backup);

        MYSQL_RES* RES_NEW_M_type_backup = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_M_type_backup = NULL;

        while (ROW_NEW_M_type_backup = mysql_fetch_row(RES_NEW_M_type_backup))
        {
            unsigned long* L_NEW_M_type_backup = mysql_fetch_lengths(RES_NEW_M_type_backup);

            for (int i = 0; i < 1; i++)
                sprintf_s(rM_type_backup, "%.*s", (int)L_NEW_M_type_backup[i], ROW_NEW_M_type_backup[i] ? ROW_NEW_M_type_backup[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_type_backup) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique du type de backup correspondant inexistant" << endl;
            cout << qSELECT_M_type_backup << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_type_backup);
    }

    m_type_backup = rM_type_backup;

    mysql_free_result(RES_M_type_backup);
}

void Backup::UP(MYSQL* connexion, char* rreport)
{
    /***** INSERT backup *****/

    char qINSERT_backup[500] = "";

    sprintf_s(qINSERT_backup, "INSERT INTO backup (ID_report, M_type_backup) VALUES ('%s', '%s');", rreport, m_type_backup.c_str());
    mysql_query(connexion, qINSERT_backup);

    if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
    {
        fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
        cout << qINSERT_backup << endl;
        exit(0);
    }
}