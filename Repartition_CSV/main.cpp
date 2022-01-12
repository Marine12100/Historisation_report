#include "Header.h"

using namespace std;

int main()
{
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);

    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "192.168.1.150", "admin", "root", "vCloud_CSV", 0, NULL, 0))
    {
        const char* chemin = "C:\\Resourcesv2"; // chemin absolu de recherche de rapports
        exploration(&connexion, chemin);

        mysql_close(&connexion); //fermeture connection
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    SetThreadExecutionState(ES_CONTINUOUS);

    return(0);
}