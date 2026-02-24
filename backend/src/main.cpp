#define CROW_MAIN
#include "external/crow/crow_all.h"
#include "modules/sales/sales.h"
#include "modules/test_drive/test_drive_routes.h"
#include "modules/inventory/inventory.h"
#include "modules/customer/customer.h"
#include "modules/images/images.h"

    int
    main()
{
    // Create the Crow application (HTTP server)
    crow::SimpleApp app;

	// Register routes from the sales module
    registerSalesRoutes(app);

    // Register routes from the inventory module
    registerInventoryRoutes(app);

    // Register routes from the customer module
    registerCustomerRoutes(app);
	
   // Register routes from the test drive module
	registerTestDriveRoutes(app);

    // Register routes from the images module
	registerImagesRoutes(app);

	// Start the server on port 3000
    app.port(3000).multithreaded().run();
	
    return 0;
}
