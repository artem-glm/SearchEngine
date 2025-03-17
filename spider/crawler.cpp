#include "crawler.h"

std::string download_page(const Link& url)
{
    try
    {
        net::io_context ioc;
        net::ssl::context ctx(net::ssl::context::sslv23_client);
        tcp::resolver resolver(ioc);
        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

        std::string host = url.host;
        std::string target = url.target;

        auto const results = resolver.resolve(host, "443");
        beast::get_lowest_layer(stream).connect(results);

        stream.handshake(boost::asio::ssl::stream_base::client);

        http::request<boost::beast::http::string_body> req{ http::verb::get, target, 11 };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        beast::error_code ec;
        stream.shutdown(ec);

        return res.body();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка загрузки: " << e.what() << std::endl;
        return "";
    }
}
