#pragma once
#include <string>

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
    // Setters
    void setId(const std::string& id);
    void setVin(const std::string& vin);
    void setMake(const std::string& make);
    void setModel(const std::string& model);
    void setYear(int year);
    void setOdometer(int odometer);
    void setMarketPrice(double price);
    void setFuelType(const std::string& fuelType);
    void setTransmission(const std::string& transmission);
    void setTrim(const std::string& trim);
    void setStatus(const std::string& status);

    // Getters
    std::string getId() const;
    std::string getVin() const;
    std::string getMake() const;
    std::string getModel() const;
    int getYear() const;
    int getOdometer() const;
    double getMarketPrice() const;
    std::string getFuelType() const;
    std::string getTransmission() const;
    std::string getTrim() const;
    std::string getStatus() const;

    // Validation
    bool isValidYear(int year) const;
    bool isValidOdometer(int odometer) const;
    bool isValidMarketPrice(double price) const;
    bool isValidFuelType(const std::string& fuelType) const;
    bool isValidTransmission(const std::string& transmission) const;
    bool isValidTrim(const std::string& trim) const;
    bool isValidStatus(const std::string& status) const;
};
