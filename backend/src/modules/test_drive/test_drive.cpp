#include "test_drive.h"


//getters and setters
void TestDrive::setTestDriveId(string id) {
	this->testDriveId = id;
}
string TestDrive::getTestDriveId() const{
	return testDriveId;
}
void TestDrive::setCustomerId(string id) {
	this->customerId = id;
}
string TestDrive::getCustomerId() const{
	return customerId;
}
void TestDrive::setVehicleId(string id) {
	this->vehicleId = id;
}
string TestDrive::getVehicleId() const{
	return vehicleId;
}
bool TestDrive::isDateValid(const string& date) {
	//implement date validation logic here (YYYY-MM-DD)
	static const std::regex pattern(R"(^\d{4}-\d{2}-\d{2}$)");
	if (!std::regex_match(date, pattern))
		return false;

	int y, m, d;
	if (sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d) != 3)
		return false;

	if (m < 1 || m > 12)
		return false;

	if (d < 1 || d > 31)
		return false;

	return true;

	return true;
}
void TestDrive::setDate(std::string date) {
	//if time is not past today return error
	
	this->date = date;
}
std::string TestDrive::getDate()const {
	return date;
}
void TestDrive::setComment(std::string comm) {
	comment = comm;
}

std::string TestDrive::getComment()const {
	return comment;
}
crow::json::wvalue TestDrive::toJson() const {
	crow::json::wvalue json;
	json["testDriveId"] = testDriveId;
	json["customerId"] = customerId;
	json["vehicleId"] = vehicleId;
	json["date"] = date;
	json["comment"] = comment;
	return json;
}
