#include "utils.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <regex>
#include <iomanip>

std::string load_html_page(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return "Ошибка: не найден HTML-файл";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string extract_query_from_request(const std::string& body) {
    std::regex query_regex("query=([^&]*)");
    std::smatch match;
    if (std::regex_search(body, match, query_regex)) {
        return match[1].str();
    }
    return "";
}

std::string format_sql_query(const std::string& query) {
    std::stringstream ss;
    std::istringstream iss(query);
    std::string word;
    bool first = true;
    while (iss >> word) {
        if (!first) ss << ", ";
        ss << "'" << word << "'";
        first = false;
    }
    return ss.str();
}

std::vector<std::string> split_words(const std::string& text)
{
    std::vector<std::string> words;
    std::string word;
    std::istringstream stream(text);

    for (char c : text) {
        if (std::isalnum(c)) {
            word += std::tolower(c);
        }
        else if (!word.empty()) {
            words.push_back(word);
            word.clear();
        }
    }

    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

std::string url_decode(const std::string& encoded)
{
    std::ostringstream decoded;
    size_t length = encoded.length();

    for (size_t i = 0; i < length; ++i) {
        if (encoded[i] == '%' && i + 2 < length) {
            std::istringstream hex_stream(encoded.substr(i + 1, 2));
            int hex_code;
            if (hex_stream >> std::hex >> hex_code) {
                decoded << static_cast<char>(hex_code);
                i += 2;
            }
        }
        else if (encoded[i] == '+') {
            decoded << ' ';
        }
        else {
            decoded << encoded[i];
        }
    }
    return decoded.str();
}
