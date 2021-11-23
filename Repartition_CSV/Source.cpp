#include "Header.h"

using namespace std;

std::vector<std::string> creation(std::string line)
{
    vector <string> tableau;
    stringstream lineStream(line);
    string cell;

    while (getline(lineStream, cell, ','))
    {
        cell.erase(remove(cell.begin(), cell.end(), '"'), cell.end()); //supprimer les guillemets de la cellule
        tableau.push_back(cell); //ins�rer la cellule dans le tableau
    }

    if (size(tableau) > 33)
    {
        int i = 0;

        do
        {
            //expression r�guli�re d'une adresse MAC
            regex rgx("(([0-9A-Fa-f]{2}[:]){5}[0-9A-Fa-f]{2})");

            //recherche d'une adresse MAC dans le tableau
            bool MAC1 = regex_search(tableau[i], rgx);
            bool MAC2 = regex_search(tableau[i + 1], rgx);

            if (MAC1 && MAC2) //si deux MAC � la suite trouv�es
            {
                tableau[i] += tableau[i + 1]; //concat�nation des MAC
                tableau.erase(tableau.begin() + (i + 1));
            }
            else //si non trouv�es
            {
                i++;
            }

        } while (tableau.size() > 33); //tant que le tableau contient plus de 33 �l�ments
    }

    return tableau;
}

void repartition(vector<string>& tableau, vm &Lvm, report& Lreport, disk& Ldisk, ram& Lram, cpu& Lcpu, backup& Lbackup, network& Lnetwork)
{
    Lvm.setUUID(tableau[0]);
    Lvm.setUID(tableau[1]);
    Lvm.setvmName(tableau[2]);
    Lvm.setID_service(tableau[3]);
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
    Lvm.setguestOSCustomization(tableau[28]);
    Lvm.setM_HWVersion(tableau[29]);
}

