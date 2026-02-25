#include "../../external/crow/crow_all.h"
#include <pqxx/pqxx>
#include <optional>
#include <string>
#include <vector>

/*
    Customer struct maps to a row in the Customers table.
*/
struct Customer
{
    std::string id;
    std::string first_name;
    std::string last_name;
    std::string address; // empty string represents no address; DB NULL is also treated as empty
    std::string ph_number;
    std::string email;
    std::string driving_licence;
};

/*
    Registers customer routes into the Crow app.
    sets up the API endpoints.
    */
void registerCustomerRoutes(crow::SimpleApp &app);

// DB helper functions

//Returns a vector of all customers, ordered by first name then last name
std::vector<Customer> getAllCustomers(pqxx::connection &conn);

// Returns a single customer by ID, or nullopt if not found
std::optional<Customer> getCustomerById(pqxx::connection &conn, const std::string &customer_id);

Customer createCustomer(pqxx::connection &conn,
                        const std::string &first_name,
                        const std::string &last_name,
                        const std::string &ph_number,
                        const std::string &email,
                        const std::string &driving_licence,
                        const std::optional<std::string> &address);

// Applies partial updates to a customer; returns the updated customer or throws if not found
Customer patchCustomer(pqxx::connection &conn,
                       const std::string &customer_id,
                       const std::optional<std::string> &first_name,
                       const std::optional<std::string> &last_name,
                       const std::optional<std::string> &ph_number,
                       const std::optional<std::string> &email,
                       const std::optional<std::string> &driving_licence,
                       const std::optional<std::string> &address);
