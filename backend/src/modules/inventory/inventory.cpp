#include "inventory.h"
#include <iostream>
#include "../../db/db_connection.h"
#include <pqxx/pqxx>

void registerInventoryRoutes(crow::SimpleApp& app) {
    std::cout << "[DEBUG] Inventory routes registered!" << std::endl;

    // Get all vehicles
    CROW_ROUTE(app, "/vehicles")
    .methods(crow::HTTPMethod::GET)
    ([]() {
        try {
            ConnectionGuard guard(getPool());      
            pqxx::work txn(guard.get());              

                pqxx::result res = txn.exec(
                    "SELECT "
                    "  v.id, v.vin, v.make, v.model, v.year, v.odometer, "
                    "  v.fuel_type, v.transmission, v.trim, v.market_price, v.status, "
                    "  (SELECT img_url FROM Images WHERE vehicle_id = v.id LIMIT 1) as first_image "
                    "FROM Vehicles v "
                    "ORDER BY v.year DESC"
                );
                crow::json::wvalue result;
                result = crow::json::wvalue::list();

                size_t i = 0;
                for (auto row : res) {
                    crow::json::wvalue vehicle;
                    vehicle["id"] = row["id"].c_str();
                    vehicle["vin"] = row["vin"].c_str();
                    vehicle["make"] = row["make"].c_str();
                    vehicle["model"] = row["model"].c_str();
                    vehicle["year"] = row["year"].as<int>();
                    vehicle["odometer"] = row["odometer"].as<int>();
                    vehicle["fuel_type"] = row["fuel_type"].c_str();
                    vehicle["transmission"] = row["transmission"].c_str();

                    if (row["trim"].is_null()) vehicle["trim"] = nullptr;
                    else vehicle["trim"] = row["trim"].c_str();

                    vehicle["market_price"] = row["market_price"].as<double>();
                    vehicle["status"] = row["status"].c_str();

                    vehicle["first_image"] = row["first_image"].is_null() ? "" : row["first_image"].as<std::string>();

                    result[i++] = std::move(vehicle);   //list indexing works
                }

                return crow::response{result};


        } catch (const std::exception& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    // Get available vehicles
    CROW_ROUTE(app, "/vehicles/available")
    .methods(crow::HTTPMethod::GET)
    ([]() {
        try {
            ConnectionGuard guard(getPool());      
            pqxx::work txn(guard.get());              

                pqxx::result res = txn.exec(
                    "SELECT "
                    "  v.id, v.vin, v.make, v.model, v.year, v.odometer, "
                    "  v.fuel_type, v.transmission, v.trim, v.market_price, v.status, "
                    "  (SELECT img_url FROM Images WHERE vehicle_id = v.id LIMIT 1) as first_image "
                    "FROM Vehicles v "
                    "WHERE v.status = 'Available' "
                    "ORDER BY v.year DESC"
                );
                crow::json::wvalue result;
                result = crow::json::wvalue::list();

                size_t i = 0;
                for (auto row : res) {
                    crow::json::wvalue vehicle;
                    vehicle["id"] = row["id"].c_str();
                    vehicle["vin"] = row["vin"].c_str();
                    vehicle["make"] = row["make"].c_str();
                    vehicle["model"] = row["model"].c_str();
                    vehicle["year"] = row["year"].as<int>();
                    vehicle["odometer"] = row["odometer"].as<int>();
                    vehicle["fuel_type"] = row["fuel_type"].c_str();
                    vehicle["transmission"] = row["transmission"].c_str();

                    if (row["trim"].is_null()) vehicle["trim"] = nullptr;
                    else vehicle["trim"] = row["trim"].c_str();

                    vehicle["market_price"] = row["market_price"].as<double>();
                    vehicle["status"] = row["status"].c_str();

                    vehicle["first_image"] = row["first_image"].is_null() ? "" : row["first_image"].as<std::string>();

                    result[i++] = std::move(vehicle);   //list indexing works
                }

                return crow::response{result};


        } catch (const std::exception& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    // Get vehicle by ID
    CROW_ROUTE(app, "/vehicles/<string>")
    .methods(crow::HTTPMethod::GET)
    ([](std::string vehicleId) {
        try {
            ConnectionGuard guard(getPool());
            pqxx::work txn(guard.get());

            pqxx::result res = txn.exec_params(
                "SELECT * FROM Vehicles WHERE id = $1",
                vehicleId
            );

            if (res.empty()) {
                return crow::response(404, "Vehicle not found");
            }

            auto row = res[0];
            crow::json::wvalue vehicle;
            vehicle["id"] = row["id"].c_str();
            vehicle["vin"] = row["vin"].c_str();
            vehicle["make"] = row["make"].c_str();
            vehicle["model"] = row["model"].c_str();
            vehicle["year"] = row["year"].as<int>();
            vehicle["odometer"] = row["odometer"].as<int>();
            vehicle["fuel_type"] = row["fuel_type"].c_str();
            vehicle["transmission"] = row["transmission"].c_str();
            vehicle["trim"] = row["trim"].c_str();
            vehicle["market_price"] = row["market_price"].as<double>();
            vehicle["status"] = row["status"].c_str();

            return crow::response{vehicle};
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    // Create a new vehicle
    CROW_ROUTE(app, "/vehicles")
    .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
         std::cout << "[DEBUG] POST /vehicles hit" << std::endl;
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        try {
            ConnectionGuard guard(getPool());
            pqxx::work txn(guard.get());

            // Convert crow::json::r_string to std::string
            pqxx::row row = txn.exec_params(
                "INSERT INTO Vehicles (vin, make, model, year, odometer, fuel_type, transmission, trim, market_price, status) "
                "VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10) RETURNING id",
                std::string(body["vin"].s()),
                std::string(body["make"].s()),
                std::string(body["model"].s()),
                body["year"].i(),
                body["odometer"].i(),
                std::string(body["fuel_type"].s()),
                std::string(body["transmission"].s()),
                std::string(body["trim"].s()),
                body["market_price"].d(),
                std::string(body["status"].s())
            )[0];

            
            txn.commit();
            crow::json::wvalue res;
            res["id"] = row["id"].c_str();
            return crow::response(201, res);
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });


    CROW_ROUTE(app, "/vehicles/<string>")
    .methods(crow::HTTPMethod::PUT)
    ([](const crow::request& req, std::string vehicleId) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        try {
            ConnectionGuard guard(getPool());   
            pqxx::work txn(guard.get());

            pqxx::result res = txn.exec_params(
                "UPDATE Vehicles SET vin=$1, make=$2, model=$3, year=$4, odometer=$5, "
                "fuel_type=$6, transmission=$7, trim=$8, market_price=$9, status=$10 "
                "WHERE id::text = $11",
                std::string(body["vin"].s()),
                std::string(body["make"].s()),
                std::string(body["model"].s()),
                body["year"].i(),
                body["odometer"].i(),
                std::string(body["fuel_type"].s()),
                std::string(body["transmission"].s()),
                std::string(body["trim"].s()),
                body["market_price"].d(),
                std::string(body["status"].s()),
                vehicleId
            );

            if (res.affected_rows() == 0) {
                return crow::response(404, "Vehicle not found or UUID mismatch");
            }

            txn.commit();
            std::cout << "[DEBUG] PUT /vehicles/" << vehicleId << " updated successfully" << std::endl;
            return crow::response(200, "Vehicle updated successfully");

        } catch (const std::exception& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/test_post").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        std::cout << "[DEBUG] /test_post received POST!" << std::endl;
        return "POST works";
    });

}
