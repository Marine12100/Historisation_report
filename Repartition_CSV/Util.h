#pragma once

#include <mysql.h>
#include <vector>
#include <string>
#include <dirent.h>
#include <fstream>
#include <regex>

#include "VM.h"
#include "Report.h"
#include "Ram.h"
#include "Disk.h"
#include "Cpu.h"
#include "Backup.h"
#include "Network.h"

class Util
{
	public:
		static void exploration(MYSQL* connexion, const char* chemin);
		static std::vector<std::string> creation(std::string line);
		static void repartition(MYSQL* connexion, std::vector<std::string>& tableau, Vm& Lvm, Report& Lreport, Disk& Ldisk, Ram& Lram, Cpu& Lcpu, Backup& Lbackup, Network& Lnetwork);
		static void insertion(MYSQL* connexion, Vm& Lvm, Report& Lreport, Disk& Ldisk, Ram& Lram, Cpu& Lcpu, Backup& Lbackup, Network& Lnetwork);
};

