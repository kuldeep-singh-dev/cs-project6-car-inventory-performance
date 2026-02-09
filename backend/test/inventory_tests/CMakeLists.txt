add_executable(InventoryUnitTests
    EXCLUDE_FROM_ALL
    InventoryTest.cpp
)

target_include_directories(InventoryUnitTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(InventoryUnitTests
    PRIVATE
        gtest
        gtest_main
        MainLibrary
)

add_test(
    NAME InventoryUnitTests
    COMMAND InventoryUnitTests
)
