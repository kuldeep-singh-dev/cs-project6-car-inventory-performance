#pragma once
#include <chrono>
#include "string"
#include <regex>
#include <iostream>
#include "../../external/crow/crow_all.h"
using namespace std;
class TestDrive {
	private:
		string testDriveId;
		string customerId;
		string vehicleId;
		string date;
		string comment;
	public:
		void setTestDriveId(string id);
		string getTestDriveId()const;
		void setCustomerId(string id);
		string getCustomerId()const;
		void setVehicleId(string id);
		string getVehicleId()const;	
		bool isDateValid(const string& date);
		void setDate(std::string date);
		string getDate()const;
		void setComment(std::string comm);
		string getComment()const;
		crow::json::wvalue toJson() const;

};