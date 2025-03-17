#include "server.h"
#include "search.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>

void handle_request(http::request<http::string_body> req, http::response<http::string_body>& res, pqxx::connection& db_conn) {
    if (req.method() == http::verb::get) {
        res.result(http::status::ok);
        res.set(http::field::content_type, "text/html");
        res.body() = load_html_page("../../../../server/index.html");
    }
    else if (req.method() == http::verb::post && req.target() == "/search") {
        handle_search_request(req, res, db_conn);
    }
    else {
        res.result(http::status::bad_request);
        res.body() = "Inclorrect HTTP method";
    }
}

void start_server(const std::string& address, int port, pqxx::connection& db_conn) {
    try
    {
        asio::io_context ioc;
        tcp::acceptor acceptor(ioc, tcp::endpoint(asio::ip::make_address(address), port));

        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            http::response<http::string_body> res;
            res.set(http::field::server, "CustomSearchServer");

            handle_request(req, res, db_conn);
            std::cout << "Server response:\n" << res.body() << std::endl;

            http::write(socket, res);
            socket.shutdown(tcp::socket::shutdown_send);
        }
    }
    catch (const boost::system::system_error& e) {
        std::cerr << "Error Boost.Asio: " << e.what() << std::endl;
    }
}