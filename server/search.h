#ifndef SEARCH_H
#define SEARCH_H

#include <boost/beast.hpp>
#include <pqxx/pqxx>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;

void handle_search_request(http::request<http::string_body> req, http::response<http::string_body>& res, pqxx::connection& db_conn);

#endif