#include "VM.h"

using namespace std;

Vm::Vm() : m_ID_service(""), m_UUID(""), m_UID(""), m_OS(""), m_vmName(""), m_hostName(""), m_SLA(""), m_guestOSCustomization(""), m_HWVersion(""), m_vmware_Tools("")
{
}

Vm::~Vm()
{
}

void Vm::setID_service(MYSQL* connexion, string VMOrg, string VMOrgFullName)
{
    /***** SELECT ID_client *****/

    char qSELECT_ID_client[500] = "";
    char rID_client[500] = "";

    sprintf_s(qSELECT_ID_client, "SELECT ID_client FROM client WHERE code_client = '%s';", VMOrg.c_str());
    mysql_query(connexion, qSELECT_ID_client);

    MYSQL_RES* RES_ID_client = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_client = NULL;

    while ((ROW_ID_client = mysql_fetch_row(RES_ID_client)))
    {
        unsigned long* L_ID_client = mysql_fetch_lengths(RES_ID_client);

        for (int i = 0; i < 1; i++)
            sprintf_s(rID_client, "%.*s", (int)L_ID_client[i], ROW_ID_client[i]);
    }

    if (mysql_num_rows(RES_ID_client) == 0) // SI CLIENT INEXISTANT
    {
        /***** INSERT CLIENT FANTOME *****/

        char qINSERT_client[500] = "";

        sprintf_s(qINSERT_client, "INSERT INTO client (code_client, social_reason) VALUES ('%s', '%s');", VMOrg.c_str(), VMOrgFullName.c_str());
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
                sprintf_s(rID_client, "%.*s", (int)L_NEW_ID_client[i], ROW_NEW_ID_client[i]);
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

        sprintf_s(qSELECT_ID_vCloud, "SELECT ID_metrics FROM metrics WHERE type_metrics = 'service' AND correspondence_metrics = 'vCloud';");
        mysql_query(connexion, qSELECT_ID_vCloud);

        MYSQL_RES* RES_ID_vCloud = mysql_use_result(connexion);
        MYSQL_ROW ROW_ID_vCloud = NULL;

        while ((ROW_ID_vCloud = mysql_fetch_row(RES_ID_vCloud)))
        {
            unsigned long* L_ID_vCloud = mysql_fetch_lengths(RES_ID_vCloud);

            for (int i = 0; i < 1; i++)
                sprintf_s(rID_vCloud, "%.*s", (int)L_ID_vCloud[i], ROW_ID_vCloud[i] ? ROW_ID_vCloud[i] : "NULL");
        }

        if (mysql_num_rows(RES_ID_vCloud) == 0) // SI REQUETE VIDE
        {
            /***** INSERT METRICS *****/

            char qINSERT_metrics[500] = "";

            sprintf_s(qINSERT_metrics, "INSERT INTO metrics (type_metrics, correspondence_metrics) VALUES('service', 'vCloud');");
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
                    sprintf_s(rID_vCloud, "%.*s", (int)L_NEW_ID_vCloud[i], ROW_NEW_ID_vCloud[i] ? ROW_NEW_ID_vCloud[i] : "NULL");
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

        sprintf_s(qINSERT_service, "INSERT INTO service (ID_client, M_description_service, quantity) VALUES ('%s', '%s', '0');", rID_client, rID_vCloud);
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

    sprintf_s(qSELECT_ID_service, "SELECT service.ID_service FROM client JOIN service ON (client.ID_client = service.ID_client) JOIN metrics ON (service.M_description_service = metrics.ID_metrics) WHERE correspondence_metrics = 'vCloud' AND client.ID_client = '%s';", rID_client);
    mysql_query(connexion, qSELECT_ID_service);

    MYSQL_RES* RES_ID_service = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_service = NULL;

    while ((ROW_ID_service = mysql_fetch_row(RES_ID_service)))
    {
        unsigned long* L_ID_service = mysql_fetch_lengths(RES_ID_service);

        for (int i = 0; i < 1; i++)
            sprintf_s(rID_service, "%.*s", (int)L_ID_service[i], ROW_ID_service[i] ? ROW_ID_service[i] : "NULL");
    }

    if (mysql_num_rows(RES_ID_service) == 0) // SI REQUETE VIDE
    {
        cout << "Service du client correspondant inexistant" << endl;
        cout << qSELECT_ID_service << endl;
        exit(0);
    }

    m_ID_service = rID_service;

    mysql_free_result(RES_ID_service);
}

void Vm::setUUID(std::string VMUUID)
{
    m_UUID = VMUUID;
}

void Vm::setUID(std::string VMUID)
{
    m_UID = VMUID;
}

void Vm::setVmName(std::string VMvmName)
{
    m_vmName = VMvmName;
}

void Vm::sethostName(std::string VMhostName)
{
    m_hostName = VMhostName;
}

void Vm::setM_OS(MYSQL* connexion, std::string VMM_OS)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_OS[500] = "";
    char rM_OS[500] = "";

    sprintf_s(qSELECT_M_OS, "SELECT ID_metrics FROM metrics where type_metrics = 'OS' AND correspondence_metrics = '%s';", VMM_OS.c_str());
    mysql_query(connexion, qSELECT_M_OS);

    MYSQL_RES* RES_M_OS = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_OS = NULL;

    while (ROW_M_OS = mysql_fetch_row(RES_M_OS))
    {
        unsigned long* L_ID_metrics = mysql_fetch_lengths(RES_M_OS);

        for (int i = 0; i < 1; i++)
            sprintf_s(rM_OS, "%.*s", (int)L_ID_metrics[i], ROW_M_OS[i] ? ROW_M_OS[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_OS) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_metrics[500] = "";

        sprintf_s(qINSERT_metrics, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('OS', '%s');", VMM_OS.c_str());
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
                sprintf_s(rM_OS, "%.*s", (int)L_NEW_ID_metrics[i], ROW_NEW_M_OS[i] ? ROW_NEW_M_OS[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_OS) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique de l'OS correspondante inexistante" << endl;
            cout << qSELECT_M_OS << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_OS);
    }

    m_OS = rM_OS;

    mysql_free_result(RES_M_OS);
}

void Vm::setM_SLA(MYSQL* connexion, std::string VMM_SLA)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_SLA[500] = "";
    char rM_SLA[500] = "";

    sprintf_s(qSELECT_M_SLA, "SELECT ID_metrics FROM metrics where type_metrics = 'SLA' AND correspondence_metrics = '%s';", VMM_SLA.c_str());
    mysql_query(connexion, qSELECT_M_SLA);

    MYSQL_RES* RES_M_SLA = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_SLA = NULL;

    while (ROW_M_SLA = mysql_fetch_row(RES_M_SLA))
    {
        unsigned long* L_M_SLA = mysql_fetch_lengths(RES_M_SLA);

        for (int i = 0; i < 1; i++)
            sprintf_s(rM_SLA, "%.*s", (int)L_M_SLA[i], ROW_M_SLA[i] ? ROW_M_SLA[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_SLA) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_M_SLA[500] = "";

        sprintf_s(qINSERT_M_SLA, "INSERT INTO metrics(type_metrics, correspondence_metrics) VALUES('SLA', '%s');", VMM_SLA.c_str());

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
                sprintf_s(rM_SLA, "%.*s", (int)L_NEW_M_SLA[i], ROW_NEW_M_SLA[i] ? ROW_NEW_M_SLA[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_SLA) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique du SLA correspondant inexistant" << endl;
            cout << qSELECT_M_SLA << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_SLA);
    }

    m_SLA = rM_SLA;

    mysql_free_result(RES_M_SLA);
}

void Vm::setguestOSCustomization(std::string VMguestOSCustomization)
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

    m_guestOSCustomization = VMguestOSCustomization;
}

void Vm::setM_HWVersion(MYSQL* connexion, std::string VMM_HWVersion)
{
    /***** SELECT ID_metrics *****/

    char qSELECT_M_HWVersion[500] = "";
    char rM_HWVersion[500] = "";

    sprintf_s(qSELECT_M_HWVersion, "SELECT ID_metrics FROM metrics where type_metrics = 'HWVersion' AND correspondence_metrics = '%s';", VMM_HWVersion.c_str());
    mysql_query(connexion, qSELECT_M_HWVersion);

    MYSQL_RES* RES_M_HWVersion = mysql_use_result(connexion);
    MYSQL_ROW ROW_M_HWVersion = NULL;

    while (ROW_M_HWVersion = mysql_fetch_row(RES_M_HWVersion))
    {
        unsigned long* L_M_HWVersion = mysql_fetch_lengths(RES_M_HWVersion);

        for (int i = 0; i < 1; i++)
            sprintf_s(rM_HWVersion, "%.*s", (int)L_M_HWVersion[i], ROW_M_HWVersion[i] ? ROW_M_HWVersion[i] : "NULL");
    }

    if (mysql_num_rows(RES_M_HWVersion) == 0) // SI METRIQUE INEXISTANTE
    {
        /***** INSERT METRICS *****/

        char qINSERT_M_HWVersion[500] = "";

        sprintf_s(qINSERT_M_HWVersion, "INSERT INTO metrics (type_metrics, correspondence_metrics) VALUES ('HWVersion', '%s');", VMM_HWVersion.c_str());
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
                sprintf_s(rM_HWVersion, "%.*s", (int)L_NEW_M_HWVersion[i], ROW_NEW_M_HWVersion[i] ? ROW_NEW_M_HWVersion[i] : "NULL");
        }

        if (mysql_num_rows(RES_NEW_M_HWVersion) == 0) // SI REQUETE VIDE
        {
            cout << "Metrique du HWVersion correspondant inexistant" << endl;
            cout << qSELECT_M_HWVersion << endl;
            exit(0);
        }

        mysql_free_result(RES_NEW_M_HWVersion);
    }

    m_HWVersion = rM_HWVersion;

    mysql_free_result(RES_M_HWVersion);
}

void Vm::UP(MYSQL* connexion)
{
    /***** SELECT ID_vm *****/

    char qSELECT_ID_vm[500] = "";
    char rID_vm[500] = "";

    sprintf_s(qSELECT_ID_vm, "SELECT ID_vm FROM vm WHERE ID_service = '%s' AND vmName = '%s';", m_ID_service.c_str(), m_vmName.c_str());
    mysql_query(connexion, qSELECT_ID_vm);

    MYSQL_RES* RES_ID_vm = mysql_use_result(connexion);
    MYSQL_ROW ROW_ID_vm = NULL;

    while (ROW_ID_vm = mysql_fetch_row(RES_ID_vm))
    {
        unsigned long* L_ID_vm = mysql_fetch_lengths(RES_ID_vm);

        for (int i = 0; i < 1; i++)
            sprintf_s(rID_vm, "%.*s", (int)L_ID_vm[i], ROW_ID_vm[i] ? ROW_ID_vm[i] : "NULL");
    }

    if (mysql_num_rows(RES_ID_vm) == 0) // SI VM INEXISTANTE
    {
        /***** INSERT VM *****/

        char qINSERT_VM[500] = "";

        sprintf_s(qINSERT_VM, "INSERT INTO vm (ID_service, UUID, UID, M_OS, vmName, hostName, M_SLA, guestOSCustomization, M_HWVersion, vmware_Tools, current) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '1');", m_ID_service.c_str(), m_UUID.c_str(), m_UID.c_str(), m_OS.c_str(), m_vmName.c_str(), m_hostName.c_str(), m_SLA.c_str(), m_guestOSCustomization.c_str(), m_HWVersion.c_str(), m_vmware_Tools.c_str());
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
                sprintf_s(rID_vm, "%.*s", (int)L_NEW_ID_vm[i], ROW_NEW_ID_vm[i] ? ROW_NEW_ID_vm[i] : "NULL");
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

        sprintf_s(qSELECT_QUANTITY, "SELECT service.quantity FROM client JOIN service ON(client.ID_client = service.ID_client) JOIN vm ON(service.ID_service = vm.ID_service) JOIN metrics ON(service.M_description_service = metrics.ID_metrics) WHERE correspondence_metrics = 'vCloud' AND ID_vm = '%s';", rID_vm);
        mysql_query(connexion, qSELECT_QUANTITY);

        MYSQL_RES* RES_QUANTITY = mysql_use_result(connexion);
        MYSQL_ROW ROW_QUANTITY = NULL;

        while (ROW_QUANTITY = mysql_fetch_row(RES_QUANTITY))
        {
            unsigned long* L_QUANTITY = mysql_fetch_lengths(RES_QUANTITY);

            for (int i = 0; i < 1; i++)
                sprintf_s(rQUANTITY, "%.*s", (int)L_QUANTITY[i], ROW_QUANTITY[i] ? ROW_QUANTITY[i] : "NULL");
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

        sprintf_s(qUPDATE_QUANTITY, "UPDATE service JOIN client ON(client.ID_client = service.ID_client) JOIN vm ON(service.ID_service = vm.ID_service) JOIN metrics ON(service.M_description_service = metrics.ID_metrics) SET service.quantity = '%d' WHERE correspondence_metrics = 'vCloud' AND ID_vm = '%s';", QuantityINCR, rID_vm);
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

        sprintf_s(qUPDATE_VM, "UPDATE vm SET UUID = '%s', UID = '%s', M_OS = '%s', vmName = '%s', hostName = '%s', M_SLA = '%s', guestOSCustomization = '%s', M_HWVersion = '%s', vmware_Tools = '%s', current = '1' WHERE ID_vm = '%s';", m_UUID.c_str(), m_UID.c_str(), m_OS.c_str(), m_vmName.c_str(), m_hostName.c_str(), m_SLA.c_str(), m_guestOSCustomization.c_str(), m_HWVersion.c_str(), m_vmware_Tools.c_str(), rID_vm);
        mysql_query(connexion, qUPDATE_VM);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qUPDATE_VM << endl;
            exit(0);
        }
    }

    m_ID_vm = rID_vm;

    mysql_free_result(RES_ID_vm);
}

string Vm::getID_vm()
{
    return m_ID_vm;
}