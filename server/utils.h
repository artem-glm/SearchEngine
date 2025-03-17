#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::string load_html_page(const std::string& filename);
std::string extract_query_from_request(const std::string& body);
std::string format_sql_query(const std::string& query);
std::vector<std::string> split_words(const std::string& text);
std::string url_decode(const std::string& encoded);

#endif