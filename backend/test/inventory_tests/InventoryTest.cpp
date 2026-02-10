#include "gtest/gtest.h"
#include "../../src/modules/inventory/inventory_model.h"

// ===== Basic Set/Get =====
TEST(InventoryTests, SetAndGetVin) {
    Vehicle v;
    v.setVin("VIN123");
    ASSERT_EQ(v.getVin(), "VIN123");
}

TEST(InventoryTests, SetAndGetMake) {
    Vehicle v;
    v.setMake("Toyota");
    ASSERT_EQ(v.getMake(), "Toyota");
}

// ===== Year =====
TEST(InventoryTests, ValidYear) {
    Vehicle v;
    ASSERT_TRUE(v.isValidYear(2024));
}

TEST(InventoryTests, InvalidYear) {
    Vehicle v;
    ASSERT_FALSE(v.isValidYear(1700));
}

// ===== Market Price =====
TEST(InventoryTests, ValidMarketPrice) {
    Vehicle v;
    ASSERT_TRUE(v.isValidMarketPrice(25000.50));
}

TEST(InventoryTests, InvalidMarketPriceZero) {
    Vehicle v;
    ASSERT_FALSE(v.isValidMarketPrice(0));
}

TEST(InventoryTests, InvalidMarketPriceNegative) {
    Vehicle v;
    ASSERT_FALSE(v.isValidMarketPrice(-100));
}

// ===== Odometer =====
TEST(InventoryTests, ValidOdometer) {
    Vehicle v;
    ASSERT_TRUE(v.isValidOdometer(50000));
}

TEST(InventoryTests, ZeroOdometerValid) {
    Vehicle v;
    ASSERT_TRUE(v.isValidOdometer(0));
}

TEST(InventoryTests, InvalidOdometer) {
    Vehicle v;
    ASSERT_FALSE(v.isValidOdometer(-5));
}

// ===== Fuel Type =====
TEST(InventoryTests, ValidFuelTypeGas) {
    Vehicle v;
    ASSERT_TRUE(v.isValidFuelType("GAS"));
}

TEST(InventoryTests, ValidFuelTypeElectric) {
    Vehicle v;
    ASSERT_TRUE(v.isValidFuelType("ELECTRIC"));
}

TEST(InventoryTests, InvalidFuelType) {
    Vehicle v;
    ASSERT_FALSE(v.isValidFuelType("WATER"));
}

// ===== Transmission =====
TEST(InventoryTests, ValidTransmissionAutomatic) {
    Vehicle v;
    ASSERT_TRUE(v.isValidTransmission("AUTOMATIC"));
}

TEST(InventoryTests, ValidTransmissionManual) {
    Vehicle v;
    ASSERT_TRUE(v.isValidTransmission("MANUAL"));
}

TEST(InventoryTests, InvalidTransmission) {
    Vehicle v;
    ASSERT_FALSE(v.isValidTransmission("CVT"));
}

// ===== Trim =====
TEST(InventoryTests, ValidTrimProvided) {
    Vehicle v;
    ASSERT_TRUE(v.isValidTrim("XLE"));
}

TEST(InventoryTests, ValidTrimEmpty) {
    Vehicle v;
    ASSERT_TRUE(v.isValidTrim(""));
}

// ===== Status =====
TEST(InventoryTests, ValidStatusAvailable) {
    Vehicle v;
    ASSERT_TRUE(v.isValidStatus("AVAILABLE"));
}

TEST(InventoryTests, InvalidStatus) {
    Vehicle v;
    ASSERT_FALSE(v.isValidStatus("IN_REPAIR"));
}
