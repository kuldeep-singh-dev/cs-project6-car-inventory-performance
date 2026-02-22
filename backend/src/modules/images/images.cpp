#include "images.h"
#include "../../db/db_connection.h"
#include <fstream>
#include <ctime>
#include <filesystem>

namespace fs = std::filesystem;

void registerImagesRoutes(crow::SimpleApp& app) {
    
    // Upload image for a vehicle
    CROW_ROUTE(app, "/vehicles/<string>/images")
        .methods("POST"_method)
    ([](const crow::request& req, const std::string& vehicle_id) {
        try {
            // Parse multipart form data
            crow::multipart::message file_message(req);
            
            if (file_message.parts.empty()) {
                return crow::response(400, "No file uploaded");
            }
            
            auto& part = file_message.parts[0];
            
            // Generate unique filename with timestamp
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                now.time_since_epoch()
            ).count();
            
            std::string filename = vehicle_id + "_" + std::to_string(timestamp) + ".jpg";
            std::string filepath = "/shareddocker/uploads/" + filename;
            
            // Ensure uploads directory exists
            fs::create_directories("/shareddocker/uploads");
            
            // Save file to disk
            std::ofstream file(filepath, std::ios::binary);
            if (!file) {
                return crow::response(500, "Failed to save file");
            }
            file.write(part.body.data(), part.body.size());
            file.close();
            
            // Save to database
            std::string img_url = "/uploads/" + filename;
            
            ConnectionGuard guard(getPool());
            pqxx::work txn(guard.get());
            
            pqxx::result r = txn.exec_params(
                "INSERT INTO Images (vehicle_id, img_url) VALUES ($1, $2) RETURNING id",
                vehicle_id, img_url
            );
            
            std::string image_id = r[0][0].as<std::string>();
            txn.commit();
            
            // Return response
            crow::json::wvalue response;
            response["id"] = image_id;
            response["vehicle_id"] = vehicle_id;
            response["img_url"] = img_url;
            
            return crow::response(201, response);
            
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Image upload error: " << e.what();
            return crow::response(500, e.what());
        }
    });
    
    // Get all images for a vehicle
    CROW_ROUTE(app, "/vehicles/<string>/images")
        .methods("GET"_method)
    ([](const std::string& vehicle_id) {
        try {
            ConnectionGuard guard(getPool());
            pqxx::work txn(guard.get());
            
            pqxx::result r = txn.exec_params(
                "SELECT id, vehicle_id, img_url FROM Images WHERE vehicle_id = $1 ORDER BY id",
                vehicle_id
            );
            
            crow::json::wvalue::list images;
            for (const auto& row : r) {
                crow::json::wvalue img;
                img["id"] = row["id"].as<std::string>();
                img["vehicle_id"] = row["vehicle_id"].as<std::string>();
                img["img_url"] = row["img_url"].as<std::string>();
                images.push_back(std::move(img));
            }
            
            crow::json::wvalue response(std::move(images));
            return crow::response(200, response);
            
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Get images error: " << e.what();
            return crow::response(500, e.what());
        }
    });
    
    // Delete an image
    CROW_ROUTE(app, "/images/<string>")
        .methods("DELETE"_method)
    ([](const std::string& image_id) {
        try {
            ConnectionGuard guard(getPool());
            pqxx::work txn(guard.get());
            
            // Get the img_url before deleting
            pqxx::result r = txn.exec_params(
                "SELECT img_url FROM Images WHERE id = $1",
                image_id
            );
            
            if (r.empty()) {
                return crow::response(404, "Image not found");
            }
            
            std::string img_url = r[0]["img_url"].as<std::string>();
            std::string filepath = "/shareddocker" + img_url;
            
            // Delete from database
            txn.exec_params("DELETE FROM Images WHERE id = $1", image_id);
            txn.commit();
            
            // Delete file from filesystem
            if (fs::exists(filepath)) {
                fs::remove(filepath);
            }
            
            return crow::response(204);
            
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Delete image error: " << e.what();
            return crow::response(500, e.what());
        }
    });
    
    // Serve static image files
    CROW_ROUTE(app, "/uploads/<path>")
        .methods("GET"_method)
    ([](const std::string& filename) {
        try {
            std::string filepath = "/shareddocker/uploads/" + filename;
            
            if (!fs::exists(filepath)) {
                return crow::response(404, "Image not found");
            }
            
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                return crow::response(500, "Failed to read file");
            }
            
            std::string content(
                (std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>()
            );
            
            crow::response res(200, content);
            
            // Set content type based on file extension
            if (filename.ends_with(".jpg") || filename.ends_with(".jpeg")) {
                res.set_header("Content-Type", "image/jpeg");
            } else if (filename.ends_with(".png")) {
                res.set_header("Content-Type", "image/png");
            } else if (filename.ends_with(".gif")) {
                res.set_header("Content-Type", "image/gif");
            } else {
                res.set_header("Content-Type", "application/octet-stream");
            }
            
            return res;
            
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Serve image error: " << e.what();
            return crow::response(500, e.what());
        }
    });
}