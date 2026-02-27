#include "customer.h"
#include "../../db/db_connection.h"

#include <algorithm>
#include <cctype>
#include <regex>

// Small Utility Helpers
namespace
{
    // Trims whitespace from both ends.
    std::string trim(const std::string &s)
    {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
            start++;

        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
            end--;

        return s.substr(start, end - start);
    }

    // Uppercase helper.
    std::string toUpper(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c)
                       { return static_cast<char>(std::toupper(c)); });
        return s;
    }

    // Backend validation patterns.
    // Required driving licence format: ON-12345678 (ON- + 8 digits)
    const std::regex LICENCE_REGEX(R"(^ON-\d{8}$)");

    // Required phone format: 10 digits (no separators)
    const std::regex PHONE_REGEX(R"(^\d{10}$)");

    // Simple email format validation
    const std::regex EMAIL_REGEX(R"(^[^\s@]+@[^\s@]+\.[^\s@]+$)");

    // Validation helpers.
    bool isValidLicence(const std::string &licence)
    {
        return std::regex_match(licence, LICENCE_REGEX);
    }

    bool isValidPhone(const std::string &phone)
    {
        return std::regex_match(phone, PHONE_REGEX);
    }

    bool isValidEmail(const std::string &email)
    {
        return std::regex_match(email, EMAIL_REGEX);
    }

    // Converts DB row to Customer struct.
    Customer rowToCustomer(const pqxx::row &row)
    {
        Customer c;
        c.id = row["id"].as<std::string>();
        c.first_name = row["first_name"].as<std::string>();
        c.last_name = row["last_name"].as<std::string>();

        // DB can store NULL for address.
        if (row["address"].is_null())
            c.address = "";
        else
            c.address = row["address"].as<std::string>();

        c.ph_number = row["ph_number"].as<std::string>();
        c.email = row["email"].as<std::string>();
        c.driving_licence = row["driving_licence"].as<std::string>();
        return c;
    }

    // Creates a consistent JSON representation for the frontend.
    crow::json::wvalue customerToJson(const Customer &c)
    {
        crow::json::wvalue item;
        item["id"] = c.id;
        item["first_name"] = c.first_name;
        item["last_name"] = c.last_name;

        //send address as null if it's empty.
        if (c.address.empty())
            item["address"] = nullptr;
        else
            item["address"] = c.address;

        item["ph_number"] = c.ph_number;
        item["email"] = c.email;
        item["driving_licence"] = c.driving_licence;
        return item;
    }

    // Uniform JSON error payload.
    crow::response jsonError(int code, const std::string &msg)
    {
        crow::json::wvalue out;
        out["error"] = msg;

        // respond with JSON error message and appropriate HTTP status code.
        crow::response res;
        res.code = code;
        res.set_header("Content-Type", "application/json");
        res.write(out.dump());
        return res;
    }

    // Helper: treat explicit JSON null as "null".
    bool isJsonNull(const crow::json::rvalue &v)
    {
        return v.t() == crow::json::type::Null;
    }

} // namespace

// DB Functions

// Fetches all customers, ordered by first name then last name.
std::vector<Customer> getAllCustomers(pqxx::connection &conn)
{
    pqxx::work txn(conn);

    // Keep ordering stable for UI table.
    pqxx::result r = txn.exec(R"(
        SELECT id, first_name, last_name, address, ph_number, email, driving_licence
        FROM Customers
        ORDER BY first_name ASC, last_name ASC
    )");

    txn.commit();

    std::vector<Customer> customers;
    customers.reserve(r.size());
    for (const auto &row : r)
        customers.push_back(rowToCustomer(row));

    return customers;
}

// Fetches a single customer by ID. Returns std::nullopt if not found.
std::optional<Customer> getCustomerById(pqxx::connection &conn, const std::string &customer_id)
{
    pqxx::work txn(conn);

    pqxx::result r = txn.exec_params(R"(
        SELECT id, first_name, last_name, address, ph_number, email, driving_licence
        FROM Customers
        WHERE id = $1
    )",
                                     customer_id);

    txn.commit();

    if (r.empty())
        return std::nullopt;

    return rowToCustomer(r[0]);
}

