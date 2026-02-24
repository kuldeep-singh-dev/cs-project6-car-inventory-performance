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
public:
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
