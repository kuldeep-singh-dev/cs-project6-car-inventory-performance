#include "test_drive_routes.h"
#include "../../modules/test_drive/test_drive.h"

void registerTestDriveRoutes(crow::SimpleApp& app) {

    CROW_ROUTE(app, "/testdrive").methods(crow::HTTPMethod::GET)
        ([](crow::response& res) {
            ConnectionGuard guard(getPool());
            TestDriveService testDriveService(guard);
            TestDriveController testDriveController(testDriveService);
            testDriveController.listTestDrives(res);
        });

    CROW_ROUTE(app, "/testdrive/<string>").methods(crow::HTTPMethod::GET)
        ([](const crow::request& req, crow::response& res, string testDriveId) {
            ConnectionGuard guard(getPool());
            TestDriveService testDriveService(guard);
            TestDriveController testDriveController(testDriveService);
            testDriveController.getTestDriveById(res, testDriveId);
        });

    CROW_ROUTE(app, "/testdrive/post").methods(crow::HTTPMethod::POST)
        ([](const crow::request& req, crow::response& res) {
            ConnectionGuard guard(getPool());
            TestDriveService testDriveService(guard);
            TestDriveController testDriveController(testDriveService);
            testDriveController.createTestDrive(req, res);
        });

    CROW_ROUTE(app, "/testdrive/<string>").methods(crow::HTTPMethod::PATCH)
        ([](const crow::request& req, crow::response& res, string testDriveId) {
            ConnectionGuard guard(getPool());
            TestDriveService testDriveService(guard);
            TestDriveController testDriveController(testDriveService);
            testDriveController.updateTestDrive(req, res, testDriveId);
        });

    CROW_ROUTE(app, "/testdrive/customer/<string>").methods(crow::HTTPMethod::GET)
        ([](const crow::request& req, crow::response& res, string customerId) {
            ConnectionGuard guard(getPool());
            TestDriveService testDriveService(guard);
            TestDriveController testDriveController(testDriveService);
            testDriveController.getTestDriveByCustomerId(res, customerId);
        });

    CROW_ROUTE(app, "/testdrive/vehicle/<string>").methods(crow::HTTPMethod::GET)
        ([](const crow::request& req, crow::response& res, string vehicleId) {
            ConnectionGuard guard(getPool());
            TestDriveService testDriveService(guard);
            TestDriveController testDriveController(testDriveService);
            testDriveController.getTestDriveByVehicleId(res, vehicleId);
        });

    CROW_ROUTE(app, "/testdrive/export/csv").methods(crow::HTTPMethod::GET)
        ([](crow::response& res) {
            ConnectionGuard guard(getPool());
            TestDriveService testDriveService(guard);
            TestDriveController testDriveController(testDriveService);
            testDriveController.getExportCsV(res);
        });
}