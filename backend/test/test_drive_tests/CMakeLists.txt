add_executable(TestDriveUnitTests 
	EXCLUDE_FROM_ALL
	TestDriveTest.cpp)

target_include_directories(TestDriveUnitTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(TestDriveUnitTests
    PRIVATE
	#add this two lines for google test
	gtest
        gtest_main
        MainLibrary
)

add_test(
    NAME TestDriveUnitTests
    COMMAND TestDriveUnitTests
)
