#include "Header.h"

using namespace std;

int nbrconn;

void exploration(const char* chemin)
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
                        strcpy(chaine, chemin);
                        strcat(chaine, "/");
                        strcat(chaine, fichierLu->d_name);

                        exploration(chaine);
                    }
                }
                else // si le fichier parcouru n'est pas un repertoire
                {
                    if ((strstr(fichierLu->d_name, "VCloud_Report")) != NULL && (strstr(fichierLu->d_name, ".csv")) != NULL) // verifier que c'est un rapport
                    {
                        cout << "Traitement du fichier " << fichierLu->d_name << "...  ";

                        char name[500] = "";

                        sprintf(name, "%s\\%s", chemin, fichierLu->d_name);

                        ifstream data(name); // ouverture csv

                        if (data.is_open())
                        {
                            string line = "";

                            while (getline(data, line)) // acquisition d'une ligne
                            {
                                vm Lvm;
                                report Lreport;
                                disk Ldisk;
                                ram Lram;
                                cpu Lcpu;
                                backup Lbackup;
                                network Lnetwork;

                                vector <string> tableau = creation(line);

                                if (tableau.size() >= 33 && tableau[0] != "UUID" && tableau[1] != "UID") // verifier que ce n'est pas une ligne d'en-tete
                                {
                                    repartition(tableau, Lvm, Lreport, Ldisk, Lram, Lcpu, Lbackup, Lnetwork); // repartir les cellules
                                    insertion(Lvm, Lreport, Ldisk, Lram, Lcpu, Lbackup, Lnetwork); // inserer les objets dans la bdd
                                }
                            }
                            cout << "Done" << endl;

                            /*if (nbrconn >= 65535)
                            {
                                Sleep(2000);
                                nbrconn = 0;
                            }*/
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

vector<string> creation(string line)
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

            //expression régulière d'une adresse IP
            regex rgxIP("([0-9]{1,3}[.]){3}[0-9]{1,3}");

            //recherche de deux adresses MAC à la suite dans le tableau
            bool MAC1 = regex_search(tableau[i], rgxMAC);
            bool MAC2 = regex_search(tableau[i + 1], rgxMAC);

            //recherche de deux adresses IP à la suite dans le tableau
            bool IP1 = regex_search(tableau[i], rgxIP);
            bool IP2 = regex_search(tableau[i + 1], rgxIP);


            if (MAC1 && MAC2) //si deux MAC à la suite trouvées
            {
                tableau[i] = tableau[i] + tableau[i + 1]; //concaténation des MAC
                tableau.erase(tableau.begin() + (i + 1));
            }
            else if (IP1 && IP2) //si deux IP à la suite trouvées
            {
                tableau[i] = tableau[i] + "/" + tableau[i + 1]; //concaténation des IP
                tableau.erase(tableau.begin() + (i + 1));
            }
            else //si non trouvées
            {
                i++;
            }

        } while (tableau.size() > 33); //tant que le tableau contient plus de 33 éléments
    }

    for (size_t i = 0; i < tableau.size(); i++)
    {
        tableau[i].erase(remove(tableau[i].begin(), tableau[i].end(), '"'), tableau[i].end()); //supprimer les guillemets de la cellule
    }

    return tableau;
}

void repartition(vector<string>& tableau, vm &Lvm, report& Lreport, disk& Ldisk, ram& Lram, cpu& Lcpu, backup& Lbackup, network& Lnetwork)
{
    Lvm.setUUID(tableau[0]);
    Lvm.setUID(tableau[1]);
    Lvm.setvmName(tableau[2]);
    Lvm.setID_service(tableau[3], tableau[4]);
    Lvm.sethostName(tableau[5]);
    Lnetwork.setIP_address(tableau[6]);
    Lnetwork.setMAC_address(tableau[7]);
    Lreport.setPowerOn(tableau[8]);
    Lvm.setM_OS(tableau[9]);
    Lbackup.setM_type_backup(tableau[11]);
    Lcpu.setnbrCore(tableau[12]);
    Lcpu.setM_model_cpu(tableau[13]);
    Lram.setcapacity_ram(tableau[14]);
    Ldisk.setcapacity_disk(tableau[15]);
    Lvm.setM_SLA(tableau[16]);
    Ldisk.setM_type_disk(tableau[17]);
    Lreport.setRptDateHour(tableau[19], tableau[20]);
    Lreport.setvApp(tableau[27]);
    Lvm.setguestOSCustomization(tableau[28]);
    Lvm.setM_HWVersion(tableau[29]);
}

void insertion(vm& Lvm, report& Lreport, disk& Ldisk, ram& Lram, cpu& Lcpu, backup& Lbackup, network& Lnetwork)
{
    Lvm.UP();
    Lvm.setID_vm();

    Lreport.setID_vm(Lvm);
    Lreport.UP();

    Ldisk.setID_report(Lreport);
    Ldisk.UP();

    Lram.setID_report(Lreport);
    Lram.UP();

    Lcpu.setID_report(Lreport);
    Lcpu.UP();

    Lbackup.setID_report(Lreport);
    Lbackup.UP();

    Lnetwork.setID_report(Lreport);
    Lnetwork.UP();
}

// VM

vm::vm() : ID_service(""), UUID(""), UID(""), M_OS(""), vmName(""), hostName(""), M_SLA(""), guestOSCustomization(""), M_HWVersion(""), vmware_Tools("")
{
}

vm::~vm()
{
}