void insertion(vm& Lvm, report& Lreport, disk& Ldisk, ram& Lram, cpu& Lcpu, backup& Lbackup, network& Lnetwork)
{
    Lvm.UP();

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

void vm::setID_service(string VMID_service)
{
    MYSQL connexion;
    mysql_init(&connexion);
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0)) //Connexion BDD
    {
        char id_service[300] = "";
        char qID_service[500] = "";

        //Requ�te
        sprintf(qID_service, "SELECT service.ID_service FROM client JOIN service ON (client.ID_client = service.ID_client) JOIN metrics ON (service.M_description_service = metrics.ID_metrics) WHERE correspondence_metrics = 'vCloud' AND code_client = '%s';", VMID_service.c_str());

        mysql_query(&connexion, qID_service);

        MYSQL_RES* rID_service;
        rID_service = NULL;
        MYSQL_ROW row_service;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rID_service = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rID_service);

        while ((row_service = mysql_fetch_row(rID_service)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rID_service);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration valeur
                sprintf(id_service, "%.*s ", (int)lengths[m], row_service[m] ? row_service[m] : "NULL");
                
            }
            VMID_service = id_service;
        }

        if (mysql_num_rows(rID_service) == 0)
        {
            do
            {
                //Requ�te insertion du nouveau client

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO client (code_client) VALUES ('%s');", ID_service.c_str());

                mysql_query(&connexion, qIN);


                //Acquisition de l'ID du client cr��

                char qID_client[500] = "";
                char id_client[500] = "";

                sprintf(qID_client, "SELECT ID_client FROM client WHERE code_client = '%s';", ID_service.c_str());

                mysql_query(&connexion, qID_client);

                MYSQL_RES* rID_client;
                rID_client = NULL;
                MYSQL_ROW row_client;

                unsigned int m;
                m = 0;
                unsigned int num_champs;
                num_champs = 0;

                rID_client = mysql_use_result(&connexion);

                num_champs = mysql_num_fields(rID_client);

                while ((row_client = mysql_fetch_row(rID_client)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rID_client);

                    for (m = 0; m < 1; m++)
                    {
                        //R�cup�ration valeur
                        sprintf(id_client, "%.*s ", (int)lengths[m], row_client[m] ? row_client[m] : "NULL");

                    }
                }

                //Recherche de la m�trique du service du client

                char qMETRIQUE[500] = "";
                char M_description_service[500] = "";

                sprintf(qMETRIQUE, "SELECT ID_metrics FROM metrics WHERE type_metrics = 'service' AND correspondence_metrics = 'vCloud';");

                mysql_query(&connexion, qMETRIQUE);

                MYSQL_RES* rMETRIQUE;
                rMETRIQUE = NULL;
                MYSQL_ROW row_METRIQUE;

                unsigned int s;
                s = 0;
                unsigned int num_champs_s;
                num_champs_s = 0;

                rMETRIQUE = mysql_use_result(&connexion);

                num_champs_s = mysql_num_fields(rMETRIQUE);

                while ((row_METRIQUE = mysql_fetch_row(rMETRIQUE)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rMETRIQUE);

                    for (s = 0; s < 1; s++)
                    {
                        //R�cup�ration valeur
                        sprintf(M_description_service, "%.*s ", (int)lengths[s], row_METRIQUE[s] ? row_METRIQUE[s] : "NULL");

                    }
                }

                //Insertion de la table service du client cr��

                char qSER[500] = "";

                sprintf(qSER, "INSERT INTO service (ID_client, M_description_service, quantity) VALUES ('%s', '%s', '1');", ID_service.c_str(), M_description_service);

                mysql_query(&connexion, qSER);


            } while (mysql_num_rows(rID_service) != 0); //Tant que le client n'existe pas
        }        
    }
    else //Erreur connexion
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }
    
    ID_service = VMID_service;
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
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0)) //Connexion
    {
        char OS[300] = "";
        char qOS[500] = "";
        char qIN[500] = "";

        //Requ�te r�cup�ration de l'ID de la m�trique
        sprintf(qOS, "SELECT ID_metrics FROM metrics where type_metrics = 'OS' AND correspondence_metrics = '%s';", VMM_OS.c_str());

        mysql_query(&connexion, qOS);

        MYSQL_RES* rOS;
        rOS = NULL;
        MYSQL_ROW row_OS;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rOS = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rOS);

        while ((row_OS = mysql_fetch_row(rOS)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rOS);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration de l'ID
                sprintf(OS, "%.*s ", (int)lengths[m], row_OS[m] ? row_OS[m] : "NULL");

            }
        }

        //Si m�trique inexistante
        if (mysql_num_rows(rOS) == 0)
        {
            do
            {
                //Requ�te insertion de la nouvelle m�trique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('OS', '%s');", VMM_OS.c_str());

                mysql_query(&connexion, qIN);

                //R�cup�ratrion de l'ID de la nouvelle m�trique
                while ((row_OS = mysql_fetch_row(rOS)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rOS);

                    for (m = 0; m < 1; m++)
                    {
                        //R�cup�ration du nouvel ID
                        sprintf(OS, "%.*s ", (int)lengths[m], row_OS[m] ? row_OS[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rOS) != 0); //Jusqu'� ce que la m�trique existe
        }
        VMM_OS = OS;
    }
    else //Erreur Connexion
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }

    M_OS = VMM_OS;
}

