#define CROW_MAIN

#include "../Library/crow_all.h"
#include <iostream>
#include "../TestDrive/TestDrive.h"
using namespace std;
using namespace crow;
int main() {
	crow::SimpleApp App;
	TestDrive testDriveApi(0, 0, 0);
	//home page 
	CROW_ROUTE(App, "/")
		([&testDriveApi](const request& req, response& res)
			{
				testDriveApi.testDriveApiTest(res, "/");
			});
	CROW_ROUTE(App, "/<string>")
		([&testDriveApi](const request &req, response& res, string path)
		{
				testDriveApi.testDriveApiTest(res, path);

		});
	CROW_ROUTE(App, "/<string>/<int>")
		([&testDriveApi](const request& req, response& res, string path, int testDriveId)
			{
				testDriveApi.getTestDriveWithId(res, path, testDriveId);
				
			});
	CROW_ROUTE(App, "/get_all/<string>")
		([&testDriveApi](const request& req, response& res, string path)
			{
				testDriveApi.getAllTestDrives(res, "/get_all/"+path);
				
			});
	App.port(3000).multithreaded().run();
	return 0; 
}