// Creates a new customer and returns the created record with ID.
Customer createCustomer(pqxx::connection &conn,
                        const std::string &first_name,
                        const std::string &last_name,
                        const std::string &ph_number,
                        const std::string &email,
                        const std::string &driving_licence,
                        const std::optional<std::string> &address)
{
    pqxx::work txn(conn);

    // Normalize and validate required fields.
    const std::string fn = trim(first_name);
    const std::string ln = trim(last_name);
    const std::string phone = trim(ph_number);
    const std::string mail = trim(email);
    const std::string licence = toUpper(trim(driving_licence));

    // Basic presence check.
    if (fn.empty() || ln.empty() || phone.empty() || mail.empty() || licence.empty())
        throw std::runtime_error("Missing required fields.");

    // Format validations.
    if (!isValidPhone(phone))
        throw std::runtime_error("Invalid phone number (must be 10 digits).");

    if (!isValidEmail(mail))
        throw std::runtime_error("Invalid email format.");

    if (!isValidLicence(licence))
        throw std::runtime_error("Invalid driving licence format (ON-12345678).");

    // Address optional.
    std::optional<std::string> addrNorm;
    if (address.has_value())
    {
        std::string a = trim(*address);
        if (!a.empty())
            addrNorm = a;
    }

    pqxx::result r = txn.exec_params(R"(
        INSERT INTO Customers (first_name, last_name, ph_number, email, driving_licence, address)
        VALUES ($1, $2, $3, $4, $5, $6)
        RETURNING id, first_name, last_name, address, ph_number, email, driving_licence
    )",
                                     fn,
                                     ln,
                                     phone,
                                     mail,
                                     licence,
                                     (addrNorm.has_value() ? addrNorm->c_str() : nullptr));

    txn.commit();
    return rowToCustomer(r[0]);
}

// Performs a partial update on a customer. Only provided fields are updated.
Customer patchCustomer(pqxx::connection &conn,
                       const std::string &customer_id,
                       const std::optional<std::string> &first_name,
                       const std::optional<std::string> &last_name,
                       const std::optional<std::string> &ph_number,
                       const std::optional<std::string> &email,
                       const std::optional<std::string> &driving_licence,
                       const std::optional<std::string> &address)
{
    pqxx::work txn(conn);

    // Normalizes inputs if provided.
    std::optional<std::string> fn, ln, phone, mail, licence, addr;

    if (first_name.has_value())
    {
        fn = trim(*first_name);
        if (fn->empty())
            throw std::runtime_error("first_name cannot be empty.");
    }

    if (last_name.has_value())
    {
        ln = trim(*last_name);
        if (ln->empty())
            throw std::runtime_error("last_name cannot be empty.");
    }

    if (ph_number.has_value())
    {
        phone = trim(*ph_number);
        if (!isValidPhone(*phone))
            throw std::runtime_error("Invalid phone number (must be 10 digits).");
    }

    if (email.has_value())
    {
        mail = trim(*email);
        if (!isValidEmail(*mail))
            throw std::runtime_error("Invalid email format.");
    }

    if (driving_licence.has_value())
    {
        licence = toUpper(trim(*driving_licence));
        if (!isValidLicence(*licence))
            throw std::runtime_error("Invalid driving licence format (ON-12345678).");
    }

    if (address.has_value())
        addr = trim(*address); // can be empty string, which means "clear the address"

    pqxx::result r = txn.exec_params(R"(
        UPDATE Customers
        SET
            first_name      = COALESCE($1, first_name),
            last_name       = COALESCE($2, last_name),
            ph_number       = COALESCE($3, ph_number),
            email           = COALESCE($4, email),
            driving_licence = COALESCE($5, driving_licence),
            address = CASE
                        WHEN $6 IS NULL THEN address
                        WHEN $6 = '' THEN NULL
                        ELSE $6
                      END
        WHERE id = $7
        RETURNING id, first_name, last_name, address, ph_number, email, driving_licence
    )",
                                     (fn.has_value() ? fn->c_str() : nullptr),
                                     (ln.has_value() ? ln->c_str() : nullptr),
                                     (phone.has_value() ? phone->c_str() : nullptr),
                                     (mail.has_value() ? mail->c_str() : nullptr),
                                     (licence.has_value() ? licence->c_str() : nullptr),
                                     (addr.has_value() ? addr->c_str() : nullptr),
                                     customer_id);

    txn.commit();

    if (r.empty())
        throw std::runtime_error("Customer not found.");

    return rowToCustomer(r[0]);
}

// Routes

