#define _CRT_SECURE_NO_WARNINGS

#include <mysql.h>
#include "Util.h"

using namespace std;

int main()
{
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);

    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "vcloud_csv", 0, NULL, 0))
    {
        const char* chemin = "C:\\Users\\marin\\Documents\\CSV manstre\\Historisation_report\\Repartition_CSV"; // chemin absolu de recherche de rapports
        Util::exploration(&connexion, chemin);

        mysql_close(&connexion); //fermeture connection
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    SetThreadExecutionState(ES_CONTINUOUS);

    return(0);
}