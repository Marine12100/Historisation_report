#include "Header.h"

using namespace std;

int main()
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "192.168.1.101", "admin", "root", "mix", 0, NULL, 0))
    {
        const char* chemin = "C:\\Resourcesv2\\juin - decembre\\septembre - decembre"; // chemin absolu de recherche de rapports
        exploration(&connexion, chemin);

        mysql_close(&connexion); //fermeture connection
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }
    return(0);
}