void vm::setID_service(string VMOrg, string VMOrgFullName)
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0)) //Connexion BDD
    {
        char id_service[300] = "";
        char qID_service[500] = "";

        //Requête pour obtenir l'ID_service du client associé à la VM
        sprintf(qID_service, "SELECT service.ID_service FROM client JOIN service ON (client.ID_client = service.ID_client) JOIN metrics ON (service.M_description_service = metrics.ID_metrics) WHERE correspondence_metrics = 'vCloud' AND code_client = '%s';", VMOrg.c_str());

        mysql_query(&connexion, qID_service);

        MYSQL_RES* rID_service = mysql_use_result(&connexion);
        MYSQL_ROW row_service;
        unsigned int num_champs = mysql_num_fields(rID_service);        

        while (row_service = mysql_fetch_row(rID_service))
        {
            unsigned long* lengths = mysql_fetch_lengths(rID_service);

            for (int m = 0; m < 1; m++)
            {
                //Récupération valeur
                sprintf(id_service, "%.*s ", (int)lengths[m], row_service[m] ? row_service[m] : "NULL");                
            }
        }

        ID_service = id_service; // donner la valeur de l'id du client à l'id_service de la classe


        /***** SI LE CLIENT N'EXISTE PAS *****/

        if (mysql_num_rows(rID_service) == 0)
        {
            do
            {
                //Requête insertion du nouveau client "fantome"

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO client (code_client, social_reason) VALUES ('%s', '%s');", VMOrg.c_str(), VMOrgFullName.c_str());

                mysql_query(&connexion, qIN);


                //Acquisition de l'ID du client "fantome" créé

                char qID_client[500] = "";
                char id_client[500] = "";

                sprintf(qID_client, "SELECT ID_client FROM client WHERE code_client = '%s' AND social_reason = '%s';", VMOrg.c_str(), VMOrgFullName.c_str());

                mysql_query(&connexion, qID_client);

                MYSQL_RES* rID_client = mysql_use_result(&connexion);
                MYSQL_ROW row_client;
                unsigned int num_champs = mysql_num_fields(rID_client);

                while (row_client = mysql_fetch_row(rID_client))
                {
                    unsigned long* lengths = mysql_fetch_lengths(rID_client);

                    for (int m = 0; m < 1; m++)
                    {
                        //Récupération ID
                        sprintf(id_client, "%.*s ", (int)lengths[m], row_client[m] ? row_client[m] : "NULL");
                    }
                }

                //Recherche de la métrique du service du client

                char qMETRIQUE[500] = "";
                char M_description_service[500] = "";

                //Requete pour recupérer l'ID_metrique vcloud
                sprintf(qMETRIQUE, "SELECT ID_metrics FROM metrics WHERE type_metrics = 'service' AND correspondence_metrics = 'vCloud';");

                mysql_query(&connexion, qMETRIQUE);

                MYSQL_RES* rMETRIQUE = mysql_use_result(&connexion);
                MYSQL_ROW row_METRIQUE;
                unsigned int num_champs_s = mysql_num_fields(rMETRIQUE);

                while (row_METRIQUE = mysql_fetch_row(rMETRIQUE))
                {
                    unsigned long* lengths = mysql_fetch_lengths(rMETRIQUE);

                    for (int s = 0; s < 1; s++)
                    {
                        //Récupération ID
                        sprintf(M_description_service, "%.*s ", (int)lengths[s], row_METRIQUE[s] ? row_METRIQUE[s] : "NULL");
                    }
                }

                //Insertion de la table service du client "fantome" créé

                char qSER[500] = "";

                sprintf(qSER, "INSERT INTO service (ID_client, M_description_service, quantity) VALUES ('%s', '%s', '1');", id_client, M_description_service);

                mysql_query(&connexion, qSER);


                //Acquisition de l'ID de service du client "fantome" créé

                char qID_service[500] = "";
                char id_service[500] = "";

                sprintf(qID_service, "SELECT ID_service FROM service JOIN client ON (client.ID_client = service.ID_client) JOIN metrics ON (service.M_description_service = metrics.ID_metrics) WHERE client.code_client = '%s' AND client.social_reason = '%s' AND metrics.correspondence_metrics = 'vCloud';", VMOrg.c_str(), VMOrgFullName.c_str());

                mysql_query(&connexion, qID_service);

                MYSQL_RES* rID_service = mysql_use_result(&connexion);
                MYSQL_ROW row_service;
                unsigned int num_champs_service = mysql_num_fields(rID_service);

                while (row_service = mysql_fetch_row(rID_service))
                {
                    unsigned long* lengths = mysql_fetch_lengths(rID_service);

                    for (int m = 0; m < 1; m++)
                    {
                        //Récupération ID
                        sprintf(id_service, "%.*s ", (int)lengths[m], row_client[m] ? row_client[m] : "NULL");
                    }
                }
                ID_service = id_service;

            } while (mysql_num_rows(rID_service) != 0); //Tant que le client n'existe pas
        }
        else // mettre à jour les données
        {
            char qUPDATE[500] = "";
            char qID_client[500] = "";
            char id_client[500] = "";

            // Acquisition de l'ID de la vm
            sprintf(qID_client, "SELECT ID_client FROM client WHERE code_client = '%s';", VMOrg.c_str());

            mysql_query(&connexion, qID_client);

            MYSQL_RES* rID_client = mysql_use_result(&connexion);
            MYSQL_ROW row_client;
            unsigned int num_champs = mysql_num_fields(rID_client);

            while (row_client = mysql_fetch_row(rID_client))
            {
                unsigned long* lengths = mysql_fetch_lengths(rID_client);

                for (int m = 0; m < 1; m++)
                {
                    //Récupération ID
                    sprintf(id_client, "%.*s ", (int)lengths[m], row_client[m] ? row_client[m] : "NULL");
                }
            }

            sprintf(qUPDATE, "UPDATE client SET code_client = '%s', social_reason = '%s' WHERE ID_client = '%s';", VMOrg.c_str(), VMOrgFullName.c_str(), id_client);

            mysql_query(&connexion, qUPDATE);
        }
    }
    else //Erreur connexion
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void vm::setUUID(std::string VMUUID)
{
    UUID = VMUUID;
}

void vm::setUID(std::string VMUID)
{
    UID = VMUID;
}

void vm::setvmName(std::string VMvmName)
{
    vmName = VMvmName;
}

void vm::sethostName(std::string VMhostName)
{
    hostName = VMhostName;
}

