#include "Network.h"

using namespace std;

Network::Network() : m_MAC_address(""), m_IP_address("")
{
}

Network::~Network()
{
}

void Network::setIP_address(std::string networkIP_address)
{
    m_IP_address = networkIP_address;
}

void Network::setMAC_address(std::string networkMAC_address)
{
    m_MAC_address = networkMAC_address;
}

void Network::UP(MYSQL* connexion, char* rreport)
{
    /****** MAC EXTRACTION *****/

    vector<string> tabMAC;
    int macChainSize = (size(m_MAC_address) / MAC_LENGH);

    for (int i = 0; i < macChainSize; i++)
    {
        // Transfer mac address from Mac_address string to tabMac
        tabMAC.push_back(m_MAC_address.substr(size(m_MAC_address) - MAC_LENGH, size(m_MAC_address)));

        for (int i = 0; i < MAC_LENGH; i++)
        {
            // deleting transfered mac address
            m_MAC_address.pop_back();
        }
    }

    if (tabMAC.size() == 0) // si la vm ne possede pas d'adresse MAC, pour enregistrer champ vide
    {
        tabMAC.push_back("");
    }


    /***** IP EXTRACTION *****/

    vector<string> tabIP;
    stringstream ssIP(m_IP_address);
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

        sprintf_s(qINSERT_network, "INSERT INTO network (ID_report, MAC_address, IP_address) VALUES ('%s', '%s', '%s');", rreport, tabMAC[j].c_str(), tabIP[j].c_str());
        mysql_query(connexion, qINSERT_network);

        if (mysql_errno(connexion) != 0) // SI ERREUR DE REQUETE
        {
            fprintf(stderr, "Erreur de requete: %s\n", mysql_error(connexion));
            cout << qINSERT_network << endl;
            exit(0);
        }
    }
}