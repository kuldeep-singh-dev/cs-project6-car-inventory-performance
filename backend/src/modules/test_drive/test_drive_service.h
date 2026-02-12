#pragma once 
#include "test_drive.h"
#include <vector>
#include <pqxx/pqxx>
#include <memory>
#include "../../external/crow/crow_all.h"
using namespace std;
class TestDriveService {
private:
	std::shared_ptr<pqxx::connection> conn;
public:
	TestDriveService(shared_ptr<pqxx::connection> connection);
	crow::json::wvalue getAllTestDrives();
	crow::json::wvalue addTestDrive(const TestDrive& testDrive);
	crow::json::wvalue updateTestDrive(const TestDrive& testDrive);
	//check records
	bool testDriveExists(const TestDrive& testDrive);
	//get vehicle status from database
	string getVehicleStatusFromDataBase(const string vehicleId);
	//get test drive customer by id 
	crow::json::wvalue getTestDriveByCustomerId(const string customerId);
	//get test drive by vehicle id
	crow::json::wvalue getTestDriveByVehicleId(const string vehicleId);

	crow::json::wvalue getTestDriveByTestId(string testId);
	
};
