#include "customer.h"

#include "../../db/db_connection.h"

#include <pqxx/pqxx>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace
{

    bool isLikelyUuid(const std::string &s)
    {
        // Check for customer id format
        if (s.size() != 36)
            return false;
        int hyphens = 0;
        for (char c : s)
        {
            if (c == '-')
                hyphens++;
        }
        return hyphens == 4;
    }

    bool isValidEmail(const std::string &email)
    {
        // Email Validation
        static const std::regex re(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
        return std::regex_match(email, re);
    }

    Customer rowToCustomer(const pqxx::row &row)
    {
        // Converts a database row to a Customer struct
        Customer c;
        c.id = row["id"].c_str();
        c.first_name = row["first_name"].c_str();
        c.last_name = row["last_name"].c_str();
        c.address = row["address"].is_null() ? "" : row["address"].c_str();
        c.ph_number = row["ph_number"].c_str();
        c.email = row["email"].c_str();
        c.driving_licence = row["driving_licence"].c_str();
        return c;
    }

    crow::json::wvalue customerToJson(const Customer &c)
    {
        // Converts a Customer struct to a JSON object
        crow::json::wvalue j;
        j["id"] = c.id;
        j["first_name"] = c.first_name;
        j["last_name"] = c.last_name;
        j["address"] = c.address;
        j["ph_number"] = c.ph_number;
        j["email"] = c.email;
        j["driving_licence"] = c.driving_licence;
        return j;
    }

    crow::response jsonError(int code, const std::string &message)
    {
        // Helper to return a JSON error response with a consistent structure
        crow::json::wvalue err;
        err["error"] = message;
        return crow::response(code, err);
    }

}

std::vector<Customer> getAllCustomers(pqxx::connection &conn)
{
    // Fetches all customers from the database, ordered by last name then first name
    pqxx::work txn(conn);
    pqxx::result r = txn.exec(
        "SELECT id, first_name, last_name, address, ph_number, email, driving_licence "
        "FROM customers "
        "ORDER BY last_name ASC, first_name ASC");
    std::vector<Customer> out;
    out.reserve(r.size());
    for (const auto &row : r)
    {
        out.push_back(rowToCustomer(row));
    }
    txn.commit();
    return out;
}

std::optional<Customer> getCustomerById(pqxx::connection &conn, const std::string &customer_id)
{
    // Fetches a single customer by ID.
    pqxx::work txn(conn);
    pqxx::result r = txn.exec_params(
        "SELECT id, first_name, last_name, address, ph_number, email, driving_licence "
        "FROM customers WHERE id = $1",
        customer_id);

    txn.commit();
    if (r.empty())
        // No customer found with the given ID
        return std::nullopt;
    return rowToCustomer(r[0]);
}

Customer createCustomer(
    pqxx::connection &conn,
    const std::string &first_name,
    const std::string &last_name,
    const std::string &ph_number,
    const std::string &email,
    const std::string &driving_licence,
    const std::optional<std::string> &address)
{
    if (first_name.empty() || last_name.empty() || ph_number.empty() || email.empty() || driving_licence.empty())
    {
        throw std::invalid_argument("Missing required fields");
    }
    if (!isValidEmail(email))
    {
        throw std::invalid_argument("Invalid email format");
    }

    pqxx::work txn(conn);
    pqxx::result r = txn.exec_params(
        "INSERT INTO customers (first_name, last_name, address, ph_number, email, driving_licence) "
        "VALUES ($1, $2, $3, $4, $5, $6) "
        "RETURNING id, first_name, last_name, address, ph_number, email, driving_licence",
        first_name,
        last_name,
        address.has_value() ? *address : std::string(""),
        ph_number,
        email,
        driving_licence);
    txn.commit();
    return rowToCustomer(r[0]);
}

Customer patchCustomer(
    pqxx::connection &conn,
    const std::string &customer_id,
    const std::optional<std::string> &first_name,
    const std::optional<std::string> &last_name,
    const std::optional<std::string> &ph_number,
    const std::optional<std::string> &email,
    const std::optional<std::string> &driving_licence,
    const std::optional<std::string> &address)
{
    if (!isLikelyUuid(customer_id))
    {
        throw std::invalid_argument("Invalid customer id");
    }

    if (email.has_value() && !email->empty() && !isValidEmail(*email))
    {
        throw std::invalid_argument("Invalid email format");
    }

    // Ensures at least one field was provided.
    if (!first_name && !last_name && !ph_number && !email && !driving_licence && !address)
    {
        throw std::invalid_argument("No fields provided for update");
    }

    // Fetches existing record, then apply a fixed update.
    auto existingOpt = getCustomerById(conn, customer_id);
    if (!existingOpt)
    {
        throw std::runtime_error("NOT_FOUND");
    }
    Customer existing = *existingOpt;

    const std::string new_first = first_name ? *first_name : existing.first_name;
    const std::string new_last = last_name ? *last_name : existing.last_name;
    const std::string new_phone = ph_number ? *ph_number : existing.ph_number;
    const std::string new_email = email ? *email : existing.email;
    const std::string new_dl = driving_licence ? *driving_licence : existing.driving_licence;
    const std::string new_address = address ? *address : existing.address;

    if (!new_email.empty() && !isValidEmail(new_email))
    {
        throw std::invalid_argument("Invalid email format");
    }

    pqxx::work txn(conn);
    pqxx::result r = txn.exec_params(
        "UPDATE customers SET first_name = $1, last_name = $2, address = $3, ph_number = $4, email = $5, driving_licence = $6 "
        "WHERE id = $7 "
        "RETURNING id, first_name, last_name, address, ph_number, email, driving_licence",
        new_first,
        new_last,
        new_address,
        new_phone,
        new_email,
        new_dl,
        customer_id);
    txn.commit();

    if (r.empty())
    {
        throw std::runtime_error("NOT_FOUND");
    }
    return rowToCustomer(r[0]);
}

static std::string getStringOrEmpty(const crow::json::rvalue &body, const char *key)
{
    return body.has(key) ? std::string(body[key].s()) : std::string();
}

// Routes
void registerCustomerRoutes(crow::SimpleApp &app)
{

    // GET /customers
    CROW_ROUTE(app, "/customers")
        .methods("GET"_method)([]()
                               {
        try {
            auto connPtr = getDbConnection();
            auto customers = getAllCustomers(*connPtr);
            crow::json::wvalue result = crow::json::wvalue::list();
            int i = 0;
            for (const auto& c : customers) {
                result[i++] = customerToJson(c);
            }
            return crow::response(200, result);
        } catch (const std::exception& e) {
            return jsonError(500, e.what());
        } });

    // GET /customers/{id}
    CROW_ROUTE(app, "/customers/<string>")
        .methods("GET"_method)([](const std::string &id)
                               {
        if (!isLikelyUuid(id)) {
            return jsonError(400, "Invalid customer id");
        }
        try {
            auto connPtr = getDbConnection();
            auto c = getCustomerById(*connPtr, id);
            if (!c) {
                return jsonError(404, "Customer not found");
            }
            return crow::response(200, customerToJson(*c));
        } catch (const std::exception& e) {
            return jsonError(500, e.what());
        } });

    // POST /customers
    CROW_ROUTE(app, "/customers")
        .methods("POST"_method)([](const crow::request &req)
                                {
        auto body = crow::json::load(req.body);
        if (!body) {
            return jsonError(400, "Invalid JSON payload");
        }
        
        const std::string first_name = getStringOrEmpty(body, "first_name");
        const std::string last_name = getStringOrEmpty(body, "last_name");
        const std::string address = getStringOrEmpty(body, "address");
        const std::string ph_number = getStringOrEmpty(body, "ph_number");
        const std::string email = getStringOrEmpty(body, "email");
        const std::string driving_licence = getStringOrEmpty(body, "driving_licence");

        try {
            auto connPtr = getDbConnection();
            Customer created = createCustomer(
                *connPtr,
                first_name,
                last_name,
                ph_number,
                email,
                driving_licence,
                address.empty() ? std::nullopt : std::optional<std::string>(address)
            );
            return crow::response(201, customerToJson(created));
        } catch (const pqxx::unique_violation&) {
            return jsonError(409, "Customer with the same email or driving_licence already exists");
        } catch (const std::invalid_argument& e) {
            return jsonError(400, e.what());
        } catch (const std::exception& e) {
            return jsonError(500, e.what());
        } });

    // PATCH /customers/{id}
    CROW_ROUTE(app, "/customers/<string>")
        .methods("PATCH"_method)([](const crow::request &req, const std::string &id)
                                 {
        auto body = crow::json::load(req.body);
        if (!body) {
            return jsonError(400, "Invalid JSON payload");
        }

        auto optStr = [&](const char* key) -> std::optional<std::string> {
            if (!body.has(key)) return std::nullopt;
            return std::optional<std::string>(std::string(body[key].s()));
        };

        const auto first_name = optStr("first_name");
        const auto last_name = optStr("last_name");
        const auto ph_number = optStr("ph_number");
        const auto email = optStr("email");
        const auto driving_licence = optStr("driving_licence");
        const auto address = optStr("address");

        try {
            auto connPtr = getDbConnection();
            Customer updated = patchCustomer(
                *connPtr,
                id,
                first_name,
                last_name,
                ph_number,
                email,
                driving_licence,
                address
            );
            return crow::response(200, customerToJson(updated));
        } catch (const pqxx::unique_violation&) {
            return jsonError(409, "Update violates uniqueness constraint (email or driving_licence already exists)");
        } catch (const std::invalid_argument& e) {
            return jsonError(400, e.what());
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "NOT_FOUND") {
                return jsonError(404, "Customer not found");
            }
            return jsonError(500, e.what());
        } catch (const std::exception& e) {
            return jsonError(500, e.what());
        } });
}