void vm::setM_OS(std::string VMM_OS)
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0)) //Connexion
    {
        char OS[300] = "";
        char qOS[500] = "";
        char qIN[500] = "";

        //Requête récupération de l'ID de la métrique
        sprintf(qOS, "SELECT ID_metrics FROM metrics where type_metrics = 'OS' AND correspondence_metrics = '%s';", VMM_OS.c_str());

        mysql_query(&connexion, qOS);

        MYSQL_RES* rOS = mysql_use_result(&connexion);
        MYSQL_ROW row_OS;
        unsigned int num_champs = mysql_num_fields(rOS);

        while (row_OS = mysql_fetch_row(rOS))
        {
            unsigned long* lengths = mysql_fetch_lengths(rOS);

            for (int m = 0; m < 1; m++)
            {
                //Récupération de l'ID
                sprintf(OS, "%.*s ", (int)lengths[m], row_OS[m] ? row_OS[m] : "NULL");
            }
        }

        //Si métrique inexistante
        if (mysql_num_rows(rOS) == 0)
        {
            do
            {
                //Requête insertion de la nouvelle métrique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('OS', '%s');", VMM_OS.c_str());

                mysql_query(&connexion, qIN);

                //Récupératrion de l'ID de la nouvelle métrique
                while ((row_OS = mysql_fetch_row(rOS)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rOS);

                    for (int m = 0; m < 1; m++)
                    {
                        //Récupération du nouvel ID
                        sprintf(OS, "%.*s ", (int)lengths[m], row_OS[m] ? row_OS[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rOS) != 0); //Jusqu'à ce que la métrique existe
        }
        VMM_OS = OS;
    }
    else //Erreur Connexion
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    M_OS = VMM_OS;

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void vm::setM_SLA(std::string VMM_SLA)
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0)) //Connexion
    {
        char SLA[300] = "";
        char qSLA[500] = "";
        char qIN[500] = "";

        //Requête récupération de l'ID de la métrique
        sprintf(qSLA, "SELECT ID_metrics FROM metrics where type_metrics = 'SLA' AND correspondence_metrics = '%s';", VMM_SLA.c_str());
        mysql_query(&connexion, qSLA);

        MYSQL_RES* rSLA = mysql_use_result(&connexion);
        MYSQL_ROW row_SLA;
        unsigned int num_champs = mysql_num_fields(rSLA);

        while (row_SLA = mysql_fetch_row(rSLA))
        {
            unsigned long* lengths = mysql_fetch_lengths(rSLA);

            for (int m = 0; m < 1; m++)
            {
                //Récupération de l'ID
                sprintf(SLA, "%.*s ", (int)lengths[m], row_SLA[m] ? row_SLA[m] : "NULL");
            }
        }

        //Si métrique inexistante
        if (mysql_num_rows(rSLA) == 0)
        {
            do
            {
                //Requête insertion de la nouvelle métrique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('SLA', '%s');", VMM_SLA.c_str());

                mysql_query(&connexion, qIN);

                //Récupératrion de l'ID de la nouvelle métrique
                while ((row_SLA = mysql_fetch_row(rSLA)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rSLA);

                    for (int m = 0; m < 1; m++)
                    {
                        //Récupération du nouvel ID
                        sprintf(SLA, "%.*s ", (int)lengths[m], row_SLA[m] ? row_SLA[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rSLA) != 0); //Jusqu'à ce que la métrique existe
        }
        VMM_SLA = SLA;
    }
    else //Erreur Connexion
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    M_SLA = VMM_SLA;

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void vm::setguestOSCustomization(std::string VMguestOSCustomization)
{
    if (VMguestOSCustomization == "True")
    {
        VMguestOSCustomization = "1";
    }
    else if (VMguestOSCustomization == "False")
    {
        VMguestOSCustomization = "0";
    }
    else
    {
        VMguestOSCustomization = "0";
    }

    guestOSCustomization = VMguestOSCustomization;
}

void vm::setM_HWVersion(std::string VMM_HWVersion)
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char HWVersion[300] = "";
        char qHWVersion[500] = "";
        char qIN[500] = "";

        //Requête récupération de l'ID de la métrique
        sprintf(qHWVersion, "SELECT ID_metrics FROM metrics where type_metrics = 'HWVersion' AND correspondence_metrics = '%s';", VMM_HWVersion.c_str());

        mysql_query(&connexion, qHWVersion);

        MYSQL_RES* rHWVersion = mysql_use_result(&connexion);
        MYSQL_ROW row_HWVersion;
        unsigned int num_champs = mysql_num_fields(rHWVersion);

        while (row_HWVersion = mysql_fetch_row(rHWVersion))
        {
            unsigned long* lengths = mysql_fetch_lengths(rHWVersion);

            for (int m = 0; m < 1; m++)
            {
                //Récupération de l'ID
                sprintf(HWVersion, "%.*s ", (int)lengths[m], row_HWVersion[m] ? row_HWVersion[m] : "NULL");
            }
        }

        //Si métrique inexistante
        if (mysql_num_rows(rHWVersion) == 0)
        {
            do
            {
                //Requête insertion de la nouvelle métrique
                sprintf(qIN, "INSERT INTO metrics (type_metrics, correspondence_metrics) VALUES ('HWVersion', '%s');", VMM_HWVersion.c_str());

                mysql_query(&connexion, qIN);

                //Exécution de la requête de récupératrion de l'ID de la nouvelle métrique
                while ((row_HWVersion = mysql_fetch_row(rHWVersion)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rHWVersion);

                    for (int m = 0; m < 1; m++)
                    {
                        //Récupération du nouvel ID
                        sprintf(HWVersion, "%.*s ", (int)lengths[m], row_HWVersion[m] ? row_HWVersion[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rHWVersion) != 0); //Jusqu'à ce que la métrique existe
        }
        VMM_HWVersion = HWVersion;
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    M_HWVersion = VMM_HWVersion;

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void vm::UP()
{
    char id_vm[500] = "";

    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char vm[300] = "";
        char qvm[500] = "";
        
        //Vérification de l'existence de la vm
        sprintf(qvm, "SELECT ID_vm FROM vm WHERE ID_service = '%s' AND UID = '%s' AND vmName = '%s';", ID_service.c_str(), UID.c_str(), vmName.c_str());

        mysql_query(&connexion, qvm);

        MYSQL_RES* rvm = mysql_use_result(&connexion);        
        MYSQL_ROW row_vm;
        unsigned int num_champs = mysql_num_fields(rvm);

        while (row_vm = mysql_fetch_row(rvm))
        {
            unsigned long* lengths = mysql_fetch_lengths(rvm);

            for (int m = 0; m < 1; m++)
            {
                //Récupération des données
                sprintf(vm, "%.*s ", (int)lengths[m], row_vm[m] ? row_vm[m] : "NULL");
            }
        }


        /***** SI VM INEXISTANTE *****/

        if (mysql_num_rows(rvm) == 0)
        {
            do
            {
                //Requête insertion de la nouvelle vm

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO vm (ID_service, UUID, UID, M_OS, vmName, hostName, M_SLA, guestOSCustomization, M_HWVersion, vmware_Tools, current) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '1');", ID_service.c_str(), UUID.c_str(), UID.c_str(), M_OS.c_str(), vmName.c_str(), hostName.c_str(), M_SLA.c_str(), guestOSCustomization.c_str(), M_HWVersion.c_str(), vmware_Tools.c_str());

                mysql_query(&connexion, qIN);


                /***** RECUPERATION DE L'ID DE LA VM CREEE *****/

                char qNvm[500] = "";
                char Nid_vm[500] = "";

                sprintf(qNvm, "SELECT ID_vm FROM vm WHERE ID_service = '%s' AND UUID = '%s' AND UID = '%s' AND M_OS = '%s' AND vmName = '%s' AND hostName = '%s' AND M_SLA = '%s' AND guestOSCustomization = '%s' AND M_HWVersion = '%s' AND vmware_Tools = '%s' AND current = '1';", ID_service.c_str(), UUID.c_str(), UID.c_str(), M_OS.c_str(), vmName.c_str(), hostName.c_str(), M_SLA.c_str(), guestOSCustomization.c_str(), M_HWVersion.c_str(), vmware_Tools.c_str());

                mysql_query(&connexion, qNvm);

                MYSQL_RES* rNvm = mysql_use_result(&connexion);
                MYSQL_ROW row_Nvm;
                int num_champs_Nvm = mysql_num_fields(rNvm);

                while (row_Nvm = mysql_fetch_row(rNvm))
                {
                    unsigned long* lengths_Nvm = mysql_fetch_lengths(rNvm);

                    for (int v = 0; v < 1; v++)
                    {
                        //Récupération des données
                        sprintf(Nid_vm, "%.*s ", (int)lengths_Nvm[v], row_Nvm[v] ? row_Nvm[v] : "NULL");
                    }
                }

                /***** RECUPERATION DU NOMBRE DE VM DU CLIENT ASSOCIE *****/
                
                char qSELECTQuantity[500] = "";
                char Quantity[500] = "";

                sprintf(qSELECTQuantity, "SELECT service.quantity FROM client JOIN service ON(client.ID_client = service.ID_client) JOIN vm ON(service.ID_service = vm.ID_service) JOIN metrics ON(service.M_description_service = metrics.ID_metrics) WHERE correspondence_metrics = 'vCloud' AND ID_vm = '%s';", Nid_vm);

                mysql_query(&connexion, qSELECTQuantity);

                MYSQL_RES* rQuantity = mysql_use_result(&connexion);
                MYSQL_ROW row_Quantity;
                int num_champs_vm = mysql_num_fields(rQuantity);

                while (row_Quantity = mysql_fetch_row(rQuantity))
                {
                    unsigned long* lengths = mysql_fetch_lengths(rQuantity);

                    for (int n = 0; n < 1; n++)
                    {
                        //Récupération valeur
                        sprintf(Quantity, "%.*s ", (int)lengths[n], row_Quantity[n] ? row_Quantity[n] : "NULL");
                    }
                }
                                

                /***** INCREMENTATION ET INSERTION DE LA QUANTITE *****/
                                
                char qINCRQuantity[500] = "";

                int quantite = atoi(Quantity);
                quantite++;
                string QuantityINCR = to_string(quantite);
                                
                sprintf(qINCRQuantity, "UPDATE service JOIN client ON(client.ID_client = service.ID_client) JOIN vm ON(service.ID_service = vm.ID_service) JOIN metrics ON(service.M_description_service = metrics.ID_metrics) SET service.quantity = '%s' WHERE correspondence_metrics = 'vCloud' AND ID_vm = '%s';", QuantityINCR.c_str(), Nid_vm);

                mysql_query(&connexion, qINCRQuantity);
                                                
            } while (mysql_num_rows(rvm) != 0); //Jusqu'à ce que la vm existe
        }
        else // mettre à jour les données
        {
            char qUPDATE[500] = "";
            char qID_vm[500] = "";

            // Acquisition de l'ID de la vm
            sprintf(qID_vm, "SELECT ID_vm FROM vm WHERE UID = '%s' AND vmName = '%s';",UID.c_str(), vmName.c_str());

            mysql_query(&connexion, qID_vm);

            MYSQL_RES* rID_vm = mysql_use_result(&connexion);            
            MYSQL_ROW row_vm;
            unsigned int num_champs = mysql_num_fields(rID_vm);

            while (row_vm = mysql_fetch_row(rID_vm))
            {
                unsigned long* lengths = mysql_fetch_lengths(rID_vm);

                for (int m = 0; m < 1; m++)
                {
                    //Récupération ID
                    sprintf(id_vm, "%.*s ", (int)lengths[m], row_vm[m] ? row_vm[m] : "NULL");
                }
            }

            sprintf(qUPDATE, "UPDATE vm SET UUID = '%s', UID = '%s', M_OS = '%s', vmName = '%s', hostName = '%s', M_SLA = '%s', guestOSCustomization = '%s', M_HWVersion = '%s', vmware_Tools = '%s', current = '1' WHERE ID_vm = '%s';", UUID.c_str(), UID.c_str(), M_OS.c_str(), vmName.c_str(), hostName.c_str(), M_SLA.c_str(), guestOSCustomization.c_str(), M_HWVersion.c_str(), vmware_Tools.c_str(), id_vm);

            mysql_query(&connexion, qUPDATE);
        }
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void vm::setID_vm()
{
    string id_vm = "";

    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char vm[300] = "";
        char qvm[500] = "";

        //Vérification de l'existence de la vm
        sprintf(qvm, "SELECT ID_vm FROM vm WHERE ID_service = '%s' AND UUID = '%s' AND UID = '%s' AND M_OS = '%s' AND vmName = '%s' AND hostName = '%s' AND M_SLA = '%s' AND guestOSCustomization = '%s' AND M_HWVersion = '%s' AND vmware_Tools = '%s' AND current = '1';", ID_service.c_str(), UUID.c_str(), UID.c_str(), M_OS.c_str(), vmName.c_str(), hostName.c_str(), M_SLA.c_str(), guestOSCustomization.c_str(), M_HWVersion.c_str(), vmware_Tools.c_str());

        mysql_query(&connexion, qvm);

        MYSQL_RES* rvm = mysql_use_result(&connexion);
        MYSQL_ROW row_vm;
        unsigned int num_champs = mysql_num_fields(rvm);

        while (row_vm = mysql_fetch_row(rvm))
        {
            unsigned long* lengths = mysql_fetch_lengths(rvm);

            for (int m = 0; m < 1; m++)
            {
                //Récupération des données
                sprintf(vm, "%.*s ", (int)lengths[m], row_vm[m] ? row_vm[m] : "NULL");                
            }
        }
        id_vm = vm;
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    ID_vm = id_vm;

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

string vm::getID_vm()
{
    return ID_vm;
}


// REPORT

report::report() : ID_vm(""), RptDateHour(""), PowerOn("")
{
}

report::~report()
{
}

void report::setID_vm(vm& Lvm)
{
    ID_vm = Lvm.getID_vm();
}

void report::setRptDateHour(std::string reportRptDate, std::string reportRptHour)
{
    string jour = reportRptDate.substr(0, 2);
    string mois = reportRptDate.substr(3, 2);
    string annee = reportRptDate.substr(8, 2);

    RptDateHour = annee + "/" + mois + "/" + jour + " " + reportRptHour;
}

void report::setvApp(std::string reportvApp)
{
    vApp = reportvApp;
}

void report::setPowerOn(std::string reportPowerOn)
{
    if (reportPowerOn == "PoweredOn")
    {
        reportPowerOn = "1";
    }
    else if (reportPowerOn == "PoweredOff")
    {
        reportPowerOn = "0";
    }
    else
    {
        reportPowerOn = "0";
    }

    PowerOn = reportPowerOn;
}

void report::UP()
{
    char id_report[500] = "";

    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char report[300] = "";
        char qreport[500] = "";

        //Vérification de l'existence du rapport
        sprintf(qreport, "SELECT ID_report FROM report WHERE ID_vm = '%s' AND RptDateHour = '%s' AND vApp = '%s' AND PowerOn = '%s';", ID_vm.c_str(), RptDateHour.c_str(), vApp.c_str(), PowerOn.c_str());

        mysql_query(&connexion, qreport);

        MYSQL_RES* rreport = mysql_use_result(&connexion);
        MYSQL_ROW row_report;
        unsigned int num_champs = mysql_num_fields(rreport);

        while (row_report = mysql_fetch_row(rreport))
        {
            unsigned long* lengths = mysql_fetch_lengths(rreport);

            for (int m = 0; m < 1; m++)
            {
                //Récupération des données
                sprintf(report, "%.*s ", (int)lengths[m], row_report[m] ? row_report[m] : "NULL");
            }
        }

        //Si rapport inexistant
        if (mysql_num_rows(rreport) == 0)
        {
            do
            {
                //Requête insertion du nouveau rapport

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO report (ID_vm, RptDateHour, vApp, PowerOn) VALUES ('%s', '%s', '%s', '%s');", ID_vm.c_str(), RptDateHour.c_str(), vApp.c_str(), PowerOn.c_str());

                mysql_query(&connexion, qIN);


                //Acquisition de l'ID du rapport créé

                char qID_report[500] = "";

                sprintf(qID_report, "SELECT ID_report FROM report WHERE ID_vm = '%s' AND RptDateHour = '%s' AND vApp = '%s' AND PowerOn = '%s';", ID_vm.c_str(), RptDateHour.c_str(), vApp.c_str(), PowerOn.c_str());

                mysql_query(&connexion, qID_report);

                MYSQL_RES* rID_report = mysql_use_result(&connexion);
                MYSQL_ROW row_report;
                unsigned int num_champs = mysql_num_fields(rID_report);

                while (row_report = mysql_fetch_row(rID_report))
                {
                    unsigned long* lengths = mysql_fetch_lengths(rID_report);

                    for (int m = 0; m < 1; m++)
                    {
                        //Récupération valeur
                        sprintf(id_report, "%.*s ", (int)lengths[m], row_report[m] ? row_report[m] : "NULL");
                    }                    
                }
                ID_report = id_report;
            } while (mysql_num_rows(rreport) != 0); //Jusqu'à ce que le rapport existe
        }
        else // recupérer l'ID du rapport existant
        {
            char qUPDATE[500] = "";
            char qID_report[500] = "";

            // Acquisition de l'ID du rapport
            sprintf(qreport, "SELECT ID_report FROM report WHERE ID_vm = '%s' AND RptDateHour = '%s' AND vApp = '%s' AND PowerOn = '%s';", ID_vm.c_str(), RptDateHour.c_str(), vApp.c_str(), PowerOn.c_str());

            mysql_query(&connexion, qreport);

            MYSQL_RES* rID_report = mysql_use_result(&connexion);
            MYSQL_ROW row_report;
            unsigned int num_champs = mysql_num_fields(rID_report);

            while (row_report = mysql_fetch_row(rID_report))
            {
                unsigned long* lengths = mysql_fetch_lengths(rID_report);

                for (int m = 0; m < 1; m++)
                {
                    //Récupération valeur
                    sprintf(id_report, "%.*s ", (int)lengths[m], row_report[m] ? row_report[m] : "NULL");
                }                
            }
            ID_report = id_report;
        }
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

string report::getID_report()
{
    return ID_report;
}


// DISK

disk::disk() : ID_report(""), capacity_disk(""), M_type_disk(""), M_unit("")
{
}

disk::~disk()
{
}

void disk::setID_report(report& Lreport)
{
    ID_report = Lreport.getID_report();
}

void disk::setcapacity_disk(std::string diskcapacity_disk)
{
    float capacity = stof(diskcapacity_disk);
    string disk_unit = "";

    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
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

        char unit[300] = "";
        char qunit[500] = "";

        //Requête récupération de l'ID de la métrique
        sprintf(qunit, "SELECT ID_metrics FROM metrics where type_metrics = 'unit' AND correspondence_metrics = '%s';", disk_unit.c_str());

        mysql_query(&connexion, qunit);

        MYSQL_RES* runit = mysql_use_result(&connexion);
        MYSQL_ROW row_unit;
        unsigned int num_champs = mysql_num_fields(runit);

        while (row_unit = mysql_fetch_row(runit))
        {
            unsigned long* lengths = mysql_fetch_lengths(runit);

            for (int m = 0; m < 1; m++)
            {
                //Récupération de l'ID
                sprintf(unit, "%.*s ", (int)lengths[m], row_unit[m] ? row_unit[m] : "NULL");
            }
        }
        M_unit = unit;
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    int capacitINT = capacity;

    capacity_disk = to_string(capacitINT);

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void disk::setM_type_disk(std::string diskM_type_disk)
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char type_disk[300] = "";
        char qtype_disk[500] = "";
        char qIN[500] = "";

        //Requête récupération de l'ID de la métrique
        sprintf(qtype_disk, "SELECT ID_metrics FROM metrics where type_metrics = 'type_disk' AND correspondence_metrics = '%s';", diskM_type_disk.c_str());

        mysql_query(&connexion, qtype_disk);

        MYSQL_RES* rtype_disk = mysql_use_result(&connexion);
        MYSQL_ROW row_type_disk;
        unsigned int num_champs = mysql_num_fields(rtype_disk);

        while (row_type_disk = mysql_fetch_row(rtype_disk))
        {
            unsigned long* lengths = mysql_fetch_lengths(rtype_disk);

            for (int m = 0; m < 1; m++)
            {
                //Récupération de l'ID
                sprintf(type_disk, "%.*s ", (int)lengths[m], row_type_disk[m] ? row_type_disk[m] : "NULL");
            }
        }

        //Si métrique inexistante
        if (mysql_num_rows(rtype_disk) == 0)
        {
            do
            {
                //Requête insertion de la nouvelle métrique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('type_disk', '%s');", diskM_type_disk.c_str());

                mysql_query(&connexion, qIN);

                //Récupératrion de l'ID de la nouvelle métrique
                while ((row_type_disk = mysql_fetch_row(rtype_disk)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rtype_disk);

                    for (int m = 0; m < 1; m++)
                    {
                        //Récupération du nouvel ID
                        sprintf(type_disk, "%.*s ", (int)lengths[m], row_type_disk[m] ? row_type_disk[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rtype_disk) != 0); //Jusqu'à ce que la métrique existe
        }
        diskM_type_disk = type_disk;
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    M_type_disk = diskM_type_disk;

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void disk::UP()
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char disk[300] = "";
        char qdisk[500] = "";

        //Vérification de l'inexistence du disk
        sprintf(qdisk, "SELECT ID_disk FROM disk WHERE ID_report = '%s' AND capacity_disk = '%s' AND M_type_disk = '%s' AND M_unit = '%s'; ", ID_report.c_str(), capacity_disk.c_str(), M_type_disk.c_str(), M_unit.c_str());

        mysql_query(&connexion, qdisk);

        MYSQL_RES* rdisk = mysql_use_result(&connexion);
        MYSQL_ROW row_disk;
        unsigned int num_champs = mysql_num_fields(rdisk);

        while (row_disk = mysql_fetch_row(rdisk))
        {
            unsigned long* lengths = mysql_fetch_lengths(rdisk);

            for (int m = 0; m < 1; m++)
            {
                //Récupération des données
                sprintf(disk, "%.*s ", (int)lengths[m], row_disk[m] ? row_disk[m] : "NULL");
            }
        }

        //Si disk inexistant
        if (mysql_num_rows(rdisk) == 0)
        {
            do
            {
                //Requête insertion du nouveau disk

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO disk (ID_report, capacity_disk, M_type_disk, M_unit) VALUES ('%s', '%s', '%s', '%s');", ID_report.c_str(), capacity_disk.c_str(), M_type_disk.c_str(), M_unit.c_str());

                mysql_query(&connexion, qIN);

            } while (mysql_num_rows(rdisk) != 0); //Jusqu'à ce que le disk existe
        }
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}


// RAM

ram::ram() : ID_report(""), capacity_ram(""), M_unit("")
{
}

ram::~ram()
{
}

void ram::setID_report(report& Lreport)
{
    ID_report = Lreport.getID_report();
}

void ram::setcapacity_ram(std::string ramcapacity_ram)
{
    float capacity = stof(ramcapacity_ram);
    string ram_unit = "";

    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
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

        char unit[300] = "";
        char qunit[500] = "";

        //Requête récupération de l'ID de la métrique
        sprintf(qunit, "SELECT ID_metrics FROM metrics where type_metrics = 'unit' AND correspondence_metrics = '%s';", ram_unit.c_str());

        mysql_query(&connexion, qunit);

        MYSQL_RES* runit = mysql_use_result(&connexion);
        MYSQL_ROW row_unit;
        unsigned int num_champs = mysql_num_fields(runit);

        while (row_unit = mysql_fetch_row(runit))
        {
            unsigned long* lengths = mysql_fetch_lengths(runit);

            for (int m = 0; m < 1; m++)
            {
                //Récupération de l'ID
                sprintf(unit, "%.*s ", (int)lengths[m], row_unit[m] ? row_unit[m] : "NULL");
            }
        }
        M_unit = unit;
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    int capacitINT = capacity;

    capacity_ram = to_string(capacitINT);

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void ram::UP()
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char ram[300] = "";
        char qram[500] = "";

        //Vérification de l'existence de la ram
        sprintf(qram, "SELECT ID_ram FROM ram WHERE ID_report = '%s' AND capacity_ram = '%s' AND M_unit = '%s';", ID_report.c_str(), capacity_ram.c_str(), M_unit.c_str());

        mysql_query(&connexion, qram);

        MYSQL_RES* rram = mysql_use_result(&connexion);
        MYSQL_ROW row_ram;
        unsigned int num_champs = mysql_num_fields(rram);

        while (row_ram = mysql_fetch_row(rram))
        {
            unsigned long* lengths = mysql_fetch_lengths(rram);

            for (int m = 0; m < 1; m++)
            {
                //Récupération des données
                sprintf(ram, "%.*s ", (int)lengths[m], row_ram[m] ? row_ram[m] : "NULL");
            }
        }

        //Si ram inexistant
        if (mysql_num_rows(rram) == 0)
        {
            do
            {
                //Requête insertion de la nouvelle ram

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO ram (ID_report, capacity_ram, M_unit) VALUES ('%s', '%s', '%s');", ID_report.c_str(), capacity_ram.c_str(), M_unit.c_str());

                mysql_query(&connexion, qIN);

            } while (mysql_num_rows(rram) != 0); //Jusqu'à ce que la ram existe
        }
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}


// CPU

cpu::cpu() : ID_report(""), nbrCore(""), M_model_cpu("")
{
}

cpu::~cpu()
{
}

void cpu::setID_report(report& Lreport)
{
    ID_report = Lreport.getID_report();
}

void cpu::setnbrCore(std::string cpunbrCore)
{
    int CorINT = stoi(cpunbrCore); //conversion vers INT pour éviter problèmes d'insertion de nombre à virgule

    string CorSTRING = to_string(CorINT); //conversion pour adaptation à la classe

    nbrCore = CorSTRING;
}

void cpu::setM_model_cpu(std::string cpuM_model_cpu)
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char model_cpu[300] = "";
        char qmodel_cpu[500] = "";
        char qIN[500] = "";

        //Requête récupération de l'ID de la métrique
        sprintf(qmodel_cpu, "SELECT ID_metrics FROM metrics where type_metrics = 'model_cpu' AND correspondence_metrics = '%s';", cpuM_model_cpu.c_str());

        mysql_query(&connexion, qmodel_cpu);

        MYSQL_RES* rmodel_cpu = mysql_use_result(&connexion);
        MYSQL_ROW row_model_cpu;
        unsigned int num_champs = mysql_num_fields(rmodel_cpu);

        while (row_model_cpu = mysql_fetch_row(rmodel_cpu))
        {
            unsigned long* lengths = mysql_fetch_lengths(rmodel_cpu);

            for (int m = 0; m < 1; m++)
            {
                //Récupération de l'ID
                sprintf(model_cpu, "%.*s ", (int)lengths[m], row_model_cpu[m] ? row_model_cpu[m] : "NULL");
            }
        }
                
        if (mysql_num_rows(rmodel_cpu) == 0) //Si métrique inexistante
        {
            do
            {
                //Requête insertion de la nouvelle métrique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('model_cpu', '%s');", cpuM_model_cpu.c_str());

                mysql_query(&connexion, qIN);

                //Récupératrion de l'ID de la nouvelle métrique
                while ((row_model_cpu = mysql_fetch_row(rmodel_cpu)))
                {
                    unsigned long* lengths;
                    
                    lengths = mysql_fetch_lengths(rmodel_cpu);

                    for (int m = 0; m < 1; m++)
                    {
                        //Récupération du nouvel ID
                        sprintf(model_cpu, "%.*s ", (int)lengths[m], row_model_cpu[m] ? row_model_cpu[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rmodel_cpu) != 0); //Jusqu'à ce que la métrique existe
        }
        cpuM_model_cpu = model_cpu;
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    M_model_cpu = cpuM_model_cpu;

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void cpu::UP()
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char cpu[300] = "";
        char qcpu[500] = "";

        //Vérification de l'existence du cpu
        sprintf(qcpu, "SELECT ID_cpu FROM cpu WHERE ID_report = '%s' AND M_model_cpu = '%s' AND nbrCore = '%s';", ID_report.c_str(), M_model_cpu.c_str(), nbrCore.c_str());

        mysql_query(&connexion, qcpu);

        MYSQL_RES* rcpu = mysql_use_result(&connexion);
        MYSQL_ROW row_cpu;
        unsigned int num_champs = mysql_num_fields(rcpu);

        while (row_cpu = mysql_fetch_row(rcpu))
        {
            unsigned long* lengths = mysql_fetch_lengths(rcpu);

            for (int m = 0; m < 1; m++)
            {
                //Récupération des données
                sprintf(cpu, "%.*s ", (int)lengths[m], row_cpu[m] ? row_cpu[m] : "NULL");
            }
        }

        //Si cpu inexistant
        if (mysql_num_rows(rcpu) == 0)
        {
            do
            {
                //Requête insertion du nouveau cpu

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO cpu (ID_report, M_model_cpu, nbrCore) VALUES ('%s', '%s', '%s');", ID_report.c_str(), M_model_cpu.c_str(), nbrCore.c_str());

                mysql_query(&connexion, qIN);

            } while (mysql_num_rows(rcpu) != 0); //Jusqu'à ce que le cpu existe
        }
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}


// BACKUP

backup::backup() : ID_report(""), M_type_backup("")
{
}

backup::~backup()
{
}

void backup::setID_report(report& Lreport)
{
    ID_report = Lreport.getID_report();
}

void backup::setM_type_backup(std::string backupM_type_backup)
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char type_backup[300] = "";
        char qtype_backup[500] = "";
        char qIN[500] = "";

        //Requête récupération de l'ID de la métrique
        sprintf(qtype_backup, "SELECT ID_metrics FROM metrics where type_metrics = 'type_backup' AND correspondence_metrics = '%s';", backupM_type_backup.c_str());

        mysql_query(&connexion, qtype_backup);

        MYSQL_RES* rtype_backup = mysql_use_result(&connexion);
        MYSQL_ROW row_type_backup;
        unsigned int num_champs = mysql_num_fields(rtype_backup);

        while (row_type_backup = mysql_fetch_row(rtype_backup))
        {
            unsigned long* lengths = mysql_fetch_lengths(rtype_backup);

            for (int m = 0; m < 1; m++)
            {
                //Récupération de l'ID
                sprintf(type_backup, "%.*s ", (int)lengths[m], row_type_backup[m] ? row_type_backup[m] : "NULL");
            }
        }

        //Si métrique inexistante
        if (mysql_num_rows(rtype_backup) == 0)
        {
            do
            {
                //Requête insertion de la nouvelle métrique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('type_backup', '%s');", backupM_type_backup.c_str());

                mysql_query(&connexion, qIN);

                //Récupératrion de l'ID de la nouvelle métrique
                while ((row_type_backup = mysql_fetch_row(rtype_backup)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rtype_backup);

                    for (int m = 0; m < 1; m++)
                    {
                        //Récupération du nouvel ID
                        sprintf(type_backup, "%.*s ", (int)lengths[m], row_type_backup[m] ? row_type_backup[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rtype_backup) != 0); //Jusqu'à ce que la métrique existe
        }
        backupM_type_backup = type_backup;
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    M_type_backup = backupM_type_backup;

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}

void backup::UP()
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char backup[300] = "";
        char qbackup[500] = "";

        //Vérification de l'existence de la backup
        sprintf(qbackup, "SELECT ID_backup FROM backup WHERE ID_report = '%s' AND M_type_backup = '%s';", ID_report.c_str(), M_type_backup.c_str());

        mysql_query(&connexion, qbackup);

        MYSQL_RES* rbackup = mysql_use_result(&connexion);
        MYSQL_ROW row_backup;
        unsigned int num_champs = mysql_num_fields(rbackup);

        while (row_backup = mysql_fetch_row(rbackup))
        {
            unsigned long* lengths = mysql_fetch_lengths(rbackup);

            for (int m = 0; m < 1; m++)
            {
                //Récupération des données
                sprintf(backup, "%.*s ", (int)lengths[m], row_backup[m] ? row_backup[m] : "NULL");
            }
        }

        //Si backup inexistant
        if (mysql_num_rows(rbackup) == 0)
        {
            do
            {
                //Requête insertion de la nouvelle backup

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO backup (ID_report, M_type_backup) VALUES ('%s', '%s');", ID_report.c_str(), M_type_backup.c_str());

                mysql_query(&connexion, qIN);

            } while (mysql_num_rows(rbackup) != 0); //Jusqu'à ce que la backup existe
        }
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}


// NETWORK

network::network() : ID_report(""), MAC_address(""), IP_address("")
{
}

network::~network()
{
}

void network::setID_report(report& Lreport)
{
    ID_report = Lreport.getID_report();
}

void network::setIP_address(std::string networkIP_address)
{
    IP_address = networkIP_address;
}

void network::setMAC_address(std::string networkMAC_address)
{
    MAC_address = networkMAC_address;
}

void network::UP()
{
    MYSQL connexion;
    mysql_init(&connexion);

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        /****** MAC EXTRACTION *****/

        vector<string> tabMAC;
        int macChainSize = (size(MAC_address) / MAC_LENGH);

        for (int i = 0; i < macChainSize; i++)
        {
            // Transfer mac address from Mac_address string to tabMac
            tabMAC.push_back(MAC_address.substr(size(MAC_address) - MAC_LENGH, size(MAC_address)));

            for (int i = 0; i < MAC_LENGH; i++)
            {
                // deleting transfered mac address
                MAC_address.pop_back();
            }
        }

        if (tabMAC.size() == 0) // si la vm ne possede pas d'adresse MAC, pour enregistrer champ vide
        {
            tabMAC.push_back("");
        }


        /***** IP EXTRACTION *****/

        vector<string> tabIP;
        stringstream ssIP(IP_address);
        string IP;

        while (getline(ssIP, IP, '/'))
        {
            tabIP.push_back(IP);
        }

        if (tabIP.size() == 0) // si la vm ne possede pas d'adresse IP, pour enregistrer champ vide
        {
            tabIP.push_back("");
        }


        /***** MISE A NIVEAU *****/

        while (tabMAC.size() != tabIP.size()) // mise à niveau des deux tableaux pour eviter d'insérer des valeurs aleatoires
        {
            if (tabMAC.size() < tabIP.size())
            {
                tabMAC.push_back("");
            }
            else if (tabMAC.size() > tabIP.size())
            {
                tabIP.push_back("");
            }
        }

        /***** INSERTION *****/

        int tailletab = tabMAC.size();

        for (int j = 0; j < tailletab; j++)
        {
            char network[300] = "";
            char qnetwork[500] = "";

            //Vérification de l'existence du network
            sprintf(qnetwork, "SELECT ID_network FROM network WHERE ID_report = '%s' AND MAC_address = '%s' AND IP_address = '%s';", ID_report.c_str(), tabMAC[j].c_str(), tabIP[j].c_str());

            mysql_query(&connexion, qnetwork);

            MYSQL_RES* rnetwork = mysql_use_result(&connexion);
            MYSQL_ROW row_network;
            unsigned int num_champs = mysql_num_fields(rnetwork);

            while (row_network = mysql_fetch_row(rnetwork))
            {
                unsigned long* lengths = mysql_fetch_lengths(rnetwork);

                for (int m = 0; m < 1; m++)
                {
                    //Récupération des données
                    sprintf(network, "%.*s ", (int)lengths[m], row_network[m] ? row_network[m] : "NULL");
                }
            }

            //Si network inexistant
            if (mysql_num_rows(rnetwork) == 0)
            {
                do
                {
                    //Requête insertion du nouveau network

                    char qIN[500] = "";

                    sprintf(qIN, "INSERT INTO network (ID_report, MAC_address, IP_address) VALUES ('%s', '%s', '%s');", ID_report.c_str(), tabMAC[j].c_str(), tabIP[j].c_str());
                    mysql_query(&connexion, qIN);

                } while (mysql_num_rows(rnetwork) != 0); //Tant que que le network n'existe pas
            }
        }
    }
    else
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&connexion));
    }

    mysql_close(&connexion); //fermeture connection
    nbrconn++;
}