void vm::setM_SLA(std::string VMM_SLA)
{
    MYSQL connexion;
    mysql_init(&connexion);
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0)) //Connexion
    {
        char SLA[300] = "";
        char qSLA[500] = "";
        char qIN[500] = "";

        //Requ�te r�cup�ration de l'ID de la m�trique
        sprintf(qSLA, "SELECT ID_metrics FROM metrics where type_metrics = 'SLA' AND correspondence_metrics = '%s';", VMM_SLA.c_str());
        mysql_query(&connexion, qSLA);

        MYSQL_RES* rSLA;
        rSLA = NULL;
        MYSQL_ROW row_SLA;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rSLA = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rSLA);

        while ((row_SLA = mysql_fetch_row(rSLA)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rSLA);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration de l'ID
                sprintf(SLA, "%.*s ", (int)lengths[m], row_SLA[m] ? row_SLA[m] : "NULL");

            }
        }

        //Si m�trique inexistante
        if (mysql_num_rows(rSLA) == 0)
        {
            do
            {
                //Requ�te insertion de la nouvelle m�trique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('SLA', '%s');", VMM_SLA.c_str());

                mysql_query(&connexion, qIN);

                //R�cup�ratrion de l'ID de la nouvelle m�trique
                while ((row_SLA = mysql_fetch_row(rSLA)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rSLA);

                    for (m = 0; m < 1; m++)
                    {
                        //R�cup�ration du nouvel ID
                        sprintf(SLA, "%.*s ", (int)lengths[m], row_SLA[m] ? row_SLA[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rSLA) != 0); //Jusqu'� ce que la m�trique existe
        }
        VMM_SLA = SLA;
    }
    else //Erreur Connexion
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }

    M_SLA = VMM_SLA;
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
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char HWVersion[300] = "";
        char qHWVersion[500] = "";
        char qIN[500] = "";

        //Requ�te r�cup�ration de l'ID de la m�trique
        sprintf(qHWVersion, "SELECT ID_metrics FROM metrics where type_metrics = 'HWVersion' AND correspondence_metrics = '%s';", VMM_HWVersion.c_str());

        mysql_query(&connexion, qHWVersion);

        MYSQL_RES* rHWVersion;
        rHWVersion = NULL;
        MYSQL_ROW row_HWVersion;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rHWVersion = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rHWVersion);

        while ((row_HWVersion = mysql_fetch_row(rHWVersion)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rHWVersion);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration de l'ID
                sprintf(HWVersion, "%.*s ", (int)lengths[m], row_HWVersion[m] ? row_HWVersion[m] : "NULL");

            }
        }

        //Si m�trique inexistante
        if (mysql_num_rows(rHWVersion) == 0)
        {
            do
            {
                //Requ�te insertion de la nouvelle m�trique
                sprintf(qIN, "INSERT INTO metrics (type_metrics, correspondence_metrics) VALUES ('HWVersion', '%s');", VMM_HWVersion.c_str());

                mysql_query(&connexion, qIN);

                //Ex�cution de la requ�te de r�cup�ratrion de l'ID de la nouvelle m�trique
                while ((row_HWVersion = mysql_fetch_row(rHWVersion)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rHWVersion);

                    for (m = 0; m < 1; m++)
                    {
                        //R�cup�ration du nouvel ID
                        sprintf(HWVersion, "%.*s ", (int)lengths[m], row_HWVersion[m] ? row_HWVersion[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rHWVersion) != 0); //Jusqu'� ce que la m�trique existe
        }
        VMM_HWVersion = HWVersion;
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }

    M_HWVersion = VMM_HWVersion;
}

void vm::UP()
{
    char id_vm[500] = "";

    MYSQL connexion;
    mysql_init(&connexion);
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char vm[300] = "";
        char qvm[500] = "";
        
        //V�rification de l'existence de la vm
        sprintf(qvm, "SELECT ID_vm FROM vm WHERE ID_service = '%s' AND UUID = '%s' AND UID = '%s';", ID_service.c_str(), UUID.c_str(), UID.c_str());
                
        mysql_query(&connexion, qvm);

        MYSQL_RES* rvm;
        rvm = NULL;
        MYSQL_ROW row_vm;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;
        
        rvm = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rvm);

        while ((row_vm = mysql_fetch_row(rvm)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rvm);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration des donn�es
                sprintf(vm, "%.*s ", (int)lengths[m], row_vm[m] ? row_vm[m] : "NULL");
            }
        }

        //Si vm inexistante
        if (mysql_num_rows(rvm) == 0)
        {            
            do
            {
                //Requ�te insertion de la nouvelle vm

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO vm (ID_service, UUID, UID, M_OS, vmName, hostName, M_SLA, guestOSCustomization, M_HWVersion, vmware_Tools, current) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '1');", ID_service.c_str(), UUID.c_str(), UID.c_str(), M_OS.c_str(), vmName.c_str(), hostName.c_str(), M_SLA.c_str(), guestOSCustomization.c_str(), M_HWVersion.c_str(), vmware_Tools.c_str());
                                
                mysql_query(&connexion, qIN);
                                

                //Acquisition de l'ID de la vm cr��e
                
                char qID_vm[500] = "";                

                sprintf(qID_vm, "SELECT ID_vm FROM vm WHERE UUID = '%s' AND UID = '%s' AND vmName = '%s' AND hostName = '%s';", UUID.c_str(), UID.c_str(), vmName.c_str(), hostName.c_str());

                mysql_query(&connexion, qID_vm);

                MYSQL_RES* rID_vm;
                rID_vm = NULL;
                MYSQL_ROW row_vm;

                unsigned int m;
                m = 0;
                unsigned int num_champs;
                num_champs = 0;

                rID_vm = mysql_use_result(&connexion);

                num_champs = mysql_num_fields(rID_vm);

                while ((row_vm = mysql_fetch_row(rID_vm)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rID_vm);

                    for (m = 0; m < 1; m++)
                    {
                        //R�cup�ration valeur
                        sprintf(id_vm, "%.*s ", (int)lengths[m], row_vm[m] ? row_vm[m] : "NULL");

                    }
                    ID_vm = id_vm;
                }
                                

                //Acquisition de la quantit� de vm du client
                
                char qSELECTQuantity[500] = "";
                char Quantity[500] = "";

                sprintf(qSELECTQuantity, "SELECT service.quantity FROM client JOIN service ON(client.ID_client = service.ID_client) JOIN vm ON(service.ID_service = vm.ID_service) JOIN metrics ON(service.M_description_service = metrics.ID_metrics) WHERE correspondence_metrics = 'vCloud' AND ID_vm = '%s';", id_vm);

                mysql_query(&connexion, qSELECTQuantity);

                MYSQL_RES* rQuantity;
                rQuantity = NULL;
                MYSQL_ROW row_Quantity;

                unsigned int n;
                n = 0;
                unsigned int num_champs_vm;
                num_champs_vm = 0;

                rQuantity = mysql_use_result(&connexion);

                num_champs_vm = mysql_num_fields(rQuantity);

                while ((row_Quantity = mysql_fetch_row(rQuantity)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rQuantity);

                    for (n = 0; n < 1; n++)
                    {
                        //R�cup�ration valeur
                        sprintf(Quantity, "%.*s ", (int)lengths[n], row_Quantity[n] ? row_Quantity[n] : "NULL");
                    }
                }
                                

                //Incr�mentation et insertion de la quantit�
                                
                char qINCRQuantity[500] = "";

                int quantite = atoi(Quantity);
                quantite++;
                string QuantityINCR = to_string(quantite);
                                
                sprintf(qINCRQuantity, "UPDATE service JOIN client ON(client.ID_client = service.ID_client) JOIN vm ON(service.ID_service = vm.ID_service) JOIN metrics ON(service.M_description_service = metrics.ID_metrics) SET service.quantity = '%s' WHERE correspondence_metrics = 'vCloud' AND ID_vm = '%s';", QuantityINCR.c_str(), id_vm);

                mysql_query(&connexion, qINCRQuantity);
                                                
            } while (mysql_num_rows(rvm) != 0); //Jusqu'� ce que la vm existe
        }
        else // mettre � jour les donn�es
        {
            char qUPDATE[500] = "";
            char qID_vm[500] = "";

            // Acquisition de l'ID de la vm
            sprintf(qID_vm, "SELECT ID_vm FROM vm WHERE UID = '%s' AND vmName = '%s';",UID.c_str(), vmName.c_str());

            mysql_query(&connexion, qID_vm);

            MYSQL_RES* rID_vm;
            rID_vm = NULL;
            MYSQL_ROW row_vm;

            unsigned int m;
            m = 0;
            unsigned int num_champs;
            num_champs = 0;

            rID_vm = mysql_use_result(&connexion);

            num_champs = mysql_num_fields(rID_vm);

            while ((row_vm = mysql_fetch_row(rID_vm)))
            {
                unsigned long* lengths;

                lengths = mysql_fetch_lengths(rID_vm);

                for (m = 0; m < 1; m++)
                {
                    //R�cup�ration valeur
                    sprintf(id_vm, "%.*s ", (int)lengths[m], row_vm[m] ? row_vm[m] : "NULL");

                }
                ID_vm = id_vm;
            }

            sprintf(qUPDATE, "UPDATE vm SET UUID = '%s', UID = '%s', M_OS = '%s', vmName = '%s', hostName = '%s', M_SLA = '%s', guestOSCustomization = '%s', M_HWVersion = '%s', vmware_Tools = '%s', current = '1' WHERE ID_vm = '%s';", UUID.c_str(), UID.c_str(), M_OS.c_str(), vmName.c_str(), hostName.c_str(), M_SLA.c_str(), guestOSCustomization.c_str(), M_HWVersion.c_str(), vmware_Tools.c_str(), ID_vm.c_str());

            mysql_query(&connexion, qUPDATE);
        }
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }
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
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char report[300] = "";
        char qreport[500] = "";

        //V�rification de l'existence du rapport
        sprintf(qreport, "SELECT ID_report FROM report WHERE ID_vm = '%s' AND RptDateHour = '%s' AND PowerOn = '%s';", ID_vm.c_str(), RptDateHour.c_str(), PowerOn.c_str());

        mysql_query(&connexion, qreport);

        MYSQL_RES* rreport;
        rreport = NULL;
        MYSQL_ROW row_report;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rreport = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rreport);

        while ((row_report = mysql_fetch_row(rreport)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rreport);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration des donn�es
                sprintf(report, "%.*s ", (int)lengths[m], row_report[m] ? row_report[m] : "NULL");
            }
        }

        //Si rapport inexistant
        if (mysql_num_rows(rreport) == 0)
        {
            do
            {
                //Requ�te insertion du nouveau rapport

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO report (ID_vm, RptDateHour, PowerOn) VALUES ('%s', '%s', '%s');", ID_vm.c_str(), RptDateHour.c_str(), PowerOn.c_str());

                mysql_query(&connexion, qIN);


                //Acquisition de l'ID du rapport cr��

                char qID_report[500] = "";

                sprintf(qID_report, "SELECT ID_report FROM report WHERE ID_vm = '%s' AND RptDateHour = '%s' AND PowerOn = '%s';", ID_vm.c_str(), RptDateHour.c_str(), PowerOn.c_str());

                mysql_query(&connexion, qID_report);

                MYSQL_RES* rID_report;
                rID_report = NULL;
                MYSQL_ROW row_report;

                unsigned int m;
                m = 0;
                unsigned int num_champs;
                num_champs = 0;

                rID_report = mysql_use_result(&connexion);

                num_champs = mysql_num_fields(rID_report);

                while ((row_report = mysql_fetch_row(rID_report)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rID_report);

                    for (m = 0; m < 1; m++)
                    {
                        //R�cup�ration valeur
                        sprintf(id_report, "%.*s ", (int)lengths[m], row_report[m] ? row_report[m] : "NULL");
                    }
                    ID_report = id_report;
                }
            } while (mysql_num_rows(rreport) != 0); //Jusqu'� ce que le rapport existe
        }
        else // recup�rer l'ID du rapport existant
        {
            char qUPDATE[500] = "";
            char qID_report[500] = "";

            // Acquisition de l'ID du rapport
            sprintf(qreport, "SELECT ID_report FROM report WHERE ID_vm = '%s' AND RptDateHour = '%s' AND PowerOn = '%s';", ID_vm.c_str(), RptDateHour.c_str(), PowerOn.c_str());

            mysql_query(&connexion, qreport);

            MYSQL_RES* rID_report;
            rID_report = NULL;
            MYSQL_ROW row_report;

            unsigned int m;
            m = 0;
            unsigned int num_champs;
            num_champs = 0;

            rID_report = mysql_use_result(&connexion);

            num_champs = mysql_num_fields(rID_report);

            while ((row_report = mysql_fetch_row(rID_report)))
            {
                unsigned long* lengths;

                lengths = mysql_fetch_lengths(rID_report);

                for (m = 0; m < 1; m++)
                {
                    //R�cup�ration valeur
                    sprintf(id_report, "%.*s ", (int)lengths[m], row_report[m] ? row_report[m] : "NULL");

                }
                ID_report = id_report;
            }
        }
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }
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
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

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

        //Requ�te r�cup�ration de l'ID de la m�trique
        sprintf(qunit, "SELECT ID_metrics FROM metrics where type_metrics = 'unit' AND correspondence_metrics = '%s';", disk_unit.c_str());

        mysql_query(&connexion, qunit);

        MYSQL_RES* runit;
        runit = NULL;
        MYSQL_ROW row_unit;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        runit = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(runit);

        while ((row_unit = mysql_fetch_row(runit)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(runit);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration de l'ID
                sprintf(unit, "%.*s ", (int)lengths[m], row_unit[m] ? row_unit[m] : "NULL");

            }
        }
        M_unit = unit;
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }

    capacity_disk = to_string(capacity);
}

