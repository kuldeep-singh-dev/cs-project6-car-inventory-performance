#include "db_connection.h"
#include <iostream>

std::shared_ptr<pqxx::connection> getDbConnection() {
    static std::shared_ptr<pqxx::connection> conn = nullptr;

    if (!conn || !conn->is_open()) {
        try {
            conn = std::make_shared<pqxx::connection>(
                "host=db "
                "port=5432 "
                "dbname=dealerdrive "
                "user=dealerdrive "
                "password=dealerdrive"
            );

            std::cout << "DB connected successfully\n";
        } catch (const std::exception& e) {
            std::cerr << "DB connection failed: " << e.what() << std::endl;
            throw;
        }
    }
    return conn;
}
