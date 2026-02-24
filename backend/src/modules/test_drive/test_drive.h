#pragma once
#pragma once
#include <chrono>
#include <string>
#include <regex>
#include <iostream>
#include "../../external/crow/crow_all.h"
using namespace std;

/// @class TestDrive
/// @brief TestDrive model for test drive records.
/// 
/// The TestDrive class stores information about booked test drives,
/// including the ID, date, and comment.
class TestDrive {
private:
    string testDriveId;   
    string customerId;    
    string vehicleId;     
    string date;          
    string comment;       
    /// @brief Sets the TestDrive ID.
    /// @param id Unique TestDrive identifier.
    void setTestDriveId(string id);

    /// @brief Gets the TestDrive ID.
    /// @return Unique TestDrive identifier.
    string getTestDriveId() const;

    /// @brief Sets the customer ID for the TestDrive record.
    /// @param id Unique customer identifier.
    void setCustomerId(string id);

    /// @brief Gets the customer ID.
    /// @return Customer ID.
    string getCustomerId() const;

    /// @brief Sets the vehicle ID for the TestDrive record.
    /// @param id Unique vehicle identifier.
    void setVehicleId(string id);

    /// @brief Gets the vehicle ID.
    /// @return Vehicle ID.
    string getVehicleId() const;

    /// @brief Checks if the date format is valid.
    /// @param date Date string to validate.
    /// @return True if format matches, otherwise False.
    bool isDateValid(const string& date);

    /// @brief Sets the date for the TestDrive record.
    /// @param date Date string.
    void setDate(std::string date);

    /// @brief Gets the date from the TestDrive object.
    /// @return Date string.
    string getDate() const;

    /// @brief Sets the comment for the TestDrive record.
    /// @param comm Notes from customer.
    void setComment(std::string comm);

    /// @brief Gets the comment from the TestDrive object.
    /// @return Customer notes.
    string getComment() const;

    /// @brief Converts the TestDrive object to JSON.
    /// @return JSON representation of the TestDrive.
    crow::json::wvalue toJson() const;
};
#include <chrono>
#include "string"
#include <regex>
#include <iostream>
#include "../../external/crow/crow_all.h"
using namespace std;

///@class TestDrive
///@brief TestDrive model for testdrive record 
/// 
///The TestDrive class stores information about booked test drive 
///including the Id, date and comment
///@author Ginbot Weldeab

class TestDrive {
	private:
		string testDriveId;
		string customerId;
		string vehicleId;
		string date;
		string comment;
	public:
		///@brief Set TestDrive ID
		///
		/// This functions set the Id for the testdrive 
		/// @param id unique TestDrive Identifier 
		void setTestDriveId(string id);
		///@brief Gets TestDriveId 
		///
		/// This functions return the test Id when it is called
		/// @return id unique TestDrive identifier
		string getTestDriveId()const;
		///@brief Set Customer ID
		///
		/// This functions set the Id for the customer to TestDrive record 
		/// @param customerId unique identifier of customer on record
		void setCustomerId(string id);
		///@brief Gets customer Id  
		///
		/// This functions return the customer Id when it is called
		/// @return customer Id 
		string getCustomerId()const;
		///@brief Sets vehicle Id  
		///
		/// This functions sets the vehicle Id for TestDrive instance when it is called
		/// @param vehicle Id unique identifier of vehicle  
		void setVehicleId(string id);
		///@brief Gets vehicle Id 
		///
		/// This functions return the test Id when it is called
		/// @return Vehicle Id
		string getVehicleId()const;
		///@brief Check date format
		///
		/// This function checks if the data format matches the format we
		/// expect from the user
		/// @param Date
		/// @return True if format matches otherwise False 
		bool isDateValid(const string& date);
		///@brief Sets Date  
		///
		/// This functions sets the date for TestDrive instance when it is called
		/// @param Date 
 
		void setDate(std::string date);
		///@brief Returns Date from TestDrive object 
		///
		/// This functions return the Date when it is called 
		/// @return TestId
		string getDate()const;
		///@brief Sets Comment 
		///
		/// This functions sets the comment for TestDrive instance when it is called
		/// @param notes from customer  
		void setComment(std::string comm);
		///@brief Returns Note from TestDrive object 
		///
		/// This functions return the comment that was made by the customer
		/// when it is called 
		/// @return Notes
		
		string getComment()const;

		crow::json::wvalue toJson() const;

};