void disk::setM_type_disk(std::string diskM_type_disk)
{
    MYSQL connexion;
    mysql_init(&connexion);
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char type_disk[300] = "";
        char qtype_disk[500] = "";
        char qIN[500] = "";

        //Requ�te r�cup�ration de l'ID de la m�trique
        sprintf(qtype_disk, "SELECT ID_metrics FROM metrics where type_metrics = 'type_disk' AND correspondence_metrics = '%s';", diskM_type_disk.c_str());

        mysql_query(&connexion, qtype_disk);

        MYSQL_RES* rtype_disk;
        rtype_disk = NULL;
        MYSQL_ROW row_type_disk;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rtype_disk = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rtype_disk);

        while ((row_type_disk = mysql_fetch_row(rtype_disk)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rtype_disk);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration de l'ID
                sprintf(type_disk, "%.*s ", (int)lengths[m], row_type_disk[m] ? row_type_disk[m] : "NULL");

            }
        }

        //Si m�trique inexistante
        if (mysql_num_rows(rtype_disk) == 0)
        {
            do
            {
                //Requ�te insertion de la nouvelle m�trique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('type_disk', '%s');", diskM_type_disk.c_str());

                mysql_query(&connexion, qIN);

                //R�cup�ratrion de l'ID de la nouvelle m�trique
                while ((row_type_disk = mysql_fetch_row(rtype_disk)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rtype_disk);

                    for (m = 0; m < 1; m++)
                    {
                        //R�cup�ration du nouvel ID
                        sprintf(type_disk, "%.*s ", (int)lengths[m], row_type_disk[m] ? row_type_disk[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rtype_disk) != 0); //Jusqu'� ce que la m�trique existe
        }
        diskM_type_disk = type_disk;
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }

    M_type_disk = diskM_type_disk;
}

void disk::UP()
{
    MYSQL connexion;
    mysql_init(&connexion);
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char disk[300] = "";
        char qdisk[500] = "";

        //V�rification de l'existence du disk
        sprintf(qdisk, "SELECT ID_disk FROM disk WHERE ID_report = '%s' AND capacity_disk = '%s' AND M_type_disk = '%s' AND M_unit = '%s';", ID_report.c_str(), capacity_disk.c_str(), M_type_disk.c_str(), M_unit.c_str());

        mysql_query(&connexion, qdisk);

        MYSQL_RES* rdisk;
        rdisk = NULL;
        MYSQL_ROW row_disk;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rdisk = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rdisk);

        while ((row_disk = mysql_fetch_row(rdisk)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rdisk);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration des donn�es
                sprintf(disk, "%.*s ", (int)lengths[m], row_disk[m] ? row_disk[m] : "NULL");
            }
        }

        //Si disk inexistant
        if (mysql_num_rows(rdisk) == 0)
        {
            do
            {
                //Requ�te insertion du nouveau disk

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO disk (ID_report, capacity_disk, M_type_disk, M_unit) VALUES ('%s', '%s', '%s', '%s');", ID_report.c_str(), capacity_disk.c_str(), M_type_disk.c_str(), M_unit.c_str());

                mysql_query(&connexion, qIN);

            } while (mysql_num_rows(rdisk) != 0); //Jusqu'� ce que le disk existe
        }
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }
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
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

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

        //Requ�te r�cup�ration de l'ID de la m�trique
        sprintf(qunit, "SELECT ID_metrics FROM metrics where type_metrics = 'unit' AND correspondence_metrics = '%s';", ram_unit.c_str());

        mysql_query(&connexion, qunit);

        MYSQL_RES* runit;
        runit = NULL;
        MYSQL_ROW row_unit;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        runit = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(runit);

        while ((row_unit = mysql_fetch_row(runit)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(runit);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration de l'ID
                sprintf(unit, "%.*s ", (int)lengths[m], row_unit[m] ? row_unit[m] : "NULL");

            }
        }
        M_unit = unit;
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }

    capacity_ram = to_string(capacity);
}

