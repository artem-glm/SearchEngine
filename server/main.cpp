#include "server.h"
#include "../config/config.h"

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    Config conf = LoadConfig("../../../../config/config.ini");

    std::string db_conn_str = "dbname = " + conf.db_name +
        " user=" + conf.db_user +
        " password=" + conf.db_password +
        " host=" + conf.db_host +
        " port=" + std::to_string(conf.db_port);

    pqxx::connection db_conn(db_conn_str);

    std::cout << "Open in browser: http://localhost:" << conf.server_port << std::endl;
    start_server("0.0.0.0", conf.server_port, db_conn);
}
