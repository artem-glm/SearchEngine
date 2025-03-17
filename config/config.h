#pragma once

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "../spider/link.h"

struct Config {
	std::string db_host;
	int db_port;
	std::string db_name, db_user, db_password;
	std::string start_url;
	int recursion_depth;
	int server_port;
};

Config LoadConfig(const std::string& file);
void PrintConfig(const Config& conf);