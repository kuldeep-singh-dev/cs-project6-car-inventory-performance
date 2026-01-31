#include "test_drive.h"
#include <iostream>

TestDrive::TestDrive(int tId, int cId, int vId) {
	testId = tId;
	customerId = cId;
	vehicleId = vId;
}
void TestDrive::setTestId(int tId) {
	testId = tId;
}
int TestDrive::getTestId() {
	return testId;
}
void TestDrive::setCustomerId(int cId) {
	customerId = cId;
}
int TestDrive::getCustomerId() {
	return customerId;
}
void TestDrive::setVehicleId(int vId) {
	vehicleId = vId;
}
int TestDrive::getVehicleId() {
	return vehicleId;
}
void TestDrive::setComment(std::string comm) {
	comment = comm;
}
std::string TestDrive::getComment() {
	return comment;
}
//API functions
void TestDrive::testDriveApiTest(crow::response& res, string apiRoute) {
	res.set_header("Content-Type", "text/css");
	res.write("Test Drive API is working at " + apiRoute );
	res.end();
}
void TestDrive::getTestDriveWithId(crow::response& res, std::string apiRoute, int testID) {
	//Implementation for getting a test drive by ID
	res.set_header("Content-Type", "application/json");
	res.write("{ \"testId\": " + std::to_string(testID) + ", \"customerId\": 123, \"vehicleId\": 456, \"comment\": \"Test drive comment\" }");
	res.end();
}
void TestDrive::getAllTestDrives(crow::response& res, std::string apiRoute) {
	//Implementation for getting all test drives
	res.set_header("Content-Type", "application/json");
	res.write("[ { \"testId\": 1, \"customerId\": 123, \"vehicleId\": 456, \"comment\": \"First test drive\" }, { \"testId\": 2, \"customerId\": 789, \"vehicleId\": 101, \"comment\": \"Second test drive\" } ]");
	res.end();
}
void TestDrive::createTestDrive(crow::response& res, std::string apiRoute, int customerID, int vehicleID, std::string comment) {
	//Implementation for creating a new test drive
}

