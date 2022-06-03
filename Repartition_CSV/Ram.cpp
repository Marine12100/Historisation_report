#include "Ram.h"

using namespace std;

Ram::Ram() : m_capacity_ram(""), m_unit("")
{
}

Ram::~Ram()
{
}

void Ram::setcapacity_ram(MYSQL* connexion, std::string ramcapacity_ram)
{
    int capacity = stoi(ramcapacity_ram);
    string ram_unit = "";

    if (capacity <= 0)
    {
        capacity *= 1000;
        ram_unit = "Mo";
    }
    else if (capacity > 1000)
    {
        capacity /= 1000;
        ram_unit = "To";
    }
    else
    {
        ram_unit = "Go";
    }

    m_capacity_ram = to_string(capacity);

    /***** SELECT ID_metrics *****/

    char qSELECT_ID_metrics[500] = "";
    char rID_metrics[500] = "";

    sprintf_s(qSELECT_ID_metrics, "SELECT ID_metrics FROM metrics where type_metrics = 'unit' AND correspondence_metrics = '%s';", ram_unit.c_str());
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
        cout << "Metrique de la capacité da la ram correspondante inexistante" << endl;
        cout << qSELECT_ID_metrics << endl;
        exit(0);
    }

    m_unit = rID_metrics;

    mysql_free_result(RES_ID_metrics);
}

void Ram::UP(MYSQL* connexion, char* rreport)
{
    /***** INSERT ram *****/

    char qINSERT_ram[500] = "";

    sprintf_s(qINSERT_ram, "INSERT INTO ram (ID_report, capacity_ram, M_unit) VALUES ('%s', '%s', '%s');", rreport, m_capacity_ram.c_str(), m_unit.c_str());
    mysql_query(connexion, qINSERT_ram);

    if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
    {
        fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
        cout << qINSERT_ram << endl;
        exit(0);
    }
}