#pragma once
#include <string>

/// @class Vehicle
/// @brief Represents a vehicle in the inventory.
class Vehicle {
private:
    std::string id;           
    std::string vin;          
    std::string make;         
    std::string model;        
    int year;                 
    int odometer;             
    double marketPrice;       
    std::string fuelType;     
    std::string transmission; 
    std::string trim;         
    std::string status;       

public:
    /// @brief Sets the vehicle ID.
    /// @param id The unique identifier for the vehicle.
    void setId(const std::string& id);

    /// @brief Sets the vehicle VIN.
    /// @param vin The Vehicle Identification Number.
    void setVin(const std::string& vin);

    /// @brief Sets the vehicle make.
    /// @param make The manufacturer of the vehicle.
    void setMake(const std::string& make);

    /// @brief Sets the vehicle model.
    /// @param model The model of the vehicle.
    void setModel(const std::string& model);

    /// @brief Sets the year of manufacture.
    /// @param year The year of manufacture.
    void setYear(int year);

    /// @brief Sets the odometer reading.
    /// @param odometer The odometer value.
    void setOdometer(int odometer);

    /// @brief Sets the market price.
    /// @param price The market price of the vehicle.
    void setMarketPrice(double price);

    /// @brief Sets the fuel type.
    /// @param fuelType The type of fuel used.
    void setFuelType(const std::string& fuelType);

    /// @brief Sets the transmission type.
    /// @param transmission The transmission type.
    void setTransmission(const std::string& transmission);

    /// @brief Sets the trim level.
    /// @param trim The trim level.
    void setTrim(const std::string& trim);

    /// @brief Sets the vehicle status.
    /// @param status The status of the vehicle.
    void setStatus(const std::string& status);

    /// @brief Gets the vehicle ID.
    /// @return The unique identifier for the vehicle.
    std::string getId() const;

    /// @brief Gets the vehicle VIN.
    /// @return The Vehicle Identification Number.
    std::string getVin() const;

    /// @brief Gets the vehicle make.
    /// @return The manufacturer of the vehicle.
    std::string getMake() const;

    /// @brief Gets the vehicle model.
    /// @return The model of the vehicle.
    std::string getModel() const;

    /// @brief Gets the year of manufacture.
    /// @return The year of manufacture.
    int getYear() const;

    /// @brief Gets the odometer reading.
    /// @return The odometer value.
    int getOdometer() const;

    /// @brief Gets the market price.
    /// @return The market price of the vehicle.
    double getMarketPrice() const;

    /// @brief Gets the fuel type.
    /// @return The type of fuel used.
    std::string getFuelType() const;

    /// @brief Gets the transmission type.
    /// @return The transmission type.
    std::string getTransmission() const;

    /// @brief Gets the trim level.
    /// @return The trim level.
    std::string getTrim() const;

    /// @brief Gets the vehicle status.
    /// @return The status of the vehicle.
    std::string getStatus() const;

    /// @brief Validates the year value.
    /// @param year The year to validate.
    /// @return true if valid, false otherwise.
    bool isValidYear(int year) const;

    /// @brief Validates the odometer value.
    /// @param odometer The odometer value to validate.
    /// @return true if valid, false otherwise.
    bool isValidOdometer(int odometer) const;

    /// @brief Validates the market price.
    /// @param price The market price to validate.
    /// @return true if valid, false otherwise.
    bool isValidMarketPrice(double price) const;

    /// @brief Validates the fuel type.
    /// @param fuelType The fuel type to validate.
    /// @return true if valid, false otherwise.
    bool isValidFuelType(const std::string& fuelType) const;

    /// @brief Validates the transmission type.
    /// @param transmission The transmission type to validate.
    /// @return true if valid, false otherwise.
    bool isValidTransmission(const std::string& transmission) const;

    /// @brief Validates the trim level.
    /// @param trim The trim level to validate.
    /// @return true if valid, false otherwise.
    bool isValidTrim(const std::string& trim) const;

    /// @brief Validates the vehicle status.
    /// @param status The status to validate.
    /// @return true if valid, false otherwise.
    bool isValidStatus(const std::string& status) const;
};
