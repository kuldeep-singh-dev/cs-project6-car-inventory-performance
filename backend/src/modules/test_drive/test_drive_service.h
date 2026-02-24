#pragma once 
#include "test_drive.h"
#include <vector>
#include <pqxx/pqxx>
#include <memory>
#include "../../external/crow/crow_all.h"
#include "../../db/db_connection.h"

using namespace std;

/// @class TestDriveService
/// @brief TestDriveService Provides business logic and database operations for test drives.
///
/// This service class handles all operations related to test drives, including
/// creation, retrieval, updating, and exporting data.
class TestDriveService {
private:
    /// Reference to a database connection guard for managing DB connections.
    ConnectionGuard& guard; 
public:
    /// @brief Constructs a TestDriveService with the given database connection guard.
    /// @param g Reference to a ConnectionGuard instance.
    TestDriveService(ConnectionGuard& g); 

    /// @brief Retrieves all test drives.
    /// @return A JSON object containing all test drives.
    crow::json::wvalue getAllTestDrives();

    /// @brief Adds a new test drive.
    /// @param testDrive The TestDrive object to add.
    /// @return A JSON object with the result of the operation.
    crow::json::wvalue addTestDrive(const TestDrive& testDrive);

    /// @brief Updates an existing test drive.
    /// @param testDrive The TestDrive object with updated data.
    /// @return A JSON object with the result of the operation.
    crow::json::wvalue updateTestDrive(const TestDrive& testDrive);

    /// @brief Checks if a test drive record exists.
    /// @param testDrive The TestDrive object to check.
    /// @return True if the test drive exists, false otherwise.
    bool testDriveExists(const TestDrive& testDrive);

    /// @brief Retrieves the status of a vehicle from the database.
    /// @param vehicleId The ID of the vehicle.
    /// @return The status of the vehicle as a string.
    string getVehicleStatusFromDataBase(const string vehicleId);

    /// @brief Retrieves test drives by customer ID.
    /// @param customerId The ID of the customer.
    /// @return A JSON object containing the test drives for the customer.
    crow::json::wvalue getTestDriveByCustomerId(const string customerId);

    /// @brief Retrieves test drives by vehicle ID.
    /// @param vehicleId The ID of the vehicle.
    /// @return A JSON object containing the test drives for the vehicle.
    crow::json::wvalue getTestDriveByVehicleId(const string vehicleId);

    /// @brief Retrieves a test drive by its test ID.
    /// @param testId The ID of the test drive.
    /// @return A JSON object containing the test drive.
    crow::json::wvalue getTestDriveByTestId(string testId);

    /// @brief Exports all test drives as a CSV string.
    /// @return A string containing all test drives in CSV format.
    string getAllTestDrivesCSV();
};
