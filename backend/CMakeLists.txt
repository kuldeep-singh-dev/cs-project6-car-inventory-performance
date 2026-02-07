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
    # src/modules/customer/customer.cpp
    src/db/db_connection.cpp
)

target_include_directories(MainLibrary
    PRIVATE
        ${PQXX_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(MainLibrary
    PRIVATE
        ${PQXX_LIBRARIES}
)

add_executable(Main src/Main.cpp)

target_link_libraries(Main PRIVATE MainLibrary)



#for google test 

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#enables the test to run 
enable_testing()

add_subdirectory(test)
