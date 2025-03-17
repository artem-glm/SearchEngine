#pragma once

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "link.h"
#include <iostream>
#include <Windows.h>
#include <boost/locale.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

std::string download_page(const Link& url);