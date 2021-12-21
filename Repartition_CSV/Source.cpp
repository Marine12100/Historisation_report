#include "Header.h"

using namespace std;

int nbrconn;

void exploration(MYSQL *connexion, const char* chemin)
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

                        exploration(connexion, chaine);
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
                                vector <string> tableau = creation(line);

                                if (tableau.size() >= 33 && tableau[0] != "UUID" && tableau[1] != "UID") // verifier que ce n'est pas une ligne d'en-tete
                                {
                                    vm Lvm;
                                    report Lreport;
                                    disk Ldisk;
                                    ram Lram;
                                    cpu Lcpu;
                                    backup Lbackup;
                                    network Lnetwork;

                                    repartition(connexion, tableau, Lvm, Lreport, Ldisk, Lram, Lcpu, Lbackup, Lnetwork); // repartir les cellules
                                    insertion(connexion, Lvm, Lreport, Ldisk, Lram, Lcpu, Lbackup, Lnetwork); // inserer les objets dans la bdd
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

            //recherche de deux adresses MAC à la suite dans le tableau
            bool MAC1 = regex_search(tableau[i], rgxMAC);
            bool MAC2 = regex_search(tableau[i + 1], rgxMAC);

            //expression régulière d'une adresse IPv4
            regex rgxIPv4("([0-9]{1,3}[.]){3}[0-9]{1,3}");

            //recherche de deux adresses IPv4 à la suite dans le tableau
            bool IPv4_1 = regex_search(tableau[i], rgxIPv4);
            bool IPv4_2 = regex_search(tableau[i + 1], rgxIPv4);

            //expression régulière d'une adresse IPv6
            regex rgxIPv6("(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))");

            //recherche de deux adresses IPv6 à la suite dans le tableau
            bool IPv6_1 = regex_search(tableau[i], rgxIPv6);
            bool IPv6_2 = regex_search(tableau[i], rgxIPv6);


            if (MAC1 && MAC2) //si deux MAC à la suite trouvées
            {
                tableau[i] = tableau[i] + tableau[i + 1]; //concaténation des MAC
                tableau.erase(tableau.begin() + (i + 1));
            }
            else if (IPv4_1 && IPv4_2) //si deux IPv4 à la suite trouvées
            {
                tableau[i] = tableau[i] + "/" + tableau[i + 1]; //concaténation des IPv4
                tableau.erase(tableau.begin() + (i + 1));
            }
            else if (IPv6_1 && IPv6_2) //si deux IPv6 à la suite trouvées
            {
                tableau[i] = tableau[i] + "/" + tableau[i + 1]; //concaténation des IPv6
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
        tableau[i].erase(remove(tableau[i].begin(), tableau[i].end(), '"'), tableau[i].end()); //supprimer les guillemets doubles de la cellule
        tableau[i].erase(remove(tableau[i].begin(), tableau[i].end(), '\''), tableau[i].end()); //supprimer les guillemets simples de la cellule
    }

    return tableau;
}

