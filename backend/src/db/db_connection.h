#pragma once
#include <pqxx/pqxx>
#include <memory>

// Returns a shared database connection
std::shared_ptr<pqxx::connection> getDbConnection();
