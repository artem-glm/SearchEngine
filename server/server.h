#ifndef SERVER_H
#define SERVER_H

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <pqxx/pqxx>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

void start_server(const std::string& address, int port, pqxx::connection& db_conn);
void handle_request(http::request<http::string_body> req, http::response<http::string_body>& res, pqxx::connection& db_conn);

#endif