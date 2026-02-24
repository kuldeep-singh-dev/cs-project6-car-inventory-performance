#pragma once

#include "../../external/crow/crow_all.h"
#include "../../db/db_connection.h"
#include <pqxx/pqxx>

#include <optional>
#include <string>
#include <vector>

/// @struct Customer
/// @brief Represents a customer entity.
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

/// @brief Registers all customer-related HTTP routes to the Crow application.
/// @param app The Crow application instance to register routes on.
void registerCustomerRoutes(crow::SimpleApp &app);

/// @brief Retrieves all customers from the database.
/// @param guard Reference to a ConnectionGuard for database access.
/// @return A vector of Customer objects.
std::vector<Customer> getAllCustomers(ConnectionGuard &guard);

/// @brief Retrieves a customer by their unique ID.
/// @param guard Reference to a ConnectionGuard for database access, customer_id The unique identifier of the customer.
/// @return An optional Customer object if found.
std::optional<Customer> getCustomerById(ConnectionGuard &guard, const std::string &customer_id);

/// @brief Creates a new customer in the database.
/// @param guard Reference to a ConnectionGuard for database access, first_name Customer's first name, last_name Customer's last name, ph_number Customer's phone number, email Customer's email address, driving_licence Customer's driving license number, address Optional customer address.
/// @return The created Customer object.
Customer createCustomer(
    ConnectionGuard &guard,
    const std::string &first_name,
    const std::string &last_name,
    const std::string &ph_number,
    const std::string &email,
    const std::string &driving_licence,
    const std::optional<sd::string> &address);

/// @brief Updates fields of an existing customer (PATCH semantics).
/// @param guard Reference to a ConnectionGuard for database access, customer_id The unique identifier of the customer, first_name Optional new first name, last_name Optional new last name, ph_number Optional new phone number, email Optional new email address, driving_licence Optional new driving license number, address Optional new address.
/// @return The updated Customer object.
Customer patchCustomer(
    ConnectionGuard &guard,
    const std::string &customer_id,
    const std::optional<std::string> &first_name,
    const std::optional<std::string> &last_name,
    const std::optional<std::string> &ph_number,
    const std::optional<std::string> &email,
    const std::optional<std::string> &driving_licence,
    const std::optional<std::string> &address
);
