#include "test_drive_controller.h"

TestDriveController::TestDriveController(TestDriveService& service) : testDriveService(service) {}

void TestDriveController::listTestDrives(crow::response& res) {
		
	//set header
	res.set_header("Content-Type", "application/json"); 
	try {
		//get all test drives from service
		res.write(testDriveService.getAllTestDrives().dump());
		res.end();
	}
	catch (const std::exception& e) {
		res.code = 500;
		crow::json::wvalue result;
		result["error"] = "Internal Server Error: " + string(e.what());
		res.end(result.dump());
	}
}
void TestDriveController::createTestDrive(const crow::request& req, crow::response& res) {
	auto body = crow::json::load(req.body);
	crow::json::wvalue result;
	//set header json
	res.set_header("Content-Type", "application/json");
	try {
		//check body is not empty
		if (!body) {
			res.code = 400;
			result["error"] = "Invalid input. Expected JSON format.";
			res.end(result.dump());
			return;
		}
		//check required fields are present
		if (!body.has("customerId") || !body.has("vehicleId") || !body.has("date")) {
			res.code = 400;
			result["error"] = "Missing required fields: firstName, lastName, vehicleType, date";
			res.end(result.dump());
			return;
		}
		// extract values
		string customerId = body["customerId"].s();
		string vehicleId = body["vehicleId"].s();
		string date = body["date"].s();
		//validate 
		if (vehicleId.length() != 36 || customerId.length() != 36) {
			result["error"] = "Invalid UUID format";
			res.end(result.dump());
			return;
		}
		TestDrive td;
		td.setCustomerId(customerId);
		td.setVehicleId(vehicleId);
		td.setDate(body["date"].s());
		td.setComment(body["comment"].s());
		//check date format before adding
		if (!td.isDateValid(td.getDate())) {
			res.code = 400;
			result["error"] = "Invalid date format. Expected YYYY-MM-DD.";
			res.end(result.dump());
			return;
		}
		//check if vehicle status is sold
		if (testDriveService.getVehicleStatusFromDataBase(vehicleId) == "Sold") {
			res.code = 409;
			result["error"] = "Vehicle is already sold. Cannot schedule test drive.";
			res.end(result.dump());
			return;
		}

		//return id of created test drive
		res.code = 201;
		res.end(testDriveService.addTestDrive(td).dump());
	}
	catch (const std::exception& e) {
		res.code = 500;
		result["error"] = "Internal Server Error: " + string(e.what());
		res.end(result.dump());
	}
}
void TestDriveController::updateTestDrive(const crow::request& req, crow::response& res, string testDriveId) {
	//check if id is valid uuid
	
	auto body = crow::json::load(req.body);
	crow::json::wvalue result;
	TestDrive td;
	//set header json
	res.set_header("Content-Type", "application/json");
	try {
		if (testDriveId.length() != 36) {
			crow::json::wvalue result;
			res.code = 400;
			result["error"] = "Invalid UUID format";
			res.end(result.dump());
			return;
		}

		//check body is not empty
		if (!body) {
			res.code = 400;
			result["error"] = "Invalid input. Expected JSON format.";
			res.end(result.dump());
			return;
		}
		if (body.has("date")) {
			string date = body["date"].s();
			if (!td.isDateValid(date)) {
				res.code = 400;
				result["error"] = "Invalid date format. Expected YYYY-MM-DD.";
				res.end(result.dump());
				return;
			}
			td.setDate(date);
		}
		if (body.has("comment")) {
			td.setComment(body["comment"].s());
		}
		//check if the body is just emopty or has no updatable fields
		if (!body.has("date") && !body.has("comment")) {
			res.code = 400;
			result["error"] = "No updatable fields provided. At least one of 'date' or 'comment' must be provided.";
			res.end(result.dump());
			return;
		}
		td.setTestDriveId(testDriveId);
		res.write(testDriveService.updateTestDrive(td).dump());
		res.code = 200;
		res.end();

	}
	catch (const std::exception& e) {
		res.code = 500;
		result["error"] = "Internal Server Error: " + string(e.what());
		res.end(result.dump());
	}

}
void TestDriveController::getTestDriveByCustomerId(crow::response& res, string customerId) {
	//validate the id
	crow::json::wvalue result;
	res.add_header("Content-Type", "application/json");
	try {
		if (customerId.length() != 36) {
			res.code = 400;
			result["error"] = "Invalid UUID format";
			res.end(result.dump());
			return;
		}
		//get test drive by customer id
		crow::json::wvalue testDrive = testDriveService.getTestDriveByCustomerId(customerId);
		if (!testDrive.size()) {
			res.code = 404;
			result["error"] = "No test drive found for the given customer ID.";
			res.end(result.dump());
			return;
		}
		res.set_header("Content-Type", "application/json");
		res.write(testDrive.dump());
		res.end();
	}
	catch (const std::exception& e) {
		res.code = 500;
		result["error"] = "Internal Server Error: " + string(e.what());
		res.end(result.dump());
	}
}
void TestDriveController::getTestDriveByVehicleId(crow::response& res, string vehicleId) {
	crow::json::wvalue result;
	res.add_header("Content-Type", "application/json");
	try {
		if (vehicleId.length() != 36) {
			res.code = 400;
			result["error"] = "Invalid UUID format";
			res.end(result.dump());
			return;
		}
		//get test drive by vehicle id
		crow::json::wvalue testDrive = testDriveService.getTestDriveByVehicleId(vehicleId);
		if (testDrive.size()==0) {
			res.code = 404;
			result["error"] = "No test drive found for the given vehicle ID.";
			res.end(result.dump());
			return;
		}
		res.set_header("Content-Type", "application/json");
		res.write(testDrive.dump());
		res.end();
	}
	catch (const std::exception& e) {
		res.code = 500;
		result["error"] = "Internal Server Error: " + string(e.what());
		res.end(result.dump());
	}
}