void ram::UP()
{
    MYSQL connexion;
    mysql_init(&connexion);
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char ram[300] = "";
        char qram[500] = "";

        //V�rification de l'existence de la ram
        sprintf(qram, "SELECT ID_ram FROM ram WHERE ID_report = '%s' AND capacity_ram = '%s' AND M_unit = '%s';", ID_report.c_str(), capacity_ram.c_str(), M_unit.c_str());

        mysql_query(&connexion, qram);

        MYSQL_RES* rram;
        rram = NULL;
        MYSQL_ROW row_ram;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rram = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rram);

        while ((row_ram = mysql_fetch_row(rram)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rram);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration des donn�es
                sprintf(ram, "%.*s ", (int)lengths[m], row_ram[m] ? row_ram[m] : "NULL");
            }
        }

        //Si ram inexistant
        if (mysql_num_rows(rram) == 0)
        {
            do
            {
                //Requ�te insertion de la nouvelle ram

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO ram (ID_report, capacity_ram, M_unit) VALUES ('%s', '%s', '%s');", ID_report.c_str(), capacity_ram.c_str(), M_unit.c_str());

                mysql_query(&connexion, qIN);

            } while (mysql_num_rows(rram) != 0); //Jusqu'� ce que la ram existe
        }
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }
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
    nbrCore = cpunbrCore;
}