void repartition(MYSQL*connexion, vector<string>& tableau, vm &Lvm, report& Lreport, disk& Ldisk, ram& Lram, cpu& Lcpu, backup& Lbackup, network& Lnetwork)
{
    Lvm.setUUID(tableau[0]);
    Lvm.setUID(tableau[1]);
    Lvm.setvmName(tableau[2]);
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

void insertion(MYSQL *connexion, vm& Lvm, report& Lreport, disk &Ldisk, ram &Lram, cpu &Lcpu, backup &Lbackup, network &Lnetwork)
{
    Lvm.UP(connexion);

    Lreport.setID_vm(Lvm);
    Lreport.UP(connexion, Ldisk, Lram, Lcpu, Lbackup, Lnetwork);
}

// VM

vm::vm() : ID_service(""), UUID(""), UID(""), M_OS(""), vmName(""), hostName(""), M_SLA(""), guestOSCustomization(""), M_HWVersion(""), vmware_Tools("")
{
}

vm::~vm()
{
}

void vm::setID_service(MYSQL *connexion, string VMOrg, string VMOrgFullName)
{
    /***** SELECT ID_client *****/

    char qSELECT_ID_client[500] = "";
    char rID_client[500] = "";

    sprintf(qSELECT_ID_client, "SELECT ID_client FROM client WHERE code_client = '%s';", VMOrg.c_str());
    mysql_query(connexion, qSELECT_ID_client);

    MYSQL_RES* RES_ID_client = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_client = NULL;

    while ((ROW_ID_client = mysql_fetch_row(RES_ID_client)))
    {
        unsigned long* L_ID_client = mysql_fetch_lengths(RES_ID_client);

        for (int i = 0; i < 1; i++)
            sprintf(rID_client, "%.*s", (int)L_ID_client[i], ROW_ID_client[i]);
    }

    if (mysql_num_rows(RES_ID_client) == 0) // SI CLIENT INEXISTANT
    {
        /***** INSERT CLIENT FANTOME *****/

        char qINSERT_client[500] = "";

        sprintf(qINSERT_client, "INSERT INTO client (code_client, social_reason) VALUES ('%s', '%s');", VMOrg.c_str(), VMOrgFullName.c_str());
        mysql_query(connexion, qINSERT_client);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_client << endl;
            exit(0);
        }

        /***** SELECT NEW_ID_client *****/

        mysql_query(connexion, qSELECT_ID_client);

        MYSQL_RES* RES_NEW_ID_client = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_ID_client = NULL;

        while ((ROW_NEW_ID_client = mysql_fetch_row(RES_NEW_ID_client)))
        {
            unsigned long* L_NEW_ID_client = mysql_fetch_lengths(RES_NEW_ID_client);

            for (int i = 0; i < 1; i++)
                sprintf(rID_client, "%.*s", (int)L_NEW_ID_client[i], ROW_NEW_ID_client[i]);
        }

        if (mysql_num_rows(RES_NEW_ID_client) == 0) // SI REQUETE VIDE
        {
            cout << "Identifiant du client correspondant inexistant" << endl;
            cout << qSELECT_ID_client << endl;
            exit(0);
        }

        /***** SELECT METRIQUE vCloud *****/

        char qSELECT_ID_vCloud[500] = "";
        char rID_vCloud[500] = "";

        sprintf(qSELECT_ID_vCloud, "SELECT ID_metrics FROM metrics WHERE type_metrics = 'service' AND correspondence_metrics = 'vCloud';");
        mysql_query(connexion, qSELECT_ID_vCloud);

        MYSQL_RES* RES_ID_vCloud = mysql_use_result(connexion);
        MYSQL_ROW ROW_ID_vCloud = NULL;

        while ((ROW_ID_vCloud = mysql_fetch_row(RES_ID_vCloud)))
        {
            unsigned long* L_ID_vCloud = mysql_fetch_lengths(RES_ID_vCloud);

            for (int i = 0; i < 1; i++)
                sprintf(rID_vCloud, "%.*s", (int)L_ID_vCloud[i], ROW_ID_vCloud[i] ? ROW_ID_vCloud[i] : "NULL");
        }

        if (mysql_num_rows(RES_ID_vCloud) == 0) // SI REQUETE VIDE
        {
            /***** INSERT METRICS *****/

            char qINSERT_metrics[500] = "";

            sprintf(qINSERT_metrics, "INSERT INTO metrics (type_metrics, correspondence_metrics) VALUES('service', 'vCloud');");
            mysql_query(connexion, qINSERT_metrics);

            if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
            {
                fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
                cout << qINSERT_metrics << endl;
                exit(0);
            }

            /***** SELECT NEW_ID_metrics *****/

            mysql_query(connexion, qSELECT_ID_vCloud);

            MYSQL_RES* RES_NEW_ID_vCloud = mysql_use_result(connexion);
            MYSQL_ROW ROW_NEW_ID_vCloud = NULL;

            while (ROW_NEW_ID_vCloud = mysql_fetch_row(RES_NEW_ID_vCloud))
            {
                unsigned long* L_NEW_ID_vCloud = mysql_fetch_lengths(RES_NEW_ID_vCloud);

                for (int i = 0; i < 1; i++)
                    sprintf(rID_vCloud, "%.*s", (int)L_NEW_ID_vCloud[i], ROW_NEW_ID_vCloud[i] ? ROW_NEW_ID_vCloud[i] : "NULL");
            }

            if (mysql_num_rows(RES_NEW_ID_vCloud) == 0) // SI REQUETE VIDE
            {
                cout << "Metrique vCloud inexistante" << endl;
                cout << qSELECT_ID_vCloud << endl;
                exit(0);
            }

            mysql_free_result(RES_NEW_ID_vCloud);
        }

        /***** INSERT SERVICE *****/

        char qINSERT_service[500] = "";

        sprintf(qINSERT_service, "INSERT INTO service (ID_client, M_description_service, quantity) VALUES ('%s', '%s', '0');", rID_client, rID_vCloud);
        mysql_query(connexion, qINSERT_service);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_service << endl;
            exit(0);
        }
    }

    mysql_free_result(RES_ID_client);

    /***** SELECT ID_SERVICE *****/

    char qSELECT_ID_service[500] = "";
    char rID_service[500] = "";

    sprintf(qSELECT_ID_service, "SELECT service.ID_service FROM client JOIN service ON (client.ID_client = service.ID_client) JOIN metrics ON (service.M_description_service = metrics.ID_metrics) WHERE correspondence_metrics = 'vCloud' AND client.ID_client = '%s';", rID_client);
    mysql_query(connexion, qSELECT_ID_service);

    MYSQL_RES* RES_ID_service = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_service = NULL;

    while ((ROW_ID_service = mysql_fetch_row(RES_ID_service)))
    {
        unsigned long* L_ID_service = mysql_fetch_lengths(RES_ID_service);

        for (int i = 0; i < 1; i++)
            sprintf(rID_service, "%.*s", (int)L_ID_service[i], ROW_ID_service[i] ? ROW_ID_service[i] : "NULL");
    }

    if (mysql_num_rows(RES_ID_service) == 0) // SI REQUETE VIDE
    {
        cout << "Service du client correspondant inexistant" << endl;
        cout << qSELECT_ID_service << endl;
        exit(0);
    }

    ID_service = rID_service;

    mysql_free_result(RES_ID_service);
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

