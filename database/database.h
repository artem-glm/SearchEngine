#pragma once

#include <iostream>
#include <pqxx/pqxx>
#include "../config/config.h"

class Database
{
public:
	Database(const Config& conf);
	void createTables();
	void InsertRow(const std::string& url, const std::map<std::string, int>& word_count);

private:
	pqxx::connection conn;
};



