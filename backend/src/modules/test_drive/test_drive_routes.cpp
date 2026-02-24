#include "test_drive_routes.h"
#include "../../modules/test_drive/test_drive.h"

void registerTestDriveRoutes(crow::SimpleApp& app) {

	//define routes
	//home route get rid off this 
	CROW_ROUTE(app, "/testdrive").methods(crow::HTTPMethod::GET)
		([](crow::response& res) {
		//create databae connection
		auto conn = getDbConnection();

		//create service and controller instances
		TestDriveService testDriveService(conn);
		TestDriveController testDriveController(testDriveService);
		testDriveController.listTestDrives(res);
			});
	//testdrive by id 
	CROW_ROUTE(app, "/testdrive/<string>").methods(crow::HTTPMethod::GET)
		([](const crow::request& req,crow::response& res, string testDriveId) {
		//create databae connection
		auto conn = getDbConnection();

		//create service and controller instances
		TestDriveService testDriveService(conn);
		TestDriveController testDriveController(testDriveService);
		testDriveController.getTestDriveById(res, testDriveId);
			
			});

	//create test drive OR schedule test drive
	CROW_ROUTE(app, "/testdrive/post").methods(crow::HTTPMethod::POST)
		([](const crow::request& req, crow::response& res) {
		//create databae connection
		auto conn = getDbConnection();

		//create service and controller instances
		TestDriveService testDriveService(conn);
		TestDriveController testDriveController(testDriveService);
		testDriveController.createTestDrive(req, res);
			});
	//update test drive 
	CROW_ROUTE(app, "/testdrive/<string>").methods(crow::HTTPMethod::PATCH)
		([](const crow::request& req, crow::response& res, string testDriveId) {
		//create databae connection
		auto conn = getDbConnection();

		//create service and controller instances
		TestDriveService testDriveService(conn);
		TestDriveController testDriveController(testDriveService);
		testDriveController.updateTestDrive(req, res,testDriveId);
			});
	//get test drive by customer id 
	CROW_ROUTE(app, "/testdrive/customer/<string>").methods(crow::HTTPMethod::GET)
		([](const crow::request& req, crow::response& res, string customerId) {
		//create databae connection
		auto conn = getDbConnection();

		//create service and controller instances
		TestDriveService testDriveService(conn);
		TestDriveController testDriveController(testDriveService);
		testDriveController.getTestDriveByCustomerId(res, customerId);
			});
	CROW_ROUTE(app, "/testdrive/vehicle/<string>").methods(crow::HTTPMethod::GET)
		([](const crow::request& req, crow::response& res, string vehicleId) {
		//create databae connection
		auto conn = getDbConnection();

		//create service and controller instances
		TestDriveService testDriveService(conn);
		TestDriveController testDriveController(testDriveService);
		testDriveController.getTestDriveByVehicleId(res, vehicleId);
			});
	CROW_ROUTE(app, "/testdrive/export/csv").methods(crow::HTTPMethod::GET)
		([](crow::response& res) {
		//create databae connection
		auto conn = getDbConnection();

		//create service and controller instances
		TestDriveService testDriveService(conn);
		TestDriveController testDriveController(testDriveService);
		testDriveController.getExportCsV(res);
			});
}