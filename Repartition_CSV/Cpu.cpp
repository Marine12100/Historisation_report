#include "Cpu.h"

using namespace std;

Cpu::Cpu() : m_nbrCore(""), m_model_cpu("")
{
}

Cpu::~Cpu()
{
}

void Cpu::setnbrCore(std::string cpunbrCore)
{
    int CorINT = stoi(cpunbrCore); //conversion vers INT pour éviter problèmes d'insertion de nombre à virgule

    m_nbrCore = to_string(CorINT); //conversion pour adaptation à la classe
}

void Cpu::setM_model_cpu(MYSQL* connexion, std::string cpuM_model_cpu)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_model_cpu[500] = "";
    char rM_model_cpu[500] = "";

    sprintf_s(qSELECT_M_model_cpu, "SELECT ID_metrics FROM metrics where type_metrics = 'model_cpu' AND correspondence_metrics = '%s';", cpuM_model_cpu.c_str());
    mysql_query(connexion, qSELECT_M_model_cpu);

    MYSQL_RES* RES_M_model_cpu = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_model_cpu = NULL;

    while (ROW_M_model_cpu = mysql_fetch_row(RES_M_model_cpu))
    {
        unsigned long* L_M_model_cpu = mysql_fetch_lengths(RES_M_model_cpu);

        for (int i = 0; i < 1; i++)
            sprintf_s(rM_model_cpu, "%.*s", (int)L_M_model_cpu[i], ROW_M_model_cpu[i] ? ROW_M_model_cpu[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_model_cpu) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_METRICS[500] = "";

        sprintf_s(qINSERT_METRICS, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('model_cpu', '%s');", cpuM_model_cpu.c_str());
        mysql_query(connexion, qINSERT_METRICS);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_METRICS << endl;
            exit(0);
        }

        /***** SELECT NEW_M_model_cpu *****/

        mysql_query(connexion, qSELECT_M_model_cpu);

        MYSQL_RES* RES_NEW_M_model_cpu = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_M_model_cpu = NULL;

        while (ROW_NEW_M_model_cpu = mysql_fetch_row(RES_NEW_M_model_cpu))
        {
            unsigned long* L_NEW_M_model_cpu = mysql_fetch_lengths(RES_NEW_M_model_cpu);

            for (int i = 0; i < 1; i++)
                sprintf_s(rM_model_cpu, "%.*s", (int)L_NEW_M_model_cpu[i], ROW_NEW_M_model_cpu[i] ? ROW_NEW_M_model_cpu[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_model_cpu) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique de TODO correspondant inexistant" << endl;
            cout << qSELECT_M_model_cpu << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_model_cpu);
    }

    m_model_cpu = rM_model_cpu;

    mysql_free_result(RES_M_model_cpu);
}

void Cpu::UP(MYSQL* connexion, char* rreport)
{
    /***** INSERT cpu *****/

    char qINSERT_cpu[500] = "";

    sprintf_s(qINSERT_cpu, "INSERT INTO cpu (ID_report, M_model_cpu, nbrCore) VALUES ('%s', '%s', '%s');", rreport, m_model_cpu.c_str(), m_nbrCore.c_str());
    mysql_query(connexion, qINSERT_cpu);

    if (mysql_errno(connexion) != 0) // SI ERREUR REQUETE
    {
        fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
        cout << qINSERT_cpu << endl;
        exit(0);
    }
}