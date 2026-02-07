#define CROW_MAIN
#include "external/crow/crow_all.h"
#include "modules/sales/sales.h"
#include "modules/test_drive/test_drive_routes.h"
int main() {
	// Create the Crow application (HTTP server)
    crow::SimpleApp app;

	// Register routes from the sales module
    registerSalesRoutes(app);

	// Register routes from the test drive module
	registerTestDriveRoutes(app);

	// Start the server on port 3000
    app.port(3000).multithreaded().run();
	
    return 0;
}