void registerCustomerRoutes(crow::SimpleApp &app)
{
    // GET /customers
    // Fetches all customers. Returns empty array if none.
    CROW_ROUTE(app, "/customers").methods("GET"_method)([]()
                                                        {
        try
        {
            auto connPtr = getDbConnection();
            pqxx::connection &conn = *connPtr;

            auto customers = getAllCustomers(conn);

            // Builds JSON array safely.
            crow::json::wvalue::list outList;
            outList.reserve(customers.size());
            for (const auto &c : customers)
                outList.push_back(customerToJson(c));

            crow::json::wvalue out(outList);

            crow::response res;
            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.write(out.dump());
            return res;
        }
        catch (const std::exception &e)
        {
            return jsonError(500, std::string("Failed to fetch customers: ") + e.what());
        } });

    // GET /customers/<id>
    // Fetches a single customer by ID. Returns 404 if not found.
    CROW_ROUTE(app, "/customers/<string>").methods("GET"_method)([](const std::string &id)
                                                                 {
        try
        {
            auto connPtr = getDbConnection();
            pqxx::connection &conn = *connPtr;

            auto customerOpt = getCustomerById(conn, id);
            if (!customerOpt.has_value())
                return jsonError(404, "Customer not found");

            crow::json::wvalue out = customerToJson(*customerOpt);

            crow::response res;
            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.write(out.dump());
            return res;
        }
        catch (const std::exception &e)
        {
            return jsonError(500, std::string("Failed to fetch customer: ") + e.what());
        } });

    // POST /customers
    // Creates a new customer. All fields except address are required.
    CROW_ROUTE(app, "/customers").methods("POST"_method)([](const crow::request &req)
                                                         {
        auto body = crow::json::load(req.body);
        if (!body)
            return jsonError(400, "Invalid JSON body.");

        // Required fields.
        if (!body.has("first_name") || !body.has("last_name") ||
            !body.has("ph_number") || !body.has("email") ||
            !body.has("driving_licence"))
        {
            return jsonError(400, "Missing required fields.");
        }

        // Optional address.
        std::optional<std::string> addressOpt;
        if (body.has("address") && !isJsonNull(body["address"]))
        {
            std::string a = std::string(body["address"].s());
            a = trim(a);
            if (!a.empty())
                addressOpt = a;
        }

        try
        {
            auto connPtr = getDbConnection();
            pqxx::connection &conn = *connPtr;

            Customer created = createCustomer(conn,
                                              std::string(body["first_name"].s()),
                                              std::string(body["last_name"].s()),
                                              std::string(body["ph_number"].s()),
                                              std::string(body["email"].s()),
                                              std::string(body["driving_licence"].s()),
                                              addressOpt);

            crow::json::wvalue out = customerToJson(created);

            crow::response res;
            res.code = 201;
            res.set_header("Content-Type", "application/json");
            res.write(out.dump());
            return res;
        }
        catch (const std::exception &e)
        {
            // Most failures here are input validation, so 400 is appropriate.
            return jsonError(400, e.what());
        } });

    // PATCH /customers/<id>
    // This route supports partial updates.
    CROW_ROUTE(app, "/customers/<string>").methods("PATCH"_method)([](const crow::request &req, const std::string &id)
                                                                   {
        auto body = crow::json::load(req.body);
        if (!body)
            return jsonError(400, "Invalid JSON body.");

        // Helper to read optional string field safely.
        auto readOptStr = [&](const char *key) -> std::optional<std::string>
        {
            if (!body.has(key))
                return std::nullopt;

            // If explicit null was sent, treats as "field not provided"
            if (isJsonNull(body[key]))
                return std::nullopt;

            return std::string(body[key].s());
        };

        std::optional<std::string> fn = readOptStr("first_name");
        std::optional<std::string> ln = readOptStr("last_name");
        std::optional<std::string> phone = readOptStr("ph_number");
        std::optional<std::string> mail = readOptStr("email");
        std::optional<std::string> licence = readOptStr("driving_licence");

        // Address is special: explicit null means "clear the address", while absence means "keep existing".
        std::optional<std::string> addr = std::nullopt;
        if (body.has("address"))
        {
            if (isJsonNull(body["address"]))
                addr = std::string(""); // clear
            else
                addr = std::string(body["address"].s());
        }

        try
        {
            auto connPtr = getDbConnection();
            pqxx::connection &conn = *connPtr;

            Customer updated = patchCustomer(conn, id, fn, ln, phone, mail, licence, addr);

            crow::json::wvalue out = customerToJson(updated);

            crow::response res;
            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.write(out.dump());
            return res;
        }
        catch (const std::exception &e)
        {
            return jsonError(400, e.what());
        } });
}