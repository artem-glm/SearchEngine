#include "search.h"
#include "utils.h"
#include <sstream>
#include <vector>


void handle_search_request(http::request<http::string_body> req, http::response<http::string_body>& res, pqxx::connection& db_conn) {
    std::string raw_query_text = extract_query_from_request(req.body());

    std::cout << raw_query_text << std::endl;

    if (raw_query_text.empty()) {
        res.result(http::status::bad_request);
        res.set(http::field::content_type, "application/json");
        res.body() = R"({"error": "Request must not be empty!"})";
        return;
    }

    std::string query_text = url_decode(raw_query_text);

    std::vector<std::string> words = split_words(query_text);
    if (words.empty() || words.size() > 4) {
        res.result(http::status::bad_request);
        res.set(http::field::content_type, "application/json");
        res.body() = R"({"error": "The query must contain from 1 to 4 words!"})";
        return;
    }

    try {
        pqxx::work txn(db_conn);
        std::ostringstream sql;

        sql << "SELECT d.url, SUM(wf.frequency) AS relevance "
            "FROM documents d "
            "JOIN word_frequency wf ON d.id = wf.document_id "
            "JOIN words w ON wf.word_id = w.id "
            "WHERE ";

        for (size_t i = 0; i < words.size(); ++i) {
            if (i > 0) sql << " OR ";
            sql << "w.word = " << txn.quote(words[i]);
        }

        sql << " GROUP BY d.id "
            "ORDER BY relevance DESC "
            "LIMIT 10;";

        pqxx::result result = txn.exec(sql.str());

        std::ostringstream response_json;
        response_json << "[";
        bool first = true;
        for (const auto& row : result) {
            if (!first) response_json << ",";
            response_json << R"({"url":")" << row[0].c_str() << R"(","relevance":)" << row[1].as<int>() << "}";
            first = false;
        }
        response_json << "]";

        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = response_json.str();
    }
    catch (const std::exception& e) {
        res.result(http::status::internal_server_error);
        res.set(http::field::content_type, "application/json");
        res.body() = R"({"error": "Request processing error"})";
    }
}