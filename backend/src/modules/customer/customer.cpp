#include "customer.h"
#include "../../db/db_connection.h"
#include <pqxx/pqxx>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace
{
    void addCorsHeaders(crow::response &res)
    {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }

    bool isLikelyUuid(const std::string &s)
    {
        if (s.size() != 36) return false;
        int hyphens = 0;
        for (char c : s) { if (c == '-') hyphens++; }
        return hyphens == 4;
    }

    bool isValidEmail(const std::string &email)
    {
        static const std::regex re(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
        return std::regex_match(email, re);
    }

    Customer rowToCustomer(const pqxx::row &row)
    {
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
        crow::json::wvalue err;
        err["error"] = message;
        crow::response res(code, err);
        addCorsHeaders(res);
        return res;
    }
}

std::vector<Customer> getAllCustomers(ConnectionGuard &guard)
{
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec(
        "SELECT id, first_name, last_name, address, ph_number, email, driving_licence "
        "FROM customers "
        "ORDER BY last_name ASC, first_name ASC");
    std::vector<Customer> out;
    out.reserve(r.size());
    for (const auto &row : r)
        out.push_back(rowToCustomer(row));
    txn.commit();
    return out;
}

std::optional<Customer> getCustomerById(ConnectionGuard &guard, const std::string &customer_id)
{
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec_params(
        "SELECT id, first_name, last_name, address, ph_number, email, driving_licence "
        "FROM customers WHERE id = $1",
        customer_id);
    txn.commit();
    if (r.empty()) return std::nullopt;
    return rowToCustomer(r[0]);
}

Customer createCustomer(
    ConnectionGuard &guard,
    const std::string &first_name,
    const std::string &last_name,
    const std::string &ph_number,
    const std::string &email,
    const std::string &driving_licence,
    const std::optional<std::string> &address)
{
    if (first_name.empty() || last_name.empty() || ph_number.empty() || email.empty() || driving_licence.empty())
        throw std::invalid_argument("Missing required fields");
    if (!isValidEmail(email))
        throw std::invalid_argument("Invalid email format");

    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec_params(
        "INSERT INTO customers (first_name, last_name, address, ph_number, email, driving_licence) "
        "VALUES ($1, $2, $3, $4, $5, $6) "
        "RETURNING id, first_name, last_name, address, ph_number, email, driving_licence",
        first_name, last_name,
        address.has_value() ? *address : std::string(""),
        ph_number, email, driving_licence);
    txn.commit();
    return rowToCustomer(r[0]);
}

Customer patchCustomer(
    ConnectionGuard &guard,
    const std::string &customer_id,
    const std::optional<std::string> &first_name,
    const std::optional<std::string> &last_name,
    const std::optional<std::string> &ph_number,
    const std::optional<std::string> &email,
    const std::optional<std::string> &driving_licence,
    const std::optional<std::string> &address)
{
    if (!isLikelyUuid(customer_id))
        throw std::invalid_argument("Invalid customer id");
    if (email.has_value() && !email->empty() && !isValidEmail(*email))
        throw std::invalid_argument("Invalid email format");
    if (!first_name && !last_name && !ph_number && !email && !driving_licence && !address)
        throw std::invalid_argument("No fields provided for update");

    // Reuse same guard - same connection, no second acquire
    auto existingOpt = getCustomerById(guard, customer_id);
    if (!existingOpt) throw std::runtime_error("NOT_FOUND");
    Customer existing = *existingOpt;

    const std::string new_first   = first_name       ? *first_name       : existing.first_name;
    const std::string new_last    = last_name         ? *last_name        : existing.last_name;
    const std::string new_phone   = ph_number         ? *ph_number        : existing.ph_number;
    const std::string new_email   = email             ? *email            : existing.email;
    const std::string new_dl      = driving_licence   ? *driving_licence  : existing.driving_licence;
    const std::string new_address = address           ? *address          : existing.address;

    if (!new_email.empty() && !isValidEmail(new_email))
        throw std::invalid_argument("Invalid email format");

    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec_params(
        "UPDATE customers SET first_name=$1, last_name=$2, address=$3, "
        "ph_number=$4, email=$5, driving_licence=$6 "
        "WHERE id=$7 "
        "RETURNING id, first_name, last_name, address, ph_number, email, driving_licence",
        new_first, new_last, new_address, new_phone, new_email, new_dl, customer_id);
    txn.commit();

    if (r.empty()) throw std::runtime_error("NOT_FOUND");
    return rowToCustomer(r[0]);
}

static std::string getStringOrEmpty(const crow::json::rvalue &body, const char *key)
{
    return body.has(key) ? std::string(body[key].s()) : std::string();
}

void registerCustomerRoutes(crow::SimpleApp &app)
{
    // OPTIONS /customers
    CROW_ROUTE(app, "/customers")
        .methods("OPTIONS"_method)([]() {
            crow::response res;
            res.code = 204;
            addCorsHeaders(res);
            return res;
        });

    // OPTIONS /customers/{id}
    CROW_ROUTE(app, "/customers/<string>")
        .methods("OPTIONS"_method)([](const std::string &) {
            crow::response res;
            res.code = 204;
            addCorsHeaders(res);
            return res;
        });

    // GET /customers
    CROW_ROUTE(app, "/customers")
        .methods("GET"_method)([]() {
            try {
                ConnectionGuard guard(getPool());
                auto customers = getAllCustomers(guard);
                crow::json::wvalue result = crow::json::wvalue::list();
                int i = 0;
                for (const auto &c : customers)
                    result[i++] = customerToJson(c);
                crow::response res(200, result);
                addCorsHeaders(res);
                return res;
            } catch (const std::exception &e) {
                return jsonError(500, e.what());
            }
        });

    // GET /customers/{id}
    CROW_ROUTE(app, "/customers/<string>")
        .methods("GET"_method)([](const std::string &id) {
            if (!isLikelyUuid(id))
                return jsonError(400, "Invalid customer id");
            try {
                ConnectionGuard guard(getPool());
                auto c = getCustomerById(guard, id);
                if (!c) return jsonError(404, "Customer not found");
                crow::response res(200, customerToJson(*c));
                addCorsHeaders(res);
                return res;
            } catch (const std::exception &e) {
                return jsonError(500, e.what());
            }
        });

    // POST /customers
    CROW_ROUTE(app, "/customers")
        .methods("POST"_method)([](const crow::request &req) {
            auto body = crow::json::load(req.body);
            if (!body) return jsonError(400, "Invalid JSON payload");

            const std::string first_name      = getStringOrEmpty(body, "first_name");
            const std::string last_name       = getStringOrEmpty(body, "last_name");
            const std::string address         = getStringOrEmpty(body, "address");
            const std::string ph_number       = getStringOrEmpty(body, "ph_number");
            const std::string email           = getStringOrEmpty(body, "email");
            const std::string driving_licence = getStringOrEmpty(body, "driving_licence");

            try {
                ConnectionGuard guard(getPool());
                Customer created = createCustomer(
                    guard, first_name, last_name, ph_number, email, driving_licence,
                    address.empty() ? std::nullopt : std::optional<std::string>(address));
                crow::response res(201, customerToJson(created));
                addCorsHeaders(res);
                return res;
            } catch (const pqxx::unique_violation &) {
                return jsonError(409, "Customer with the same email or driving_licence already exists");
            } catch (const std::invalid_argument &e) {
                return jsonError(400, e.what());
            } catch (const std::exception &e) {
                return jsonError(500, e.what());
            }
        });

    // PATCH /customers/{id}
    CROW_ROUTE(app, "/customers/<string>")
        .methods("PATCH"_method)([](const crow::request &req, const std::string &id) {
            auto body = crow::json::load(req.body);
            if (!body) return jsonError(400, "Invalid JSON payload");

            auto optStr = [&](const char *key) -> std::optional<std::string> {
                if (!body.has(key)) return std::nullopt;
                return std::optional<std::string>(std::string(body[key].s()));
            };

            try {
                ConnectionGuard guard(getPool());
                Customer updated = patchCustomer(
                    guard, id,
                    optStr("first_name"), optStr("last_name"), optStr("ph_number"),
                    optStr("email"), optStr("driving_licence"), optStr("address"));
                crow::response res(200, customerToJson(updated));
                addCorsHeaders(res);
                return res;
            } catch (const pqxx::unique_violation &) {
                return jsonError(409, "Update violates uniqueness constraint");
            } catch (const std::invalid_argument &e) {
                return jsonError(400, e.what());
            } catch (const std::runtime_error &e) {
                if (std::string(e.what()) == "NOT_FOUND")
                    return jsonError(404, "Customer not found");
                return jsonError(500, e.what());
            } catch (const std::exception &e) {
                return jsonError(500, e.what());
            }
        });
}