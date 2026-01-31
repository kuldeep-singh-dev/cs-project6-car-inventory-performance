#include "sales.h"
#include "../../db/db_connection.h"
#include <pqxx/pqxx>

void registerSalesRoutes(crow::SimpleApp& app) {

    CROW_ROUTE(app, "/sales")
    .methods("GET"_method)
    ([]() {

        auto conn = getDbConnection();
        pqxx::work txn(*conn);

        pqxx::result r = txn.exec(
            "SELECT s.id, s.date, s.sale_price, "
            "v.make, v.model, "
            "c.first_name, c.last_name "
            "FROM Sales s "
            "JOIN Vehicles v ON s.vehicle_id = v.id "
            "JOIN Customers c ON s.customer_id = c.id "
            "LIMIT 20"
        );

        crow::json::wvalue result;
        int i = 0;

        for (const auto& row : r) {
            result[i]["sale_id"] = row["id"].c_str();
            result[i]["date"] = row["date"].c_str();
            result[i]["price"] = row["sale_price"].as<double>();
            result[i]["vehicle"] =
                std::string(row["make"].c_str()) + " " +
                row["model"].c_str();
            result[i]["customer"] =
                std::string(row["first_name"].c_str()) + " " +
                row["last_name"].c_str();
            i++;
        }

        return result;
    });
}
