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

            boost::system::error_code ec_read;
            http::read(socket, buffer, req, ec_read);

            if (ec_read == http::error::end_of_stream || ec_read == asio::error::eof) {
                std::cout << "[Info] The client closed the connection before sending the request.\n";
                boost::system::error_code ec_shutdown;
                socket.shutdown(tcp::socket::shutdown_send, ec_shutdown);
                continue;
            }

            if (ec_read) {
                std::cerr << "[Error] Request reading error: " << ec_read.message() << std::endl;
                continue;
            }

            http::response<http::string_body> res;
            res.set(http::field::server, "CustomSearchServer");

            handle_request(req, res, db_conn);
            std::cout << "Server response:\n" << res.body() << std::endl;

            boost::system::error_code ec_write;
            http::write(socket, res, ec_write);
            if (ec_write) {
                std::cerr << "[Error] Error writing response: " << ec_write.message() << std::endl;
            }

            boost::system::error_code ec_shutdown;
            socket.shutdown(tcp::socket::shutdown_send, ec_shutdown);

            if (ec_shutdown && ec_shutdown != asio::error::eof) {
                std::cerr << "[Warning] Error terminating connection: " << ec_shutdown.message() << std::endl;
            }
        }
    }
    catch (const boost::system::system_error& e) {
        std::cerr << "[Fatal] Boost.Asio Error: " << e.what() << std::endl;
    }
}