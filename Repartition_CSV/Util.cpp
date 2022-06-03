#include "Util.h"

using namespace std;

void Util::exploration(MYSQL* connexion, const char* chemin)
{
    char chaine[4096];

    DIR* rep = opendir(chemin);
    struct dirent* fichierLu = NULL;

    //ouverture du repertoire

    if (rep != NULL)
    {
        while ((fichierLu = readdir(rep)) != NULL) //jusqu'à la fin du dossier
        {
            if (fichierLu != NULL)
            {
                if (fichierLu->d_type == DT_DIR) // si le fichier parcouru est un repertoire
                {
                    if (strcmp(fichierLu->d_name, ".") != 0 && strcmp(fichierLu->d_name, "..") != 0)
                    {
                        strcpy_s(chaine, chemin);
                        strcat_s(chaine, "/");
                        strcat_s(chaine, fichierLu->d_name);

                        Util::exploration(connexion, chaine);
                    }
                }
                else // si le fichier parcouru n'est pas un repertoire
                {
                    if ((strstr(fichierLu->d_name, "VCloud_Report")) != NULL && (strstr(fichierLu->d_name, ".csv")) != NULL) // verifier que c'est un rapport
                    {
                        cout << "Traitement du fichier " << fichierLu->d_name << "...  ";

                        char name[500] = "";

                        sprintf_s(name, "%s\\%s", chemin, fichierLu->d_name);

                        ifstream data(name); // ouverture csv

                        if (data.is_open())
                        {
                            string line = "";

                            while (getline(data, line)) // acquisition d'une ligne
                            {
                                vector <string> tableau = Util::creation(line);

                                if (tableau.size() >= 33 && tableau[0] != "UUID" && tableau[1] != "UID") // verifier que ce n'est pas une ligne d'en-tete
                                {
                                    Vm Lvm;
                                    Report Lreport;
                                    Disk Ldisk;
                                    Ram Lram;
                                    Cpu Lcpu;
                                    Backup Lbackup;
                                    Network Lnetwork;

                                    Util::repartition(connexion, tableau, Lvm, Lreport, Ldisk, Lram, Lcpu, Lbackup, Lnetwork); // repartir les cellules
                                    Util::insertion(connexion, Lvm, Lreport, Ldisk, Lram, Lcpu, Lbackup, Lnetwork); // inserer les objets dans la bdd
                                }
                            }
                            cout << "Done" << endl;

                            if (mysql_ping(connexion) != 0) // VERIFICATION DE LA CONNEXION
                            {
                                fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(connexion));
                            }
                        }
                        else
                        {
                            cout << "Erreur d'ouverture du document" << endl;
                        }
                    }
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

std::vector<std::string> Util::creation(std::string line)
{
    vector <string> tableau;
    stringstream lineStream(line);
    string cell;

    while (getline(lineStream, cell, ','))
    {
        tableau.push_back(cell); //insérer la cellule dans le tableau
    }

    if (size(tableau) > 33)
    {
        int i = 0;

        do
        {
            /***** RECHERCHE ADRESSE MAC OU IP *****/

            //expression régulière d'une adresse MAC
            regex rgxMAC("(([0-9A-Fa-f]{2}[:]){5}[0-9A-Fa-f]{2})");

            //recherche de deux adresses MAC à la suite dans le tableau
            bool MAC1 = regex_search(tableau[i], rgxMAC);
            bool MAC2 = regex_search(tableau[i+1], rgxMAC);

            //expression régulière d'une adresse IPv4
            regex rgxIPv4("([0-9]{1,3}[.]){3}[0-9]{1,3}");

            //recherche de deux adresses IPv4 à la suite dans le tableau
            bool IPv4_1 = regex_search(tableau[i], rgxIPv4);
            bool IPv4_2 = regex_search(tableau[i+1], rgxIPv4);

            //expression régulière d'une adresse IPv6
            regex rgxIPv6("(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))");

            //recherche de deux adresses IPv6 à la suite dans le tableau
            bool IPv6_1 = regex_search(tableau[i], rgxIPv6);
            bool IPv6_2 = regex_search(tableau[i], rgxIPv6);


            if (MAC1 && MAC2) //si deux MAC à la suite trouvées
            {
                tableau[i] = tableau[i] + tableau[i+1]; //concaténation des MAC
                tableau.erase(tableau.begin() + (i+1));
            }
            else if (IPv4_1 && IPv4_2) //si deux IPv4 à la suite trouvées
            {
                tableau[i] = tableau[i] + "/" + tableau[i+1]; //concaténation des IPv4
                tableau.erase(tableau.begin() + (i+1));
            }
            else if (IPv6_1 && IPv6_2) //si deux IPv6 à la suite trouvées
            {
                tableau[i] = tableau[i] + "/" + tableau[i+1]; //concaténation des IPv6
                tableau.erase(tableau.begin() + (i+1));
            }
            else //si non trouvées
            {
                i++;
            }

        } while (tableau.size() > 33); //tant que le tableau contient plus de 33 éléments
    }

    for (size_t i = 0; i < tableau.size(); i++)
    {
        tableau[i].erase(remove(tableau[i].begin(), tableau[i].end(), '"'), tableau[i].end()); //supprimer les guillemets doubles de la cellule
        tableau[i].erase(remove(tableau[i].begin(), tableau[i].end(), '\''), tableau[i].end()); //supprimer les guillemets simples de la cellule
    }

    return tableau;
}

void Util::repartition(MYSQL* connexion, std::vector<std::string>& tableau, Vm& Lvm, Report& Lreport, Disk& Ldisk, Ram& Lram, Cpu& Lcpu, Backup& Lbackup, Network& Lnetwork)
{
    Lvm.setUUID(tableau[0]);
    Lvm.setUID(tableau[1]);
    Lvm.setVmName(tableau[2]);
    Lvm.setID_service(connexion, tableau[3], tableau[4]);
    Lvm.sethostName(tableau[5]);
    Lnetwork.setIP_address(tableau[6]);
    Lnetwork.setMAC_address(tableau[7]);
    Lreport.setPowerOn(tableau[8]);
    Lvm.setM_OS(connexion, tableau[9]);
    Lbackup.setM_type_backup(connexion, tableau[11]);
    Lcpu.setnbrCore(tableau[12]);
    Lcpu.setM_model_cpu(connexion, tableau[13]);
    Lram.setcapacity_ram(connexion, tableau[14]);
    Ldisk.setcapacity_disk(connexion, tableau[15]);
    Lvm.setM_SLA(connexion, tableau[16]);
    Ldisk.setM_type_disk(connexion, tableau[17]);
    Lreport.setRptDateHour(tableau[19], tableau[20]);
    Lreport.setvApp(tableau[27]);
    Lvm.setguestOSCustomization(tableau[28]);
    Lvm.setM_HWVersion(connexion, tableau[29]);
}

void Util::insertion(MYSQL* connexion, Vm& Lvm, Report& Lreport, Disk& Ldisk, Ram& Lram, Cpu& Lcpu, Backup& Lbackup, Network& Lnetwork)
{
    Lvm.UP(connexion);

    Lreport.setID_vm(Lvm);
    Lreport.UP(connexion, Ldisk, Lram, Lcpu, Lbackup, Lnetwork);
}
