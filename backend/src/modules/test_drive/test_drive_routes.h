#pragma once
#include "../../external/crow/crow_all.h"
#include "../../db/db_connection.h"
#include "test_drive_controller.h"
#include "test_drive_service.h"


void registerTestDriveRoutes(crow::SimpleApp& app);