void cpu::setM_model_cpu(std::string cpuM_model_cpu)
{
    MYSQL connexion;
    mysql_init(&connexion);
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char model_cpu[300] = "";
        char qmodel_cpu[500] = "";
        char qIN[500] = "";

        //Requ�te r�cup�ration de l'ID de la m�trique
        sprintf(qmodel_cpu, "SELECT ID_metrics FROM metrics where type_metrics = 'model_cpu' AND correspondence_metrics = '%s';", cpuM_model_cpu.c_str());

        mysql_query(&connexion, qmodel_cpu);

        MYSQL_RES* rmodel_cpu;
        rmodel_cpu = NULL;
        MYSQL_ROW row_model_cpu;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rmodel_cpu = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rmodel_cpu);

        while ((row_model_cpu = mysql_fetch_row(rmodel_cpu)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rmodel_cpu);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration de l'ID
                sprintf(model_cpu, "%.*s ", (int)lengths[m], row_model_cpu[m] ? row_model_cpu[m] : "NULL");

            }
        }
                
        if (mysql_num_rows(rmodel_cpu) == 0) //Si m�trique inexistante
        {
            do
            {
                //Requ�te insertion de la nouvelle m�trique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('model_cpu', '%s');", cpuM_model_cpu.c_str());

                mysql_query(&connexion, qIN);

                //R�cup�ratrion de l'ID de la nouvelle m�trique
                while ((row_model_cpu = mysql_fetch_row(rmodel_cpu)))
                {
                    unsigned long* lengths;
                    
                    lengths = mysql_fetch_lengths(rmodel_cpu);

                    for (m = 0; m < 1; m++)
                    {
                        //R�cup�ration du nouvel ID
                        sprintf(model_cpu, "%.*s ", (int)lengths[m], row_model_cpu[m] ? row_model_cpu[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rmodel_cpu) != 0); //Jusqu'� ce que la m�trique existe
        }

        cpuM_model_cpu = model_cpu;
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }

    M_model_cpu = cpuM_model_cpu;
}

void cpu::UP()
{
    MYSQL connexion;
    mysql_init(&connexion);
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char cpu[300] = "";
        char qcpu[500] = "";

        //V�rification de l'existence du cpu
        sprintf(qcpu, "SELECT ID_cpu FROM cpu WHERE ID_report = '%s' AND M_model_cpu = '%s' AND nbrCore = '%s';", ID_report.c_str(), M_model_cpu.c_str(), nbrCore.c_str());

        mysql_query(&connexion, qcpu);

        MYSQL_RES* rcpu;
        rcpu = NULL;
        MYSQL_ROW row_cpu;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rcpu = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rcpu);

        while ((row_cpu = mysql_fetch_row(rcpu)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rcpu);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration des donn�es
                sprintf(cpu, "%.*s ", (int)lengths[m], row_cpu[m] ? row_cpu[m] : "NULL");
            }
        }

        //Si cpu inexistant
        if (mysql_num_rows(rcpu) == 0)
        {
            do
            {
                //Requ�te insertion du nouveau cpu

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO cpu (ID_report, M_model_cpu, nbrCore) VALUES ('%s', '%s', '%s');", ID_report.c_str(), M_model_cpu.c_str(), nbrCore.c_str());

                mysql_query(&connexion, qIN);

            } while (mysql_num_rows(rcpu) != 0); //Jusqu'� ce que le cpu existe
        }
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }
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
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char type_backup[300] = "";
        char qtype_backup[500] = "";
        char qIN[500] = "";

        //Requ�te r�cup�ration de l'ID de la m�trique
        sprintf(qtype_backup, "SELECT ID_metrics FROM metrics where type_metrics = 'type_backup' AND correspondence_metrics = '%s';", backupM_type_backup.c_str());

        mysql_query(&connexion, qtype_backup);

        MYSQL_RES* rtype_backup;
        rtype_backup = NULL;
        MYSQL_ROW row_type_backup;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rtype_backup = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rtype_backup);

        while ((row_type_backup = mysql_fetch_row(rtype_backup)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rtype_backup);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration de l'ID
                sprintf(type_backup, "%.*s ", (int)lengths[m], row_type_backup[m] ? row_type_backup[m] : "NULL");

            }
        }

        //Si m�trique inexistante
        if (mysql_num_rows(rtype_backup) == 0)
        {
            do
            {
                //Requ�te insertion de la nouvelle m�trique
                sprintf(qIN, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('type_backup', '%s');", backupM_type_backup.c_str());

                mysql_query(&connexion, qIN);

                //R�cup�ratrion de l'ID de la nouvelle m�trique
                while ((row_type_backup = mysql_fetch_row(rtype_backup)))
                {
                    unsigned long* lengths;

                    lengths = mysql_fetch_lengths(rtype_backup);

                    for (m = 0; m < 1; m++)
                    {
                        //R�cup�ration du nouvel ID
                        sprintf(type_backup, "%.*s ", (int)lengths[m], row_type_backup[m] ? row_type_backup[m] : "NULL");

                    }
                }
            } while (mysql_num_rows(rtype_backup) != 0); //Jusqu'� ce que la m�trique existe
        }
        backupM_type_backup = type_backup;
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }

    M_type_backup = backupM_type_backup;
}

