// #include "db_connection.h"
// #include <iostream>
// #include <memory>
// #include <pqxx/pqxx>

// std::shared_ptr<pqxx::connection> getDbConnection() {
//     try {
//         thread_local std::shared_ptr<pqxx::connection> conn =
//         std::make_shared<pqxx::connection>(
//             "host=db "
//             "port=5432 "
//             "dbname=dealerdrive "
//             "user=dealerdrive "
//             "password=dealerdrive"
//         );

//         if (!conn->is_open()) {
//             throw std::runtime_error("Database connection is not open");
//         }

//         return conn;
//     } catch (const std::exception& e) {
//         std::cerr << "DB connection failed: " << e.what() << std::endl;
//         throw;
//     }
// }

#include "db_connection.h"
#include <iostream>

ConnectionPool::ConnectionPool(int size, const std::string& connStr) {
    for (int i = 0; i < size; i++) {
        auto conn = std::make_shared<pqxx::connection>(connStr);
        pool.push_back(conn);
        available.push(conn);
    }
}

std::shared_ptr<pqxx::connection> ConnectionPool::acquire() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]{ return !available.empty(); });
    auto conn = available.front();
    available.pop();
    return conn;
}

void ConnectionPool::release(std::shared_ptr<pqxx::connection> conn) {
    std::unique_lock<std::mutex> lock(mtx);
    available.push(conn);
    cv.notify_one();
}

ConnectionPool& getPool() {
    static ConnectionPool pool(20,
        "host=db port=5432 dbname=dealerdrive "
        "user=dealerdrive password=dealerdrive"
    );
    return pool;
}