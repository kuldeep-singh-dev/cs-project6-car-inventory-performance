// #pragma once
// #include <pqxx/pqxx>
// #include <memory>

// // Returns a shared database connection
// std::shared_ptr<pqxx::connection> getDbConnection();

#pragma once
#include <pqxx/pqxx>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

class ConnectionPool {
    std::vector<std::shared_ptr<pqxx::connection>> pool;
    std::queue<std::shared_ptr<pqxx::connection>> available;
    std::mutex mtx;
    std::condition_variable cv;

public:
    ConnectionPool(int size, const std::string& connStr);
    std::shared_ptr<pqxx::connection> acquire();
    void release(std::shared_ptr<pqxx::connection> conn);
};

// RAII guard - auto releases connection when it goes out of scope
class ConnectionGuard {
    ConnectionPool& pool;
    std::shared_ptr<pqxx::connection> conn;
public:
    ConnectionGuard(ConnectionPool& p) : pool(p), conn(p.acquire()) {}
    ~ConnectionGuard() { pool.release(conn); }
    pqxx::connection& get() { return *conn; }
};

// Global pool accessor
ConnectionPool& getPool();