#include "sales.h"
#include "../../db/db_connection.h"
#include <pqxx/pqxx>
#include <ctime>
#include <sstream>
#include <iomanip>

bool isValidDate(const std::string& date) {
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");

    if (ss.fail()) return false;

    tm.tm_isdst = -1;
    std::time_t t = std::mktime(&tm);
    if (t == -1) return false;

    std::tm check = *std::gmtime(&t);

    return check.tm_year == tm.tm_year &&
           check.tm_mon  == tm.tm_mon &&
           check.tm_mday == tm.tm_mday;
}

std::time_t toTimeT(const std::string& date) {
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    tm.tm_isdst = -1;
    return std::mktime(&tm);
}

std::string toDateString(std::time_t t) {
    std::tm tm = *std::gmtime(&t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return std::string(buf);
}

void registerSalesRoutes(crow::SimpleApp& app) {

    //------------------------------------------------------------------
    // GET /sales
    //------------------------------------------------------------------
    CROW_ROUTE(app, "/sales")
    .methods("GET"_method)
    ([]() {

        auto conn = getDbConnection();
        pqxx::work txn(*conn);

        pqxx::result r = txn.exec(
            "SELECT s.id AS sale_id, s.date, s.sale_price, "
            "v.make, v.model, "
            "c.first_name, c.last_name "
            "FROM Sales s "
            "JOIN Vehicles v ON s.vehicle_id = v.id "
            "JOIN Customers c ON s.customer_id = c.id "
            "ORDER BY s.date DESC"
        );

        crow::json::wvalue result = crow::json::wvalue::list();
        int i = 0;

        for (const auto& row : r) {
            result[i]["sale_id"] = row["sale_id"].c_str();
            result[i]["date"] = row["date"].c_str();
            result[i]["price"] = row["sale_price"].as<double>();
            result[i]["vehicle"] =
                std::string(row["make"].c_str()) + " " +
                row["model"].c_str();
            result[i]["customer"] =
                std::string(row["first_name"].c_str()) + " " +
                row["last_name"].c_str();
            ++i;
        }

        return result;
    });

    //------------------------------------------------------------------
    // GET /sales/weekly-report
    //------------------------------------------------------------------
    CROW_ROUTE(app, "/sales/weekly-report")
    .methods(crow::HTTPMethod::GET)
    ([](const crow::request& req) {

        try {
            const char* startParam = req.url_params.get("start");
            const char* endParam   = req.url_params.get("end");

            if (!startParam || !endParam) {
                crow::json::wvalue err;
                err["error"] = "start and end query parameters are required (YYYY-MM-DD)";
                return crow::response(400, err);
            }

            std::string startDate = startParam;
            std::string endDate   = endParam;

            // 1️⃣ Validate date format + calendar validity
            if (!isValidDate(startDate) || !isValidDate(endDate)) {
                return crow::response(
                    400,
                    "Invalid date format or invalid calendar date (YYYY-MM-DD)"
                );
            }

            std::time_t startTime = toTimeT(startDate);
            std::time_t endTime   = toTimeT(endDate);

            // 2️⃣ Logical validation
            if (startTime >= endTime) {
                return crow::response(400, "start date must be before end date");
            }

            // 3️⃣ Safety limit (10 years max)
            const int MAX_WEEKS = 520;
            int weekCounter = 0;

            auto conn = getDbConnection();
            pqxx::work txn(*conn);

            std::ostringstream csv;
            csv << "week_start,week_end,total_sales_count,total_revenue,"
                << "avg_sale_price,min_sale_price,max_sale_price,"
                << "total_market_value,total_profit,avg_profit_per_sale\n";

            std::time_t currentWeekStart = startTime;

            while (currentWeekStart < endTime) {

                if (++weekCounter > MAX_WEEKS) {
                    return crow::response(400, "Date range too large");
                }

                std::time_t currentWeekEnd =
                    currentWeekStart + 7 * 24 * 60 * 60;

                std::string weekStartStr = toDateString(currentWeekStart);
                std::string weekEndStr   = toDateString(currentWeekEnd);

                pqxx::result r = txn.exec_params(
                    R"(
                        SELECT
                            COUNT(*) AS total_sales_count,
                            COALESCE(SUM(s.sale_price), 0) AS total_revenue,
                            COALESCE(AVG(s.sale_price), 0) AS avg_sale_price,
                            COALESCE(MIN(s.sale_price), 0) AS min_sale_price,
                            COALESCE(MAX(s.sale_price), 0) AS max_sale_price,
                            COALESCE(SUM(v.market_price), 0) AS total_market_value,
                            COALESCE(SUM(s.sale_price - v.market_price), 0) AS total_profit,
                            COALESCE(AVG(s.sale_price - v.market_price), 0) AS avg_profit_per_sale
                        FROM Sales s
                        JOIN Vehicles v ON s.vehicle_id = v.id
                        WHERE s.date >= $1 AND s.date < $2
                    )",
                    weekStartStr,
                    weekEndStr
                );

                const auto& row = r[0];

                csv
                    << weekStartStr << ","
                    << toDateString(currentWeekStart + 6 * 24 * 60 * 60) << ","
                    << row["total_sales_count"].as<int>() << ","
                    << row["total_revenue"].as<double>() << ","
                    << row["avg_sale_price"].as<double>() << ","
                    << row["min_sale_price"].as<double>() << ","
                    << row["max_sale_price"].as<double>() << ","
                    << row["total_market_value"].as<double>() << ","
                    << row["total_profit"].as<double>() << ","
                    << row["avg_profit_per_sale"].as<double>() << "\n";

                currentWeekStart = currentWeekEnd;
            }

            txn.commit();

            crow::response res;
            res.code = 200;
            res.set_header("Content-Type", "text/csv");
            res.set_header(
                "Content-Disposition",
                "attachment; filename=weekly_sales_report.csv"
            );
            res.body = csv.str();
            return res;

        } catch (const std::exception& e) {
            return crow::response(500, e.what());
        }
    });

    //------------------------------------------------------------------
    // GET /sales/export/csv - Export all sales to CSV
    //------------------------------------------------------------------
    CROW_ROUTE(app, "/sales/export/csv")
    .methods("GET"_method)
    ([]() {
        try {
            auto conn = getDbConnection();
            pqxx::work txn(*conn);

            pqxx::result r = txn.exec(
                "SELECT "
                "s.id AS sale_id, "
                "s.date, "
                "s.sale_price, "
                "v.id AS vehicle_id, "
                "v.vin, "
                "v.make, "
                "v.model, "
                "v.year, "
                "v.trim, "
                "v.odometer, "
                "v.fuel_type, "
                "v.transmission, "
                "v.market_price, "
                "c.id AS customer_id, "
                "c.first_name, "
                "c.last_name, "
                "c.email, "
                "c.ph_number, "
                // Calculate profit
                "(s.sale_price - v.market_price) AS profit, "
                "ROUND(((s.sale_price - v.market_price) / v.market_price * 100), 2) AS profit_percentage "
                "FROM Sales s "
                "JOIN Vehicles v ON s.vehicle_id = v.id "
                "JOIN Customers c ON s.customer_id = c.id "
                "ORDER BY s.date DESC"
            );

            // Build CSV content
            std::stringstream csv;

            // CSV Header
            csv << "Date,Sale Price,Market Price,Profit compare to MRP,Profit %,"
                << "VIN,Make,Model,Year,Trim,Odometer,Fuel Type,Transmission,"
                << "First Name,Last Name,Email,Phone\n";

            // CSV Rows
            for (const auto& row : r) {
                csv << row["date"].c_str() << ","
                    << row["sale_price"].as<double>() << ","
                    << row["market_price"].as<double>() << ","
                    << row["profit"].as<double>() << ","
                    << row["profit_percentage"].as<double>() << ","
                    << row["vin"].c_str() << ","
                    << row["make"].c_str() << ","
                    << row["model"].c_str() << ","
                    << row["year"].c_str() << ","
                    << (row["trim"].is_null() ? "" : row["trim"].c_str()) << ","
                    << row["odometer"].as<int>() << ","
                    << row["fuel_type"].c_str() << ","
                    << row["transmission"].c_str() << ","
                    << row["first_name"].c_str() << ","
                    << row["last_name"].c_str() << ","
                    << row["email"].c_str() << ","
                    << row["ph_number"].c_str() << "\n";
            }

            // Create response with proper headers
            crow::response res(csv.str());
            res.set_header("Content-Type", "text/csv");
            res.set_header("Content-Disposition", "attachment; filename=sales_export.csv");
            
            return res;

        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Error in CSV export: " << e.what();
            
            crow::json::wvalue error;
            error["error"] = "Export failed";
            error["message"] = e.what();
            return crow::response(500, error);
        }
    });

    
    //------------------------------------------------------------------
    // GET /sales/<id> - Get invoice for a sale
    //------------------------------------------------------------------
    CROW_ROUTE(app, "/sales/id/<string>")
    .methods("GET"_method)
    ([](const std::string& id) {
        try {
            // Validate UUID format
            if (id.length() != 36) {
                crow::json::wvalue error;
                error["error"] = "Invalid UUID format";
                return crow::response(400, error);
            }

            auto conn = getDbConnection();
            pqxx::work txn(*conn);

            pqxx::result r = txn.exec_params(
                "SELECT "
                "  s.id AS sale_id, "
                "  s.date AS sale_date, "
                "  s.sale_price, "
                "  v.id AS vehicle_id, "
                "  v.make, "
                "  v.model, "
                "  v.year, "
                "  v.vin, "
                "  v.odometer, "
                "  v.fuel_type, "
                "  v.transmission, "
                "  v.trim, "
                "  v.market_price, "
                "  v.status, "
                "  c.id AS customer_id, "
                "  c.first_name, "
                "  c.last_name, "
                "  c.email, "
                "  c.ph_number, "
                "  c.address, "
                "  c.driving_licence "
                "FROM Sales s "
                "JOIN Vehicles v ON s.vehicle_id = v.id "
                "JOIN Customers c ON s.customer_id = c.id "
                "WHERE s.id = $1",
                id
            );

            if (r.empty()) {
                crow::json::wvalue error;
                error["error"] = "Sale not found";
                return crow::response(404, error);
            }

            auto row = r[0];

            // ========================================
            // BUILDING STRINGS FIRST 
            // ========================================
            
            std::string customer_name = std::string(row["first_name"].c_str()) + " " + 
                                    row["last_name"].c_str();
            
            std::string vehicle_description = std::string(row["year"].c_str()) + " " +
                                            row["make"].c_str() + " " +
                                            row["model"].c_str();
            
            // Add trim if exists
            if (!row["trim"].is_null()) {
                vehicle_description += " " + std::string(row["trim"].c_str());
            }

            // ========================================
            // CALCULATIONS
            // ========================================
            
            double base_price = row["sale_price"].as<double>();
            double sales_tax_rate = 0.13;
            double sales_tax = base_price * sales_tax_rate;
            double documentation_fee = 500.00;
            double registration_fee = 120.00;
            double delivery_fee = 0.00;
            
            double subtotal = base_price;
            double total_fees = documentation_fee + registration_fee + delivery_fee;
            double total_before_tax = subtotal + total_fees;
            double total_amount = total_before_tax + sales_tax;


            // Profit analysis is just for backend crunching - not shown to customers
            double market_price = row["market_price"].as<double>();
            double profit = base_price - market_price;
            double profit_percentage = (profit / market_price) * 100;

            // ========================================
            // BUILD JSON RESPONSE
            // ========================================
            
            crow::json::wvalue invoice;

            // Invoice metadata
            invoice["invoice_number"] = row["sale_id"].c_str();
            invoice["invoice_date"] = row["sale_date"].c_str();
            invoice["invoice_type"] = "Vehicle Sale";

            // Customer information
            invoice["customer"]["customer_id"] = row["customer_id"].c_str();
            invoice["customer"]["name"] = customer_name;  // ✅ Use the string variable
            invoice["customer"]["email"] = row["email"].c_str();
            invoice["customer"]["phone"] = row["ph_number"].c_str();
            invoice["customer"]["address"] = row["address"].is_null() ? "" : row["address"].c_str();
            invoice["customer"]["driving_licence"] = row["driving_licence"].c_str();

            // Vehicle information
            invoice["vehicle"]["vehicle_id"] = row["vehicle_id"].c_str();
            invoice["vehicle"]["description"] = vehicle_description;  // ✅ Use the string variable
            invoice["vehicle"]["vin"] = row["vin"].c_str();
            invoice["vehicle"]["year"] = row["year"].as<int>();
            invoice["vehicle"]["make"] = row["make"].c_str();
            invoice["vehicle"]["model"] = row["model"].c_str();
            invoice["vehicle"]["trim"] = row["trim"].is_null() ? "" : row["trim"].c_str();
            invoice["vehicle"]["odometer"] = row["odometer"].as<int>();
            invoice["vehicle"]["fuel_type"] = row["fuel_type"].c_str();
            invoice["vehicle"]["transmission"] = row["transmission"].c_str();

            // Line items
            invoice["line_items"] = crow::json::wvalue::list();
            
            invoice["line_items"][0]["description"] = vehicle_description;  // ✅ Use string variable
            invoice["line_items"][0]["quantity"] = 1;
            invoice["line_items"][0]["unit_price"] = base_price;
            invoice["line_items"][0]["amount"] = base_price;

            invoice["line_items"][1]["description"] = "Documentation Fee";
            invoice["line_items"][1]["quantity"] = 1;
            invoice["line_items"][1]["unit_price"] = documentation_fee;
            invoice["line_items"][1]["amount"] = documentation_fee;

            invoice["line_items"][2]["description"] = "Registration Fee";
            invoice["line_items"][2]["quantity"] = 1;
            invoice["line_items"][2]["unit_price"] = registration_fee;
            invoice["line_items"][2]["amount"] = registration_fee;

            // Pricing breakdown
            invoice["pricing"]["subtotal"] = subtotal;
            invoice["pricing"]["documentation_fee"] = documentation_fee;
            invoice["pricing"]["registration_fee"] = registration_fee;
            invoice["pricing"]["delivery_fee"] = delivery_fee;
            invoice["pricing"]["total_fees"] = total_fees;
            invoice["pricing"]["total_before_tax"] = total_before_tax;
            invoice["pricing"]["sales_tax_rate"] = sales_tax_rate;
            invoice["pricing"]["sales_tax"] = sales_tax;
            invoice["pricing"]["total_amount"] = total_amount;

            // Tax breakdown
            invoice["taxes"] = crow::json::wvalue::list();
            invoice["taxes"][0]["name"] = "HST (Harmonized Sales Tax)";
            invoice["taxes"][0]["rate"] = sales_tax_rate;
            invoice["taxes"][0]["amount"] = sales_tax;

            // Payment information
            invoice["payment"]["status"] = "Paid";
            invoice["payment"]["method"] = "Not specified";
            invoice["payment"]["amount_paid"] = total_amount;
            invoice["payment"]["balance_due"] = 0.00;

            // Internal analytics
            invoice["analytics"]["market_price"] = market_price;
            invoice["analytics"]["profit"] = profit;
            invoice["analytics"]["profit_percentage"] = std::round(profit_percentage * 100) / 100.0;

            // Dealer information
            invoice["dealer"]["name"] = "DealerDrive Auto Sales";
            invoice["dealer"]["address"] = "123 Main Street, Toronto, ON M5V 3A8";
            invoice["dealer"]["phone"] = "(416) 555-0100";
            invoice["dealer"]["email"] = "sales@dealerdrive.com";
            invoice["dealer"]["website"] = "www.dealerdrive.com";

            // Terms and conditions
            invoice["terms"] = crow::json::wvalue::list();
            invoice["terms"][0] = "Vehicle sold 'as is' with no warranty";
            invoice["terms"][1] = "All sales are final";
            invoice["terms"][2] = "Buyer is responsible for vehicle inspection";
            invoice["terms"][3] = "Payment due upon signing";

            // Notes
            invoice["notes"] = "Thank you for your business!";

            return crow::response(200, invoice);

        } catch (const pqxx::sql_error& e) {
            CROW_LOG_ERROR << "SQL Error in GET /sales/" << id << ": " << e.what();
            crow::json::wvalue error;
            error["error"] = "Database error";
            error["message"] = e.what();
            return crow::response(500, error);

        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Error in GET /sales/" << id << ": " << e.what();
            crow::json::wvalue error;
            error["error"] = "Internal server error";
            error["message"] = e.what();
            return crow::response(500, error);
        }
    });

    //------------------------------------------------------------------
    // POST /sales
    //------------------------------------------------------------------
    CROW_ROUTE(app, "/sales")
    .methods("POST"_method)
    ([](const crow::request& req) {

        try{
            // Parse JSON body
            auto body = crow::json::load(req.body);
            if (!body){
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                error["message"] = "Request body must be valid JSON.";
                return crow::response(400, error);
            }
             
            // Validate required fields
            if(!body.has("vehicle_id") || !body.has("customer_id") || !body.has("sale_price") || !body.has("date")){
                crow::json::wvalue error;
                error["error"] = "Missing fields";
                error["message"] = "Required fields: vehicle_id, customer_id, sale_price, date.";
                return crow::response(400, error);
            }

            // Extract values
            std::string vehicle_id = body["vehicle_id"].s();
            std::string customer_id = body["customer_id"].s();
            double sale_price = body["sale_price"].d();
            std::string date = body["date"].s();

             // Validate format 
            if (vehicle_id.length() != 36 || customer_id.length() != 36) {
                crow::json::wvalue error;
                error["error"] = "Invalid UUID format";
                return crow::response(400, error);
            }

            // Validate sale price
            if (sale_price <= 0) {
                crow::json::wvalue error;
                error["error"] = "Sale price must be positive";
                return crow::response(400, error);
            }
            if (sale_price > 1000000) {
                crow::json::wvalue error;
                error["error"] = "Sale price is too high";
                return crow::response(400, error);
            }

            auto conn = getDbConnection();
            pqxx::work txn(*conn);

            // insert and return new sale
            pqxx::result r = txn.exec_params(
                "INSERT INTO Sales (vehicle_id, customer_id, date, sale_price) "
                "VALUES ($1, $2, $3, $4) "
                "RETURNING id, vehicle_id, customer_id, date, sale_price",
                vehicle_id, customer_id, date, sale_price
            );

            txn.commit();

            // response with created sale
            crow::json::wvalue result;
            result["sale_id"] = r[0]["id"].c_str();
            result["vehicle_id"] = r[0]["vehicle_id"].c_str();
            result["customer_id"] = r[0]["customer_id"].c_str();
            result["date"] = r[0]["date"].c_str();
            result["sale_price"] = r[0]["sale_price"].as<double>();

            return crow::response(201, result);

        } catch (const pqxx::sql_error& e) {
            std::string error_msg = e.what();
            crow::json::wvalue error;
            
            // ✅ Handle database constraint violations
            if (error_msg.find("foreign key constraint") != std::string::npos) {
                if (error_msg.find("vehicle_id") != std::string::npos) {
                    error["error"] = "Vehicle not found";
                } else {
                    error["error"] = "Customer not found";
                }
                return crow::response(404, error);
                
            } else if (error_msg.find("unique constraint") != std::string::npos || 
                    error_msg.find("duplicate key") != std::string::npos) {
                error["error"] = "Vehicle already sold";
                return crow::response(409, error);
                
            } else {
                error["error"] = "Database error";
                error["message"] = e.what();
                return crow::response(500, error);
            }
            
        } catch (const std::exception& e) {
            crow::json::wvalue error;
            error["error"] = "Internal server error";
            return crow::response(500, error);
        }
    }) ;

    //------------------------------------------------------------------
    // GET /sales/vehicles/<id>
    //------------------------------------------------------------------
    CROW_ROUTE(app, "/sales/vehicles/<string>")
    .methods("GET"_method)
    ([](const std::string& vehicle_id) {

        auto conn = getDbConnection();

        // Prepared once per connection
        static bool prepared = false;
        if (!prepared) {
            conn->prepare(
                "get_sales_by_vehicle",
                "SELECT s.id AS sale_id, s.date, s.sale_price, "
                "v.id AS vehicle_id, v.make, v.model, "
                "c.id AS customer_id, c.first_name, c.last_name "
                "FROM Sales s "
                "JOIN Vehicles v ON s.vehicle_id = v.id "
                "JOIN Customers c ON s.customer_id = c.id "
                "WHERE v.id = $1 "
                "ORDER BY s.date DESC"
            );
            prepared = true;
        }

        pqxx::work txn(*conn);
        pqxx::result r = txn.exec_prepared("get_sales_by_vehicle", vehicle_id);

        // ✅ Explicit JSON array
        crow::json::wvalue result = crow::json::wvalue::list();
        int i = 0;

        for (const auto& row : r) {
            result[i]["sale_id"] = row["sale_id"].c_str();
            result[i]["date"] = row["date"].c_str();
            result[i]["price"] = row["sale_price"].as<double>();
            result[i]["vehicle_id"] = row["vehicle_id"].c_str();
            result[i]["vehicle"] =
                std::string(row["make"].c_str()) + " " +
                row["model"].c_str();
            result[i]["customer_id"] = row["customer_id"].c_str();
            result[i]["customer"] =
                std::string(row["first_name"].c_str()) + " " +
                row["last_name"].c_str();
            ++i;
        }

        return result;
    });

    //------------------------------------------------------------------
    // GET /sales/customers/<id>
    //------------------------------------------------------------------
    CROW_ROUTE(app, "/sales/customers/<string>")
    .methods("GET"_method)
    ([](const std::string& customer_id) {

        auto conn = getDbConnection();

        // Prepared once per connection
        static bool prepared = false;
        if (!prepared) {
            conn->prepare(
                "get_sales_by_customer",
                "SELECT s.id AS sale_id, s.date, s.sale_price, "
                "v.id AS vehicle_id, v.make, v.model, "
                "c.id AS customer_id, c.first_name, c.last_name "
                "FROM Sales s "
                "JOIN Vehicles v ON s.vehicle_id = v.id "
                "JOIN Customers c ON s.customer_id = c.id "
                "WHERE c.id = $1 "
                "ORDER BY s.date DESC"
            );
            prepared = true;
        }

        pqxx::work txn(*conn);
        pqxx::result r = txn.exec_prepared("get_sales_by_customer", customer_id);

        // Explicit JSON array
        crow::json::wvalue result = crow::json::wvalue::list();
        int i = 0;

        for (const auto& row : r) {
            result[i]["sale_id"] = row["sale_id"].c_str();
            result[i]["date"] = row["date"].c_str();
            result[i]["price"] = row["sale_price"].as<double>();
            result[i]["vehicle_id"] = row["vehicle_id"].c_str();
            result[i]["vehicle"] =
                std::string(row["make"].c_str()) + " " +
                row["model"].c_str();
            result[i]["customer_id"] = row["customer_id"].c_str();
            result[i]["customer"] =
                std::string(row["first_name"].c_str()) + " " +
                row["last_name"].c_str();
            ++i;
        }

        return result;
    });

    //------------------------------------------------------------------
    // PUT /sales/<id> - Update sale
    //------------------------------------------------------------------
    CROW_ROUTE(app, "/sales/<string>")
    .methods("PUT"_method)
    ([](const crow::request& req, const std::string& id) {

        try {
            // ----------------------------
            // Validate UUID
            // ----------------------------
            if (id.length() != 36) {
                crow::json::wvalue error;
                error["error"] = "Invalid UUID format";
                return crow::response(400, error);
            }

            // ----------------------------
            // Parse JSON body
            // ----------------------------
            auto body = crow::json::load(req.body);
            if (!body) {
                crow::json::wvalue error;
                error["error"] = "Invalid JSON";
                return crow::response(400, error);
            }

            bool has_price = body.has("sale_price");
            bool has_date  = body.has("date");

            if (!has_price && !has_date) {
                crow::json::wvalue error;
                error["error"] = "No fields to update";
                error["message"] = "Provide at least sale_price or date.";
                return crow::response(400, error);
            }

            // ----------------------------
            // Validate fields
            // ----------------------------
            double sale_price = 0.0;
            std::string date;

            if (has_price) {
                sale_price = body["sale_price"].d();
                if (sale_price <= 0) {
                    crow::json::wvalue error;
                    error["error"] = "Sale price must be positive";
                    return crow::response(400, error);
                }
            }

            if (has_date) {
                date = body["date"].s();
                if (date.empty()) {
                    crow::json::wvalue error;
                    error["error"] = "Invalid date";
                    return crow::response(400, error);
                }
            }

            // ----------------------------
            // Database update
            // ----------------------------
            auto conn = getDbConnection();
            pqxx::work txn(*conn);

            pqxx::result r;

            if (has_price && has_date) {
                r = txn.exec_params(
                    "UPDATE Sales "
                    "SET sale_price = $1, date = $2 "
                    "WHERE id = $3 "
                    "RETURNING id, vehicle_id, customer_id, date, sale_price",
                    sale_price, date, id
                );
            } else if (has_price) {
                r = txn.exec_params(
                    "UPDATE Sales "
                    "SET sale_price = $1 "
                    "WHERE id = $2 "
                    "RETURNING id, vehicle_id, customer_id, date, sale_price",
                    sale_price, id
                );
            } else {
                r = txn.exec_params(
                    "UPDATE Sales "
                    "SET date = $1 "
                    "WHERE id = $2 "
                    "RETURNING id, vehicle_id, customer_id, date, sale_price",
                    date, id
                );
            }

            if (r.empty()) {
                crow::json::wvalue error;
                error["error"] = "Sale not found";
                return crow::response(404, error);
            }

            txn.commit();

            // ----------------------------
            // Build response
            // ----------------------------
            crow::json::wvalue result;
            result["sale_id"]     = r[0]["id"].c_str();
            result["vehicle_id"]  = r[0]["vehicle_id"].c_str();
            result["customer_id"] = r[0]["customer_id"].c_str();
            result["date"]        = r[0]["date"].c_str();
            result["sale_price"]  = r[0]["sale_price"].as<double>();

            return crow::response(200, result);

        } catch (const pqxx::sql_error& e) {
            CROW_LOG_ERROR << "SQL error in PUT /sales/" << id << ": " << e.what();

            crow::json::wvalue error;
            error["error"] = "Database error";
            error["message"] = e.what();
            return crow::response(500, error);

        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Error in PUT /sales/" << id << ": " << e.what();

            crow::json::wvalue error;
            error["error"] = "Internal server error";
            return crow::response(500, error);
        }
    });

}