void backup::UP()
{
    MYSQL connexion;
    mysql_init(&connexion);
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        char backup[300] = "";
        char qbackup[500] = "";

        //V�rification de l'existence de la backup
        sprintf(qbackup, "SELECT ID_backup FROM backup WHERE ID_report = '%s' AND M_type_backup = '%s';", ID_report.c_str(), M_type_backup.c_str());

        mysql_query(&connexion, qbackup);

        MYSQL_RES* rbackup;
        rbackup = NULL;
        MYSQL_ROW row_backup;

        unsigned int m;
        m = 0;
        unsigned int num_champs;
        num_champs = 0;

        rbackup = mysql_use_result(&connexion);

        num_champs = mysql_num_fields(rbackup);

        while ((row_backup = mysql_fetch_row(rbackup)))
        {
            unsigned long* lengths;

            lengths = mysql_fetch_lengths(rbackup);

            for (m = 0; m < 1; m++)
            {
                //R�cup�ration des donn�es
                sprintf(backup, "%.*s ", (int)lengths[m], row_backup[m] ? row_backup[m] : "NULL");
            }
        }

        //Si backup inexistant
        if (mysql_num_rows(rbackup) == 0)
        {
            do
            {
                //Requ�te insertion de la nouvelle backup

                char qIN[500] = "";

                sprintf(qIN, "INSERT INTO backup (ID_report, M_type_backup) VALUES ('%s', '%s');", ID_report.c_str(), M_type_backup.c_str());

                mysql_query(&connexion, qIN);

            } while (mysql_num_rows(rbackup) != 0); //Jusqu'� ce que la backup existe
        }
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }
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
    mysql_options(&connexion, MYSQL_READ_DEFAULT_GROUP, "mix");

    if (mysql_real_connect(&connexion, "localhost", "root", "", "mix", 0, NULL, 0))
    {
        vector<string> tabMAC;
        int macChainSize = (size(MAC_address) / MAC_LENGH);

        // Mac address extraction
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

        for (int j = 0 ; j < tabMAC.size() ; j++)
        {
            char network[300] = "";
            char qnetwork[500] = "";

            //V�rification de l'existence du network
            sprintf(qnetwork, "SELECT ID_network FROM network WHERE ID_report = '%s' AND MAC_address = '%s' AND IP_address = '%s';", ID_report.c_str(), tabMAC[j].c_str(), IP_address.c_str());

            mysql_query(&connexion, qnetwork);

            MYSQL_RES* rnetwork;
            rnetwork = NULL;
            MYSQL_ROW row_network;

            unsigned int m;
            m = 0;
            unsigned int num_champs;
            num_champs = 0;

            rnetwork = mysql_use_result(&connexion);

            num_champs = mysql_num_fields(rnetwork);

            while ((row_network = mysql_fetch_row(rnetwork)))
            {
                unsigned long* lengths;

                lengths = mysql_fetch_lengths(rnetwork);

                for (m = 0; m < 1; m++)
                {
                    //R�cup�ration des donn�es
                    sprintf(network, "%.*s ", (int)lengths[m], row_network[m] ? row_network[m] : "NULL");
                }
            }

            //Si network inexistant
            if (mysql_num_rows(rnetwork) == 0)
            {
                do
                {
                    //Requ�te insertion du nouveau network

                    char qIN[500] = "";

                    sprintf(qIN, "INSERT INTO network (ID_report, MAC_address, IP_address) VALUES ('%s', '%s', '%s');", ID_report.c_str(), tabMAC[j].c_str(), IP_address.c_str());
                    mysql_query(&connexion, qIN);

                } while (mysql_num_rows(rnetwork) != 0); //Tant que que le network n'existe pas
            }
        }        
    }
    else
    {
        cout << "Erreur de connexion � la base de donn�es.\n";
    }
}