#pragma once 
#include "test_drive_service.h"
#include "../../external/crow/crow_all.h"

class TestDriveController {
private:
	TestDriveService& testDriveService;
public:
	TestDriveController(TestDriveService& service);
	void listTestDrives(crow::response& res);
	void createTestDrive(const crow::request& req, crow::response& res);
	void updateTestDrive(const crow::request& req, crow::response& res, string testDriveId);
	void getTestDriveByCustomerId(crow::response& res, string customerId);
	void getTestDriveByVehicleId(crow::response& res, string vehicleId);
	void getTestDriveById(crow::response& res, string testId);
};
