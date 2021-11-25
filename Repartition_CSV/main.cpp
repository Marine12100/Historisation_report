#include "Header.h"

using namespace std;

int main()
{
    DIR* rep;
    struct dirent* fichierLu = NULL;

    const char* chemin = "C:\\Users\\m.brioude\\OneDrive - QUADRIA\\Documents\\QUADRIA\\Tâches\\En cour\\Interface CSV\\CSV client 37";

    rep = opendir(chemin);

    //ouverture du repertoire

    if (rep != NULL)
    {
        while ((fichierLu = readdir(rep)) != NULL)
        {
            if ((strchr(fichierLu->d_name, '.csv')) != NULL)
            {
                cout << "Traitement du fichier " << fichierLu->d_name << "...  ";

                char name[500] = "";

                sprintf(name, "%s\\%s", chemin, fichierLu->d_name);

                ifstream data(name);

                if (data.is_open())
                {
                    string line = "";

                    while (getline(data, line))
                    {
                        vm Lvm;
                        report Lreport;
                        disk Ldisk;
                        ram Lram;
                        cpu Lcpu;
                        backup Lbackup;
                        network Lnetwork;

                        vector <string> tableau = creation(line);

                        if (tableau[0] != "UUID" && tableau[1] != "UID")
                        {
                            repartition(tableau, Lvm, Lreport, Ldisk, Lram, Lcpu, Lbackup, Lnetwork);
                            insertion(Lvm, Lreport, Ldisk, Lram, Lcpu, Lbackup, Lnetwork);
                        }
                    }
                    cout << "Done" << endl;
                }
                else
                {
                    cout << "Error" << endl;
                }
            }
        }
    }
    else
    {
        perror(""); //message d'erreur
    }

    if (closedir(rep) == -1) //fermeture du repertoire
        exit(-1);
}