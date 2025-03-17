#include "config.h"

Config LoadConfig(const std::string& file)
{
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(file, pt);

	Config config;
	config.db_host = pt.get<std::string>("DB_Conn.host");
	config.db_port = pt.get<int>("DB_Conn.port");
	config.db_name = pt.get<std::string>("DB_Conn.dbname");
	config.db_user = pt.get<std::string>("DB_Conn.user");
	config.db_password = pt.get<std::string>("DB_Conn.password");

	config.start_url = pt.get<std::string>("Crawler_config.startURL");
	config.recursion_depth = pt.get<int>("Crawler_config.recursionDepth");

	config.server_port = pt.get<int>("HTTP_config.serverPort");

	PrintConfig(config);

	return config;
}

void PrintConfig(const Config& conf)
{
	std::cout << "-----DB_Config-----" << std::endl;
	std::cout << "DB_Host = " << conf.db_host << std::endl;
	std::cout << "DB_Port = " << conf.db_port << std::endl;
	std::cout << "DB_Name = " << conf.db_name << std::endl;
	std::cout << "DB_User = " << conf.db_user << std::endl;
	std::cout << "DB_Password = " << conf.db_password << std::endl;
	std::cout << "Start_URL = " << conf.start_url << std::endl;
	std::cout << "\tURL_protocol: " << SplitLink(conf.start_url).protocol << std::endl;
	std::cout << "\tURL_host: " << SplitLink(conf.start_url).host<< std::endl;
	std::cout << "\tURL_target: " << SplitLink(conf.start_url).target << std::endl;
	std::cout << "Recursion_Depth = " << conf.recursion_depth << std::endl;
	std::cout << "Server_Port = " << conf.server_port << std::endl << std::endl;
}


