#define CROW_MAIN
#include "modules/sales/sales.h"

int main() {
	// Create the Crow application (HTTP server)
    crow::SimpleApp app;

	// Register routes from the sales module
    registerSalesRoutes(app);

	// Start the server on port 3000
    app.port(3000).multithreaded().run();
	
    return 0;
}






















// #define CROW_MAIN

// #include "external/crow/crow_all.h"
// #include <iostream>
// #include "test_drive/test_drive.h"
// using namespace std;
// using namespace crow;
// int main() {
// 	crow::SimpleApp App;
// 	TestDrive testDriveApi(0, 0, 0);
// 	//home page 
// 	CROW_ROUTE(App, "/")
// 		([&testDriveApi](const request& req, response& res)
// 			{
// 				testDriveApi.testDriveApiTest(res, "/");
// 			});
// 	CROW_ROUTE(App, "/<string>")
// 		([&testDriveApi](const request &req, response& res, string path)
// 		{
// 				testDriveApi.testDriveApiTest(res, path);

// 		});
// 	CROW_ROUTE(App, "/<string>/<int>")
// 		([&testDriveApi](const request& req, response& res, string path, int testDriveId)
// 			{
// 				testDriveApi.getTestDriveWithId(res, path, testDriveId);
				
// 			});
// 	CROW_ROUTE(App, "/get_all/<string>")
// 		([&testDriveApi](const request& req, response& res, string path)
// 			{
// 				testDriveApi.getAllTestDrives(res, "/get_all/"+path);
				
// 			});
// 	App.port(3000).multithreaded().run();
// 	return 0; 
// }