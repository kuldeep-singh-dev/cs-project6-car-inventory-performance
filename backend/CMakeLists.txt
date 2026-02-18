cmake_minimum_required(VERSION 3.10)
project(car_dealership)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

add_library(MainLibrary
    src/modules/test_drive/test_drive.cpp
    src/modules/test_drive/test_drive_routes.cpp
    src/modules/test_drive/test_drive_service.cpp
    src/modules/test_drive/test_drive_controller.cpp
    src/modules/sales/sales.cpp
    src/modules/inventory/inventory.cpp
    src/modules/customer/customer.cpp
    src/modules/inventory/inventory_model.cpp
    src/db/db_connection.cpp
    src/modules/images/images.cpp
)

target_include_directories(MainLibrary
    PUBLIC
        ${PQXX_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src
)

target_link_directories(MainLibrary PRIVATE ${PQXX_LIBRARY_DIRS})

target_link_libraries(MainLibrary
    PRIVATE
        ${PQXX_LIBRARIES}
)

add_executable(Main ../src/main.cpp)

target_link_libraries(Main PRIVATE MainLibrary)

# GoogleTest (shared by all module tests)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Tests for each module

# Customer module tests
add_executable(CustomerUnitTests
    test/customer_tests/customer_tests.cpp
)
target_include_directories(CustomerUnitTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(CustomerUnitTests PRIVATE gtest gtest_main MainLibrary)
add_test(NAME CustomerUnitTests COMMAND CustomerUnitTests)

# Inventory module tests
add_executable(InventoryUnitTests
    test/inventory_tests/InventoryTest.cpp
)
target_include_directories(InventoryUnitTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(InventoryUnitTests PRIVATE gtest gtest_main MainLibrary)
add_test(NAME InventoryUnitTests COMMAND InventoryUnitTests)

# Sales module tests
add_executable(SalesUnitTests
    test/sales_tests/SalesTest.cpp
)
target_include_directories(SalesUnitTests PRIVATE ${CMAKE_SOURCE_DIR}/src ${PQXX_INCLUDE_DIRS})
target_link_libraries(SalesUnitTests PRIVATE gtest gtest_main MainLibrary ${PQXX_LIBRARIES})
add_test(NAME SalesUnitTests COMMAND SalesUnitTests)

# Test-drive module tests
add_executable(TestDriveUnitTests
    test/test_drive_tests/TestDriveTest.cpp
)
target_include_directories(TestDriveUnitTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(TestDriveUnitTests PRIVATE gtest gtest_main MainLibrary)
add_test(NAME TestDriveUnitTests COMMAND TestDriveUnitTests)
