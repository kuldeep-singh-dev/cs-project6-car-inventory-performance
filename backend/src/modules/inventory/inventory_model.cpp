#include "inventory_model.h"

// Setters
void Vehicle::setId(const std::string& id) { this->id = id; }
void Vehicle::setVin(const std::string& vin) { this->vin = vin; }
void Vehicle::setMake(const std::string& make) { this->make = make; }
void Vehicle::setModel(const std::string& model) { this->model = model; }
void Vehicle::setYear(int year) { this->year = year; }
void Vehicle::setOdometer(int odometer) { this->odometer = odometer; }
void Vehicle::setMarketPrice(double price) { this->marketPrice = price; }
void Vehicle::setFuelType(const std::string& fuelType) { this->fuelType = fuelType; }
void Vehicle::setTransmission(const std::string& transmission) { this->transmission = transmission; }
void Vehicle::setTrim(const std::string& trim) { this->trim = trim; }
void Vehicle::setStatus(const std::string& status) { this->status = status; }

// Getters
std::string Vehicle::getId() const { return id; }
std::string Vehicle::getVin() const { return vin; }
std::string Vehicle::getMake() const { return make; }
std::string Vehicle::getModel() const { return model; }
int Vehicle::getYear() const { return year; }
int Vehicle::getOdometer() const { return odometer; }
double Vehicle::getMarketPrice() const { return marketPrice; }
std::string Vehicle::getFuelType() const { return fuelType; }
std::string Vehicle::getTransmission() const { return transmission; }
std::string Vehicle::getTrim() const { return trim; }
std::string Vehicle::getStatus() const { return status; }

// Validation rules
bool Vehicle::isValidYear(int year) const {
    return year >= 1886 && year <= 2100;
}

bool Vehicle::isValidOdometer(int odometer) const {
    return odometer >= 0;
}

bool Vehicle::isValidMarketPrice(double price) const {
    return price > 0;
}

bool Vehicle::isValidFuelType(const std::string& fuelType) const {
    return fuelType == "GAS" ||
           fuelType == "DIESEL" ||
           fuelType == "ELECTRIC" ||
           fuelType == "HYBRID";
}

bool Vehicle::isValidTransmission(const std::string& transmission) const {
    return transmission == "AUTOMATIC" ||
           transmission == "MANUAL";
}

bool Vehicle::isValidTrim(const std::string& trim) const {
    return trim.empty() || trim.length() > 0; // optional but not invalid
}

bool Vehicle::isValidStatus(const std::string& status) const {
    return status == "AVAILABLE" || status == "SOLD";
}
