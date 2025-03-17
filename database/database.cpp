#include "database.h"

Database::Database(const Config& conf) : conn("dbname = " + conf.db_name +
												" user=" + conf.db_user +
												" password=" + conf.db_password +
												" host=" + conf.db_host +
												" port=" + std::to_string(conf.db_port))
{
    if (conn.is_open())
    {
        std::cout << "DB connection success" << std::endl;
    }
}

void Database::createTables()
{
	try
	{
		pqxx::transaction txn(conn);

        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS documents (
            id SERIAL PRIMARY KEY,
            url TEXT UNIQUE NOT NULL
            );
        )");

        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS words (
            id SERIAL PRIMARY KEY,
            word TEXT UNIQUE NOT NULL
            );
        )");

        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS word_frequency (
            document_id INT REFERENCES documents(id) ON DELETE CASCADE,
            word_id INT REFERENCES words(id) ON DELETE CASCADE,
            frequency INT NOT NULL,
            PRIMARY KEY (document_id, word_id)
            );
        )");

        txn.commit();
	}
	catch (const std::exception& e)
	{
        std::cerr << "Creating tables error: " << e.what() << std::endl;
	}
}

void Database::InsertRow(const std::string& url, const std::map<std::string, int>& word_count)
{
    pqxx::work txn(conn);

    int doc_id = txn.exec1("INSERT INTO documents (url) VALUES ('" + txn.esc(url) + "') RETURNING id;")[0].as<int>();

    for (const auto& [word, frequency] : word_count)
    {
        int word_id = txn.exec1("INSERT INTO words (word) VALUES ('" + txn.esc(word) + "') ON CONFLICT (word) DO UPDATE SET word=EXCLUDED.word RETURNING id;")[0].as<int>();
        txn.exec0("INSERT INTO word_frequency (document_id, word_id, frequency) VALUES (" + std::to_string(doc_id) + ", " + std::to_string(word_id) + ", " + std::to_string(frequency) + ");");
    }
    txn.commit();
}
