#include "crawler.h"

std::string download_page(const Link& url)
{
    net::io_context ioc;
    net::ssl::context ctx(net::ssl::context::sslv23_client);
    int redirect_limit = 5;
    Link current_url = url;

    for (int i = 0; i < redirect_limit; ++i)
    {
        try
        {
            std::string host = current_url.host;
            std::string target = current_url.target;

            tcp::resolver resolver(ioc);
            beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

            auto const results = resolver.resolve(host, "443");
            beast::get_lowest_layer(stream).connect(results);
           
            if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
            {
                boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
                throw boost::system::system_error{ ec };
            }

            stream.handshake(boost::asio::ssl::stream_base::client);

            http::request<boost::beast::http::string_body> req{ http::verb::get, target, 11 };
            req.set(http::field::host, host);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            http::write(stream, req);

            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);

            std::cout << "RESULT INT = " << res.result_int() << std::endl;
            if (res.result_int() >= 300 && res.result_int() < 400) {
                if (res.find(http::field::location) != res.end()) {
                    std::string str_url = res[http::field::location];
                    current_url = SplitLink(str_url);
                    std::cout << "Redirect № " << i << " to: " << str_url << std::endl;
                    continue;
                }
                else {
                    throw std::runtime_error("Редирект без заголовка Location");
                }
            }
           
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
}
