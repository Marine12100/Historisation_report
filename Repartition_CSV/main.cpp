#include "Header.h"

using namespace std;

int main()
{
    DIR* rep;
    struct dirent* fichierLu = NULL;

    rep = opendir("C:\\Users\\m.brioude\\OneDrive - QUADRIA\\Documents\\QUADRIA\\Tâches\\En cour\\Interface CSV\\CSV client 37");

    //ouverture du repertoire

    if (rep == NULL) //echec d'ouverture
        perror(""); //message d'erreur

    while ((fichierLu = readdir(rep)) != NULL)
    {
        if (fichierLu->d_name != "." || fichierLu->d_name != "..")
        {
            string name = fichierLu->d_name;

            cout << "Traitement du fichier " << name << "...  ";

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

    if (closedir(rep) == -1) //fermeture du repertoire
        exit(-1);
}