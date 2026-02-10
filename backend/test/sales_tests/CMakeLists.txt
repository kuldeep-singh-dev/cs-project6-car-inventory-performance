# test/CMakeLists.txt

# Sales Unit Tests
add_executable(SalesUnitTests 
    EXCLUDE_FROM_ALL
    SalesTest.cpp
)

target_include_directories(SalesUnitTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${PQXX_INCLUDE_DIRS}
)

target_link_libraries(SalesUnitTests
    PRIVATE
        gtest
        gtest_main
        MainLibrary
        ${PQXX_LIBRARIES}
)

add_test(
    NAME SalesUnitTests
    COMMAND SalesUnitTests
)


