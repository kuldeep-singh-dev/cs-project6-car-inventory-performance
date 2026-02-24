#pragma once 
#include "test_drive_service.h"
#include "../../external/crow/crow_all.h"

/// @class TestDriveController
/// @brief TestDriveController Handles the communication with the frontend.
///
/// This class acts as a controller layer above the service, handling requests from
/// the frontend and responding accordingly.
class TestDriveController {
private:
    /// Reference to the TestDriveService used for business logic operations.
    TestDriveService& testDriveService;
public:

    /// @brief Constructs a TestDriveController with the given service.
    /// @param service Reference to a TestDriveService instance.
    TestDriveController(TestDriveService& service);

    /// @brief Lists all test drives.
    /// @param res The Crow response object to send the result.
    void listTestDrives(crow::response& res);

    /// @brief Creates a new test drive from the request data.
    /// @param req The Crow request containing test drive data.
    /// @param res The Crow response object to send the result.
    void createTestDrive(const crow::request& req, crow::response& res);

    /// @brief Updates an existing test drive.
    /// @param req The Crow request containing updated data.
    /// @param res The Crow response object to send the result.
    /// @param testDriveId The ID of the test drive to update.
    void updateTestDrive(const crow::request& req, crow::response& res, string testDriveId);

    /// @brief Retrieves test drives by customer ID.
    /// @param res The Crow response object to send the result.
    /// @param customerId The ID of the customer.
    void getTestDriveByCustomerId(crow::response& res, string customerId);

    /// @brief Retrieves test drives by vehicle ID.
    /// @param res The Crow response object to send the result.
    /// @param vehicleId The ID of the vehicle.
    void getTestDriveByVehicleId(crow::response& res, string vehicleId);

    /// @brief Retrieves a test drive by its ID.
    /// @param res The Crow response object to send the result.
    /// @param testId The ID of the test drive.
    void getTestDriveById(crow::response& res, string testId);

    /// @brief Exports test drives as a CSV file.
    /// @param res The Crow response object to send the CSV data.
    void getExportCsV(crow::response& res);
};