void vm::setM_OS(MYSQL* connexion, std::string VMM_OS)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_OS[500] = "";
    char rM_OS[500] = "";

    sprintf(qSELECT_M_OS, "SELECT ID_metrics FROM metrics where type_metrics = 'OS' AND correspondence_metrics = '%s';", VMM_OS.c_str());
    mysql_query(connexion, qSELECT_M_OS);

    MYSQL_RES* RES_M_OS = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_OS = NULL;

    while (ROW_M_OS = mysql_fetch_row(RES_M_OS))
    {
        unsigned long* L_ID_metrics = mysql_fetch_lengths(RES_M_OS);

        for (int i = 0; i < 1; i++)
            sprintf(rM_OS, "%.*s", (int)L_ID_metrics[i], ROW_M_OS[i] ? ROW_M_OS[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_OS) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_metrics[500] = "";

        sprintf(qINSERT_metrics, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('OS', '%s');", VMM_OS.c_str());
        mysql_query(connexion, qINSERT_metrics);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_metrics << endl;
            exit(0);
        }

        /***** SELECT NEW_ID_metrics *****/

        mysql_query(connexion, qSELECT_M_OS);

        MYSQL_RES* RES_NEW_M_OS = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_M_OS = NULL;

        while (ROW_NEW_M_OS = mysql_fetch_row(RES_NEW_M_OS))
        {
            unsigned long* L_NEW_ID_metrics = mysql_fetch_lengths(RES_NEW_M_OS);

            for (int i = 0; i < 1; i++)
                sprintf(rM_OS, "%.*s", (int)L_NEW_ID_metrics[i], ROW_NEW_M_OS[i] ? ROW_NEW_M_OS[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_OS) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique de l'OS correspondante inexistante" << endl;
            cout << qSELECT_M_OS << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_OS);
    }

    M_OS = rM_OS;

    mysql_free_result(RES_M_OS);
}

void vm::setM_SLA(MYSQL* connexion, std::string VMM_SLA)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_SLA[500] = "";
    char rM_SLA[500] = "";

    sprintf(qSELECT_M_SLA, "SELECT ID_metrics FROM metrics where type_metrics = 'SLA' AND correspondence_metrics = '%s';", VMM_SLA.c_str());
    mysql_query(connexion, qSELECT_M_SLA);

    MYSQL_RES* RES_M_SLA = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_SLA = NULL;    

    while (ROW_M_SLA = mysql_fetch_row(RES_M_SLA))
    {
        unsigned long* L_M_SLA = mysql_fetch_lengths(RES_M_SLA);

        for (int i = 0; i < 1; i++)
            sprintf(rM_SLA, "%.*s", (int)L_M_SLA[i], ROW_M_SLA[i] ? ROW_M_SLA[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_SLA) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_M_SLA[500] = "";

        sprintf(qINSERT_M_SLA, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('SLA', '%s');", VMM_SLA.c_str());

        mysql_query(connexion, qINSERT_M_SLA);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_M_SLA << endl;
            exit(0);
        }

        /***** SELECT NEW_ID_metrics *****/

        mysql_query(connexion, qSELECT_M_SLA);

        MYSQL_RES* RES_NEW_M_SLA = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_M_SLA = NULL;

        while (ROW_NEW_M_SLA = mysql_fetch_row(RES_NEW_M_SLA))
        {
            unsigned long* L_NEW_M_SLA = mysql_fetch_lengths(RES_NEW_M_SLA);

            for (int i = 0; i < 1; i++)
                sprintf(rM_SLA, "%.*s", (int)L_NEW_M_SLA[i], ROW_NEW_M_SLA[i] ? ROW_NEW_M_SLA[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_SLA) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique du SLA correspondant inexistant" << endl;
            cout << qSELECT_M_SLA << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_SLA);
    }

    M_SLA = rM_SLA;

    mysql_free_result(RES_M_SLA);
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

void vm::setM_HWVersion(MYSQL* connexion, std::string VMM_HWVersion)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_HWVersion[500] = "";
    char rM_HWVersion[500] = "";

    sprintf(qSELECT_M_HWVersion, "SELECT ID_metrics FROM metrics where type_metrics = 'HWVersion' AND correspondence_metrics = '%s';", VMM_HWVersion.c_str());
    mysql_query(connexion, qSELECT_M_HWVersion);

    MYSQL_RES* RES_M_HWVersion = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_HWVersion = NULL;

    while (ROW_M_HWVersion = mysql_fetch_row(RES_M_HWVersion))
    {
        unsigned long* L_M_HWVersion = mysql_fetch_lengths(RES_M_HWVersion);

        for (int i = 0; i < 1; i++)
            sprintf(rM_HWVersion, "%.*s", (int)L_M_HWVersion[i], ROW_M_HWVersion[i] ? ROW_M_HWVersion[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_HWVersion) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_M_HWVersion[500] = "";

        sprintf(qINSERT_M_HWVersion, "INSERT INTO metrics (type_metrics, correspondence_metrics) VALUES ('HWVersion', '%s');", VMM_HWVersion.c_str());
        mysql_query(connexion, qINSERT_M_HWVersion);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_M_HWVersion << endl;
            exit(0);
        }

        /***** SELECT NEW_ID_metrics *****/

        mysql_query(connexion, qSELECT_M_HWVersion);

        MYSQL_RES* RES_NEW_M_HWVersion = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_M_HWVersion = NULL;

        while (ROW_NEW_M_HWVersion = mysql_fetch_row(RES_NEW_M_HWVersion))
        {
            unsigned long* L_NEW_M_HWVersion = mysql_fetch_lengths(RES_NEW_M_HWVersion);

            for (int i = 0; i < 1; i++)
                sprintf(rM_HWVersion, "%.*s", (int)L_NEW_M_HWVersion[i], ROW_NEW_M_HWVersion[i] ? ROW_NEW_M_HWVersion[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_HWVersion) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique du HWVersion correspondant inexistant" << endl;
            cout << qSELECT_M_HWVersion << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_HWVersion);
    }

    M_HWVersion = rM_HWVersion;

    mysql_free_result(RES_M_HWVersion);
}

void vm::UP(MYSQL* connexion)
{
    /***** SELECT ID_vm *****/

    char qSELECT_ID_vm[500] = "";
    char rID_vm[500] = "";

    sprintf(qSELECT_ID_vm, "SELECT ID_vm FROM vm WHERE ID_service = '%s' AND vmName = '%s';", ID_service.c_str(), vmName.c_str());
    mysql_query(connexion, qSELECT_ID_vm);

    MYSQL_RES* RES_ID_vm = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_vm = NULL;

    while (ROW_ID_vm = mysql_fetch_row(RES_ID_vm))
    {
        unsigned long* L_ID_vm = mysql_fetch_lengths(RES_ID_vm);

        for (int i = 0; i < 1; i++)
            sprintf(rID_vm, "%.*s", (int)L_ID_vm[i], ROW_ID_vm[i] ? ROW_ID_vm[i] : "NULL");
    }

    if (mysql_num_rows(RES_ID_vm) == 0) // SI VM INEXISTANTE
    {
        /***** INSERT VM *****/

        char qINSERT_VM[500] = "";

        sprintf(qINSERT_VM, "INSERT INTO vm (ID_service, UUID, UID, M_OS, vmName, hostName, M_SLA, guestOSCustomization, M_HWVersion, vmware_Tools, current) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '1');", ID_service.c_str(), UUID.c_str(), UID.c_str(), M_OS.c_str(), vmName.c_str(), hostName.c_str(), M_SLA.c_str(), guestOSCustomization.c_str(), M_HWVersion.c_str(), vmware_Tools.c_str());
        mysql_query(connexion, qINSERT_VM);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_VM << endl;
            exit(0);
        }

        /***** SELECT NEW_ID_vm *****/

        mysql_query(connexion, qSELECT_ID_vm);

        MYSQL_RES* RES_NEW_ID_vm = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_ID_vm = NULL;

        while (ROW_NEW_ID_vm = mysql_fetch_row(RES_NEW_ID_vm))
        {
            unsigned long* L_NEW_ID_vm = mysql_fetch_lengths(RES_NEW_ID_vm);

            for (int i = 0; i < 1; i++)
                sprintf(rID_vm, "%.*s", (int)L_NEW_ID_vm[i], ROW_NEW_ID_vm[i] ? ROW_NEW_ID_vm[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_ID_vm) == 0) // SI REQUETE VIDE
        {
            cout << "VM correspondante inexistante" << endl;
            cout << qSELECT_ID_vm << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_ID_vm);


        /***** SELECT QUANTITY *****/

        char qSELECT_QUANTITY[500] = "";
        char rQUANTITY[500] = "";

        sprintf(qSELECT_QUANTITY, "SELECT service.quantity FROM client JOIN service ON(client.ID_client = service.ID_client) JOIN vm ON(service.ID_service = vm.ID_service) JOIN metrics ON(service.M_description_service = metrics.ID_metrics) WHERE correspondence_metrics = 'vCloud' AND ID_vm = '%s';", rID_vm);
        mysql_query(connexion, qSELECT_QUANTITY);

        MYSQL_RES* RES_QUANTITY = mysql_use_result(connexion);
        MYSQL_ROW ROW_QUANTITY = NULL;

        while (ROW_QUANTITY = mysql_fetch_row(RES_QUANTITY))
        {
            unsigned long* L_QUANTITY = mysql_fetch_lengths(RES_QUANTITY);

            for (int i = 0; i < 1; i++)
                sprintf(rQUANTITY, "%.*s", (int)L_QUANTITY[i], ROW_QUANTITY[i] ? ROW_QUANTITY[i] : "NULL");
        }

        if (mysql_num_rows(RES_QUANTITY) == 0) // SI REQUETE VIDE
        {
            cout << "Quantité de la VM correspondante inexistant" << endl;
            cout << qSELECT_QUANTITY << endl;
            exit(0);
        }

        mysql_free_result(RES_QUANTITY);

        /***** INCREMENT & UPDATE QUANTITY *****/

        int QuantityINCR = atoi(rQUANTITY);
        QuantityINCR++;

        char qUPDATE_QUANTITY[500] = "";

        sprintf(qUPDATE_QUANTITY, "UPDATE service JOIN client ON(client.ID_client = service.ID_client) JOIN vm ON(service.ID_service = vm.ID_service) JOIN metrics ON(service.M_description_service = metrics.ID_metrics) SET service.quantity = '%d' WHERE correspondence_metrics = 'vCloud' AND ID_vm = '%s';", QuantityINCR, rID_vm);
        mysql_query(connexion, qUPDATE_QUANTITY);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qUPDATE_QUANTITY << endl;
            exit(0);
        }
    }
    else
    {
        /***** UPDATE VM *****/

        char qUPDATE_VM[500] = "";

        sprintf(qUPDATE_VM, "UPDATE vm SET UUID = '%s', UID = '%s', M_OS = '%s', vmName = '%s', hostName = '%s', M_SLA = '%s', guestOSCustomization = '%s', M_HWVersion = '%s', vmware_Tools = '%s', current = '1' WHERE ID_vm = '%s';", UUID.c_str(), UID.c_str(), M_OS.c_str(), vmName.c_str(), hostName.c_str(), M_SLA.c_str(), guestOSCustomization.c_str(), M_HWVersion.c_str(), vmware_Tools.c_str(), rID_vm);
        mysql_query(connexion, qUPDATE_VM);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qUPDATE_VM << endl;
            exit(0);
        }
    }

    ID_vm = rID_vm;

    mysql_free_result(RES_ID_vm);
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

void report::UP(MYSQL* connexion, disk Ldisk, ram Lram, cpu Lcpu, backup Lbackup, network Lnetwork)
{
    /***** SELECT ID_REPORT *****/

    char qSELECT_ID_REPORT[500] = "";
    char rID_REPORT[500] = "";

    sprintf(qSELECT_ID_REPORT, "SELECT ID_report FROM report WHERE ID_vm = '%s' AND RptDateHour = '%s' AND vApp = '%s' AND PowerOn = '%s';", ID_vm.c_str(), RptDateHour.c_str(), vApp.c_str(), PowerOn.c_str());
    mysql_query(connexion, qSELECT_ID_REPORT);

    MYSQL_RES* RES_ID_REPORT = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_REPORT = NULL;

    while (ROW_ID_REPORT = mysql_fetch_row(RES_ID_REPORT))
    {
        unsigned long* L_ID_REPORT = mysql_fetch_lengths(RES_ID_REPORT);

        for (int i = 0; i < 1; i++)
            sprintf(rID_REPORT, "%.*s", (int)L_ID_REPORT[i], ROW_ID_REPORT[i] ? ROW_ID_REPORT[i] : "NULL");
    }

    if (mysql_num_rows(RES_ID_REPORT) == 0) // SI REPORT INEXISTANT
    {
        /***** INSERT REPORT *****/

        char qINSERT_REPORT[500] = "";

        sprintf(qINSERT_REPORT, "INSERT INTO report (ID_vm, RptDateHour, vApp, PowerOn) VALUES ('%s', '%s', '%s', '%s');", ID_vm.c_str(), RptDateHour.c_str(), vApp.c_str(), PowerOn.c_str());
        mysql_query(connexion, qINSERT_REPORT);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_REPORT << endl;
            exit(0);
        }

        /***** SELECT NEW_ID_REPORT *****/

        mysql_query(connexion, qSELECT_ID_REPORT);

        MYSQL_RES* RES_NEW_ID_REPORT = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_ID_REPORT = NULL;

        while (ROW_NEW_ID_REPORT = mysql_fetch_row(RES_NEW_ID_REPORT))
        {
            unsigned long* L_NEW_ID_REPORT = mysql_fetch_lengths(RES_NEW_ID_REPORT);

            for (int i = 0; i < 1; i++)
                sprintf(rID_REPORT, "%.*s", (int)L_NEW_ID_REPORT[i], ROW_NEW_ID_REPORT[i] ? ROW_NEW_ID_REPORT[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_ID_REPORT) == 0) // SI REQUETE VIDE
        {
            cout << "Rapport correspondant inexistant" << endl;
            cout << qSELECT_ID_REPORT << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_ID_REPORT);

        /***** APPEL DES FONCTIONS ****/

        Ldisk.disk::UP(connexion, rID_REPORT);

        Lram.ram::UP(connexion, rID_REPORT);

        Lcpu.cpu::UP(connexion, rID_REPORT);

        Lbackup.backup::UP(connexion, rID_REPORT);

        Lnetwork.network::UP(connexion, rID_REPORT);
    }

    mysql_free_result(RES_ID_REPORT);
}


// DISK

disk::disk() : capacity_disk(""), M_type_disk(""), M_unit("")
{
}

disk::~disk()
{
}

void disk::setcapacity_disk(MYSQL* connexion, std::string diskcapacity_disk)
{
    int capacity = stoi(diskcapacity_disk);
    string disk_unit = "";

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

    capacity_disk = to_string(capacity);

    /***** SELECT ID_metrics *****/

    char qSELECT_ID_metrics[500] = "";
    char rID_metrics[500] = "";

    sprintf(qSELECT_ID_metrics, "SELECT ID_metrics FROM metrics where type_metrics = 'unit' AND correspondence_metrics = '%s';", disk_unit.c_str());
    mysql_query(connexion, qSELECT_ID_metrics);

    MYSQL_RES* RES_ID_metrics = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_metrics = NULL;

    while (ROW_ID_metrics = mysql_fetch_row(RES_ID_metrics))
    {
        unsigned long* L_ID_metrics = mysql_fetch_lengths(RES_ID_metrics);

        for (int i = 0; i < 1; i++)
            sprintf(rID_metrics, "%.*s", (int)L_ID_metrics[i], ROW_ID_metrics[i] ? ROW_ID_metrics[i] : "NULL");
    }

    if (mysql_num_rows(RES_ID_metrics) == 0) // SI REQUETE VIDE
    {
        cout << "Metrique de la capacité du disk correspondant inexistant" << endl;
        cout << qSELECT_ID_metrics << endl;
        exit(0);
    }

    M_unit = rID_metrics;

    mysql_free_result(RES_ID_metrics);
}

void disk::setM_type_disk(MYSQL* connexion, std::string diskM_type_disk)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_type_disk[500] = "";
    char rM_type_disk[500] = "";

    sprintf(qSELECT_M_type_disk, "SELECT ID_metrics FROM metrics where type_metrics = 'type_disk' AND correspondence_metrics = '%s';", diskM_type_disk.c_str());
    mysql_query(connexion, qSELECT_M_type_disk);

    MYSQL_RES* RES_M_type_disk = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_type_disk = NULL;

    while (ROW_M_type_disk = mysql_fetch_row(RES_M_type_disk))
    {
        unsigned long* L_M_type_disk = mysql_fetch_lengths(RES_M_type_disk);

        for (int i = 0; i < 1; i++)
            sprintf(rM_type_disk, "%.*s", (int)L_M_type_disk[i], ROW_M_type_disk[i] ? ROW_M_type_disk[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_type_disk) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_M_type_disk[500] = "";

        sprintf(qINSERT_M_type_disk, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('type_disk', '%s');", diskM_type_disk.c_str());
        mysql_query(connexion, qINSERT_M_type_disk);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_M_type_disk << endl;
            exit(0);
        }

        /***** SELECT NEW_ID_metrics *****/

        mysql_query(connexion, qSELECT_M_type_disk);

        MYSQL_RES* RES_NEW_M_type_disk = mysql_use_result(connexion);
        MYSQL_ROW ROW_NEW_M_type_disk = NULL;

        while (ROW_NEW_M_type_disk = mysql_fetch_row(RES_NEW_M_type_disk))
        {
            unsigned long* L_NEW_M_type_disk = mysql_fetch_lengths(RES_NEW_M_type_disk);

            for (int i = 0; i < 1; i++)
                sprintf(rM_type_disk, "%.*s", (int)L_NEW_M_type_disk[i], ROW_NEW_M_type_disk[i] ? ROW_NEW_M_type_disk[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_type_disk) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique du type du disk correspondant inexistante" << endl;
            cout << qSELECT_M_type_disk << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_type_disk);
    }

    M_type_disk = rM_type_disk;

    mysql_free_result(RES_M_type_disk);
}

void disk::UP(MYSQL* connexion, char* rreport)
{
    /***** INSERT disk *****/

    char qINSERT_disk[500] = "";

    sprintf(qINSERT_disk, "INSERT INTO disk (ID_report, capacity_disk, M_type_disk, M_unit) VALUES ('%s', '%s', '%s', '%s');", rreport, capacity_disk.c_str(), M_type_disk.c_str(), M_unit.c_str());
    mysql_query(connexion, qINSERT_disk);

    if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
    {
        fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
        cout << qINSERT_disk << endl;
        exit(0);
    }
}


// RAM

ram::ram() : capacity_ram(""), M_unit("")
{
}

ram::~ram()
{
}

void ram::setcapacity_ram(MYSQL* connexion, std::string ramcapacity_ram)
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

    capacity_ram = to_string(capacity);

    /***** SELECT ID_metrics *****/

    char qSELECT_ID_metrics[500] = "";
    char rID_metrics[500] = "";

    sprintf(qSELECT_ID_metrics, "SELECT ID_metrics FROM metrics where type_metrics = 'unit' AND correspondence_metrics = '%s';", ram_unit.c_str());
    mysql_query(connexion, qSELECT_ID_metrics);

    MYSQL_RES* RES_ID_metrics = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_metrics = NULL;

    while (ROW_ID_metrics = mysql_fetch_row(RES_ID_metrics))
    {
        unsigned long* L_ID_metrics = mysql_fetch_lengths(RES_ID_metrics);

        for (int i = 0; i < 1; i++)
            sprintf(rID_metrics, "%.*s", (int)L_ID_metrics[i], ROW_ID_metrics[i] ? ROW_ID_metrics[i] : "NULL");
    }

    if (mysql_num_rows(RES_ID_metrics) == 0) // SI REQUETE VIDE
    {
        cout << "Metrique de la capacité da la ram correspondante inexistante" << endl;
        cout << qSELECT_ID_metrics << endl;
        exit(0);
    }

    M_unit = rID_metrics;

    mysql_free_result(RES_ID_metrics);
}

void ram::UP(MYSQL* connexion, char* rreport)
{
    /***** INSERT ram *****/

    char qINSERT_ram[500] = "";

    sprintf(qINSERT_ram, "INSERT INTO ram (ID_report, capacity_ram, M_unit) VALUES ('%s', '%s', '%s');", rreport, capacity_ram.c_str(), M_unit.c_str());
    mysql_query(connexion, qINSERT_ram);

    if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
    {
        fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
        cout << qINSERT_ram << endl;
        exit(0);
    }
}


// CPU

cpu::cpu() : nbrCore(""), M_model_cpu("")
{
}

cpu::~cpu()
{
}

void cpu::setnbrCore(std::string cpunbrCore)
{
    int CorINT = stoi(cpunbrCore); //conversion vers INT pour éviter problèmes d'insertion de nombre à virgule

    nbrCore = to_string(CorINT); //conversion pour adaptation à la classe
}

void cpu::setM_model_cpu(MYSQL* connexion, std::string cpuM_model_cpu)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_model_cpu[500] = "";
    char rM_model_cpu[500] = "";

    sprintf(qSELECT_M_model_cpu, "SELECT ID_metrics FROM metrics where type_metrics = 'model_cpu' AND correspondence_metrics = '%s';", cpuM_model_cpu.c_str());
    mysql_query(connexion, qSELECT_M_model_cpu);

    MYSQL_RES* RES_M_model_cpu = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_model_cpu = NULL;

    while (ROW_M_model_cpu = mysql_fetch_row(RES_M_model_cpu))
    {
        unsigned long* L_M_model_cpu = mysql_fetch_lengths(RES_M_model_cpu);

        for (int i = 0; i < 1; i++)
            sprintf(rM_model_cpu, "%.*s", (int)L_M_model_cpu[i], ROW_M_model_cpu[i] ? ROW_M_model_cpu[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_model_cpu) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_METRICS[500] = "";

        sprintf(qINSERT_METRICS, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('model_cpu', '%s');", cpuM_model_cpu.c_str());
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
                sprintf(rM_model_cpu, "%.*s", (int)L_NEW_M_model_cpu[i], ROW_NEW_M_model_cpu[i] ? ROW_NEW_M_model_cpu[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_model_cpu) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique de TODO correspondant inexistant" << endl;
            cout << qSELECT_M_model_cpu << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_model_cpu);
    }

    M_model_cpu = rM_model_cpu;

    mysql_free_result(RES_M_model_cpu);
}

void cpu::UP(MYSQL* connexion, char* rreport)
{
    /***** INSERT cpu *****/

    char qINSERT_cpu[500] = "";

    sprintf(qINSERT_cpu, "INSERT INTO cpu (ID_report, M_model_cpu, nbrCore) VALUES ('%s', '%s', '%s');", rreport, M_model_cpu.c_str(), nbrCore.c_str());
    mysql_query(connexion, qINSERT_cpu);

    if (mysql_errno(connexion) != 0) // SI ERREUR REQUETE
    {
        fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
        cout << qINSERT_cpu << endl;
        exit(0);
    }
}


// BACKUP

backup::backup() : M_type_backup("")
{
}

backup::~backup()
{
}

void backup::setM_type_backup(MYSQL* connexion, std::string backupM_type_backup)
{
    /***** SELECT M_type_backup *****/

    char qSELECT_M_type_backup[500] = "";
    char rM_type_backup[500] = "";

    sprintf(qSELECT_M_type_backup, "SELECT ID_metrics FROM metrics where type_metrics = 'type_backup' AND correspondence_metrics = '%s';", backupM_type_backup.c_str());
    mysql_query(connexion, qSELECT_M_type_backup);

    MYSQL_RES* RES_M_type_backup = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_type_backup = NULL;

    while (ROW_M_type_backup = mysql_fetch_row(RES_M_type_backup))
    {
        unsigned long* L_M_type_backup = mysql_fetch_lengths(RES_M_type_backup);

        for (int i = 0; i < 1; i++)
            sprintf(rM_type_backup, "%.*s", (int)L_M_type_backup[i], ROW_M_type_backup[i] ? ROW_M_type_backup[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_type_backup) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_M_type_backup[500] = "";

        sprintf(qINSERT_M_type_backup, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('type_backup', '%s');", backupM_type_backup.c_str());
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
                sprintf(rM_type_backup, "%.*s", (int)L_NEW_M_type_backup[i], ROW_NEW_M_type_backup[i] ? ROW_NEW_M_type_backup[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_type_backup) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique du type de backup correspondant inexistant" << endl;
            cout << qSELECT_M_type_backup << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_type_backup);
    }

    M_type_backup = rM_type_backup;

    mysql_free_result(RES_M_type_backup);
}

void backup::UP(MYSQL* connexion, char* rreport)
{
    /***** INSERT backup *****/

    char qINSERT_backup[500] = "";

    sprintf(qINSERT_backup, "INSERT INTO backup (ID_report, M_type_backup) VALUES ('%s', '%s');", rreport, M_type_backup.c_str());
    mysql_query(connexion, qINSERT_backup);

    if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
    {
        fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
        cout << qINSERT_backup << endl;
        exit(0);
    }
}


// NETWORK

network::network() : MAC_address(""), IP_address("")
{
}

network::~network()
{
}

void network::setIP_address(std::string networkIP_address)
{
    IP_address = networkIP_address;
}

void network::setMAC_address(std::string networkMAC_address)
{
    MAC_address = networkMAC_address;
}

void network::UP(MYSQL* connexion, char* rreport)
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
        /***** INSERT network *****/

        char qINSERT_network[500] = "";

        sprintf(qINSERT_network, "INSERT INTO network (ID_report, MAC_address, IP_address) VALUES ('%s', '%s', '%s');", rreport, tabMAC[j].c_str(), tabIP[j].c_str());
        mysql_query(connexion, qINSERT_network);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_network << endl;
            exit(0);
        }
    }
}