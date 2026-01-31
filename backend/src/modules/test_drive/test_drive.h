#pragma once

#include <iostream>
#include "../../external/crow/crow_all.h"
using namespace std;
using namespace crow;

class TestDrive {
	private:
		int testId;
		int customerId;
		int vehicleId;
		string comment = "";
	public:
		TestDrive(int tId, int cId, int vId);
		void setTestId(int tId);
		int getTestId();
		void setCustomerId(int cId);
		int getCustomerId();
		void setVehicleId(int vId);
		int getVehicleId();
		void setComment(string comm);
		string getComment();

		//APi functions
		void testDriveApiTest(response& res, string apiRoute);
		void getTestDriveWithId(response& res, string apiRoute, int testID);
		void getAllTestDrives(response& res, string apiRoute);
		void createTestDrive(response& res, string apiRoute, int customerID, int vehicleID, string comment = " ");
};