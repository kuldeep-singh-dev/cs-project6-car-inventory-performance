#include "test_drive_routes.h"
#include "../../modules/test_drive/test_drive.h"

void registerTestDriveRoutes(crow::SimpleApp& app) {

	//create databae connection
	static auto conn = getDbConnection();

	//create service and controller instances
	static TestDriveService testDriveService(conn);
	static TestDriveController testDriveController(testDriveService);

	//define routes
	//home route get rid off this 
	CROW_ROUTE(app, "/testdrive").methods(crow::HTTPMethod::GET)
		([](crow::response& res) {
		testDriveController.listTestDrives(res);
			});
	//testdrive by id 
	CROW_ROUTE(app, "/testdrive/<string>").methods(crow::HTTPMethod::GET)
		([](const crow::request& req,crow::response& res, string testDriveId) {
		testDriveController.getTestDriveById(res, testDriveId);
			
			});

	//create test drive OR schedule test drive
	CROW_ROUTE(app, "/testdrive/post").methods(crow::HTTPMethod::POST)
		([](const crow::request& req, crow::response& res) {
		testDriveController.createTestDrive(req, res);
			});
	//update test drive 
	CROW_ROUTE(app, "/testdrive/<string>").methods(crow::HTTPMethod::PATCH)
		([](const crow::request& req, crow::response& res, string testDriveId) {
		testDriveController.updateTestDrive(req, res,testDriveId);
			});
	//get test drive by customer id 
	CROW_ROUTE(app, "/testdrive/customer/<string>").methods(crow::HTTPMethod::GET)
		([](const crow::request& req, crow::response& res, string customerId) {
		testDriveController.getTestDriveByCustomerId(res, customerId);
			});
	CROW_ROUTE(app, "/testdrive/vehicle/<string>").methods(crow::HTTPMethod::GET)
		([](const crow::request& req, crow::response& res, string vehicleId) {
		testDriveController.getTestDriveByVehicleId(res, vehicleId);
			});
}