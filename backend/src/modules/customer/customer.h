#pragma once

#include "../../external/crow/crow_all.h"
#include <pqxx/pqxx>

#include <optional>
#include <string>
#include <vector>

struct Customer
{
    std::string id;
    std::string first_name;
    std::string last_name;
    std::string address;
    std::string ph_number;
    std::string email;
    std::string driving_licence;
};

// Register all Customer routes into the Crow application.
void registerCustomerRoutes(crow::SimpleApp &app);

// Helper Functions
std::vector<Customer> getAllCustomers(pqxx::connection &conn);
std::optional<Customer> getCustomerById(pqxx::connection &conn, const std::string &customer_id);
Customer createCustomer(
    pqxx::connection &conn,
    const std::string &first_name,
    const std::string &last_name,
    const std::string &ph_number,
    const std::string &email,
    const std::string &driving_licence,
    const std::optional<std::string> &address);

Customer patchCustomer(
    pqxx::connection &conn,
    const std::string &customer_id,
    const std::optional<std::string> &first_name,
    const std::optional<std::string> &last_name,
    const std::optional<std::string> &ph_number,
    const std::optional<std::string> &email,
    const std::optional<std::string> &driving_licence,
    const std::optional<std::string> &address

);
