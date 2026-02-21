#include "db_connection.h"
#include <iostream>
#include <memory>
#include <pqxx/pqxx>

std::shared_ptr<pqxx::connection> getDbConnection() {
    try {
        thread_local std::shared_ptr<pqxx::connection> conn =
        std::make_shared<pqxx::connection>(
            "host=db "
            "port=5432 "
            "dbname=dealerdrive "
            "user=dealerdrive "
            "password=dealerdrive"
        );

        if (!conn->is_open()) {
            throw std::runtime_error("Database connection is not open");
        }

        return conn;
    } catch (const std::exception& e) {
        std::cerr << "DB connection failed: " << e.what() << std::endl;
        throw;
    }
}
