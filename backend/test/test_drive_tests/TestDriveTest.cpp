#include <cassert>
#include "../../src/modules/test_drive/test_drive.h"
#include "gtest/gtest.h"


//add unit test for testdrive module 
TEST(TestDriveTests, setTestDriveId) {
	//Arrange
	TestDrive td;
	string expected = "123dlsla45";
	td.setTestDriveId(expected);
	//Act
	string actual = td.getTestDriveId();
	// Assert
	ASSERT_EQ(actual, expected);
}
TEST(TestDriveTests, setCustomerId) {
	//Arrange
	TestDrive td;
	string expected = "cust12345";
	td.setCustomerId(expected);
	//Act
	string actual = td.getCustomerId();
	// Assert
	ASSERT_EQ(actual, expected);
}
TEST(TestDriveTests, setVehicleId) {
	//Arrange
	TestDrive td;
	string expected = "veh12345";
	td.setVehicleId(expected);
	//Act
	string actual = td.getVehicleId();
	// Assert
	ASSERT_EQ(actual, expected);
}
TEST(TestDriveTests, setDateValid) {
	//Arrange
	TestDrive td;
	string expected = "2024-12-31";
	td.setDate(expected);
	//Act
	string actual = td.getDate();
	// Assert
	ASSERT_EQ(actual, expected);
}
TEST(TestDriveTests, setComment) {
	//Arrange
	TestDrive td;
	string expected = "This is a test drive comment.";
	td.setComment(expected);
	//Act
	string actual = td.getComment();
	// Assert
	ASSERT_EQ(actual, expected);
}
TEST(TestDriveTests, setDateInvalidFormat) {
	//Arrange
	TestDrive td;
	string invalidDate = "2024/12/31";
	//Act
	bool isValid = td.isDateValid(invalidDate);
	// Assert
	ASSERT_FALSE(isValid);
}
TEST(TestDriveTests, setInvalidDate) {
	//Arrange
	TestDrive td;
	string invalidDate = "2024-12-32";
	//Act
	bool isValid = td.isDateValid(invalidDate);
	// Assert
	ASSERT_FALSE(isValid);
}
TEST(TestDriveTests, setInvalidMonth) {
	//Arrange
	TestDrive td;
	string invalidDate = "2024-13-01";
	//Act
	bool isValid = td.isDateValid(invalidDate);
	// Assert
	ASSERT_FALSE(isValid);
}
TEST(TestDriveTests, setInvalidDay) {
	//Arrange
	TestDrive td;
	string invalidDate = "2024-12-00";
	//Act
	bool isValid = td.isDateValid(invalidDate);
	// Assert
	ASSERT_FALSE(isValid);
}
TEST(TestDriveTests, setValidDate) {
	//Arrange
	TestDrive td;
	string validDate = "2024-06-15";
	//Act
	bool isValid = td.isDateValid(validDate);
	// Assert
	ASSERT_TRUE(isValid);
}
