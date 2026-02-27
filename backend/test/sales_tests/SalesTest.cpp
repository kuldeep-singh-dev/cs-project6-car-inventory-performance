#include <gtest/gtest.h>
#include <pqxx/pqxx>
#include <string>
#include <memory>
#include "../../src/db/db_connection.h"

// ========================================
// TEST HELPERS
// ========================================

class SalesTestHelper {
public:
    static std::string createTestVehicle(pqxx::connection& conn) {
        pqxx::work txn(conn);
        pqxx::result r = txn.exec(
            "INSERT INTO Vehicles (vin, make, model, year, odometer, fuel_type, transmission, market_price, status) "
            "VALUES ('TEST_VIN_' || FLOOR(RANDOM() * 1000000)::TEXT, 'Toyota', 'Camry', 2020, 50000, 'Gasoline', 'Automatic', 25000.00, 'Available') "
            "RETURNING id"
        );
        txn.commit();
        return r[0]["id"].c_str();
    }

    static std::string createTestCustomer(pqxx::connection& conn) {
        pqxx::work txn(conn);
        std::string unique_suffix = std::to_string(std::time(nullptr)) + std::to_string(rand() % 10000);
        std::string unique_email = "test_" + unique_suffix + "@test.com";
        std::string unique_license = "TEST-" + unique_suffix;
        pqxx::result r = txn.exec_params(
            "INSERT INTO Customers (first_name, last_name, ph_number, email, driving_licence) "
            "VALUES ('Test', 'User', '4165551234', $1, $2) RETURNING id",
            unique_email, unique_license);
        txn.commit();
        return r[0]["id"].c_str();
    }

    static std::string createTestSale(pqxx::connection& conn, const std::string& vehicle_id, const std::string& customer_id, double sale_price = 99999.99) {
        pqxx::work txn(conn);
        pqxx::result r = txn.exec_params(
            "INSERT INTO Sales (vehicle_id, customer_id, date, sale_price) "
            "VALUES ($1, $2, CURRENT_DATE, $3) RETURNING id",
            vehicle_id, customer_id, sale_price);
        txn.commit();
        return r[0]["id"].c_str();
    }

    static void deleteTestSale(pqxx::connection& conn, const std::string& sale_id) {
        pqxx::work txn(conn);
        txn.exec_params("DELETE FROM Sales WHERE id = $1", sale_id);
        txn.commit();
    }

    static void deleteTestVehicle(pqxx::connection& conn, const std::string& vehicle_id) {
        pqxx::work txn(conn);
        txn.exec_params("DELETE FROM Vehicles WHERE id = $1", vehicle_id);
        txn.commit();
    }

    static void deleteTestCustomer(pqxx::connection& conn, const std::string& customer_id) {
        pqxx::work txn(conn);
        txn.exec_params("DELETE FROM Customers WHERE id = $1", customer_id);
        txn.commit();
    }
};


// ========================================
// TEST FIXTURE
// ========================================

class SalesTest : public ::testing::Test {
    protected:
        std::unique_ptr<ConnectionGuard> guard_;  
        std::string test_vehicle_id;
        std::string test_customer_id;
        std::string test_sale_id;

        // Convenience accessor so all the test bodies below stay unchanged
        pqxx::connection& conn() { return guard_->get(); }

        void SetUp() override {
            guard_ = std::make_unique<ConnectionGuard>(getPool());
            ASSERT_TRUE(guard_->get().is_open());
            test_vehicle_id  = SalesTestHelper::createTestVehicle(conn());
            test_customer_id = SalesTestHelper::createTestCustomer(conn());
        }

        void TearDown() override {
            try {
                if (!test_sale_id.empty())    SalesTestHelper::deleteTestSale(conn(),     test_sale_id);
                if (!test_vehicle_id.empty()) SalesTestHelper::deleteTestVehicle(conn(),  test_vehicle_id);
                if (!test_customer_id.empty())SalesTestHelper::deleteTestCustomer(conn(), test_customer_id);
            } catch (...) {}
            guard_.reset(); // return connection to pool
        }
    };


// ========================================
// DATABASE CONNECTION TESTS
// ========================================

TEST(DatabaseTest, CanConnectToDatabase) {
    ConnectionGuard guard(getPool());
    EXPECT_TRUE(guard.get().is_open());
}

TEST(DatabaseTest, CanExecuteSimpleQuery) {
    ConnectionGuard guard(getPool());
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec("SELECT 1 AS test_value");
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]["test_value"].as<int>(), 1);
}

// ========================================
// CREATE SALE TESTS (POST /sales)
// ========================================

TEST_F(SalesTest, CreateSale_Success) {
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "INSERT INTO Sales (vehicle_id, customer_id, date, sale_price) "
        "VALUES ($1, $2, '2026-02-05', 28500.00) "
        "RETURNING id, vehicle_id, customer_id, date, sale_price",
        test_vehicle_id, test_customer_id
    );
    
    txn.commit();
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]["vehicle_id"].c_str(), test_vehicle_id);
    EXPECT_EQ(r[0]["customer_id"].c_str(), test_customer_id);
    EXPECT_DOUBLE_EQ(r[0]["sale_price"].as<double>(), 28500.00);
    EXPECT_EQ(r[0]["date"].c_str(), std::string("2026-02-05"));
    
    // Save for cleanup
    test_sale_id = r[0]["id"].c_str();
}

TEST_F(SalesTest, CreateSale_InvalidVehicleId) {
    pqxx::work txn(conn());
    
    EXPECT_THROW({
        txn.exec_params(
            "INSERT INTO Sales (vehicle_id, customer_id, date, sale_price) "
            "VALUES ('00000000-0000-0000-0000-000000000000', $1, '2026-02-05', 28500.00)",
            test_customer_id
        );
    }, pqxx::foreign_key_violation);
}

TEST_F(SalesTest, CreateSale_InvalidCustomerId) {
    pqxx::work txn(conn());
    
    EXPECT_THROW({
        txn.exec_params(
            "INSERT INTO Sales (vehicle_id, customer_id, date, sale_price) "
            "VALUES ($1, '00000000-0000-0000-0000-000000000000', '2026-02-05', 28500.00)",
            test_vehicle_id
        );
    }, pqxx::foreign_key_violation);
}

TEST_F(SalesTest, CreateSale_ZeroPrice) {
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "INSERT INTO Sales (vehicle_id, customer_id, date, sale_price) "
        "VALUES ($1, $2, '2026-02-05', 0.01) "
        "RETURNING id, sale_price",
        test_vehicle_id, test_customer_id
    );
    
    txn.commit();
    
    EXPECT_DOUBLE_EQ(r[0]["sale_price"].as<double>(), 0.01);
    
    test_sale_id = r[0]["id"].c_str();
}

// ========================================
// READ SALES TESTS (GET /sales)
// ========================================

TEST_F(SalesTest, GetAllSales_ReturnsResults) {
    // Create a test sale first
    test_sale_id = SalesTestHelper::createTestSale(conn(), test_vehicle_id, test_customer_id);
    
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec(
        "SELECT s.id AS sale_id, s.date, s.sale_price, "
        "v.make, v.model, "
        "c.first_name, c.last_name "
        "FROM Sales s "
        "JOIN Vehicles v ON s.vehicle_id = v.id "
        "JOIN Customers c ON s.customer_id = c.id "
        "ORDER BY s.date DESC"
    );
    
    EXPECT_GE(r.size(), 1);
    
    // Find our test sale
    bool found = false;
    for (const auto& row : r) {
        if (std::string(row["sale_id"].c_str()) == test_sale_id) {
            found = true;
            EXPECT_DOUBLE_EQ(row["sale_price"].as<double>(), 99999.99);
            EXPECT_EQ(row["make"].c_str(), std::string("Toyota"));
            EXPECT_EQ(row["first_name"].c_str(), std::string("Test"));
            break;
        }
    }
    EXPECT_TRUE(found) << "Test sale not found in results";
}

// ========================================
// GET SALE BY ID TESTS (GET /sales/id/<id>)
// ========================================

TEST_F(SalesTest, GetSaleById_ValidId) {
    test_sale_id = SalesTestHelper::createTestSale(conn(), test_vehicle_id, test_customer_id, 28500.00);
    
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "SELECT "
        "  s.id AS sale_id, "
        "  s.date AS sale_date, "
        "  s.sale_price, "
        "  v.make, "
        "  v.model, "
        "  v.year, "
        "  v.market_price, "
        "  c.first_name, "
        "  c.last_name "
        "FROM Sales s "
        "JOIN Vehicles v ON s.vehicle_id = v.id "
        "JOIN Customers c ON s.customer_id = c.id "
        "WHERE s.id = $1",
        test_sale_id
    );
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]["sale_id"].c_str(), test_sale_id);
    EXPECT_DOUBLE_EQ(r[0]["sale_price"].as<double>(), 28500.00);
    EXPECT_EQ(r[0]["make"].c_str(), std::string("Toyota"));
}

TEST_F(SalesTest, GetSaleById_InvalidId) {
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "SELECT * FROM Sales WHERE id = $1",
        "00000000-0000-0000-0000-000000000000"
    );
    
    EXPECT_EQ(r.size(), 0);
}

// ========================================
// UPDATE SALE TESTS (PUT /sales/<id>)
// ========================================

TEST_F(SalesTest, UpdateSale_Price) {
    test_sale_id = SalesTestHelper::createTestSale(conn(), test_vehicle_id, test_customer_id);
    
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "UPDATE Sales SET sale_price = $1 WHERE id = $2 "
        "RETURNING sale_price",
        88888.88, test_sale_id
    );
    
    txn.commit();
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_DOUBLE_EQ(r[0]["sale_price"].as<double>(), 88888.88);
}

TEST_F(SalesTest, UpdateSale_Date) {
    test_sale_id = SalesTestHelper::createTestSale(conn(), test_vehicle_id, test_customer_id);
    
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "UPDATE Sales SET date = $1 WHERE id = $2 "
        "RETURNING date",
        "2026-01-15", test_sale_id
    );
    
    txn.commit();
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]["date"].c_str(), std::string("2026-01-15"));
}

TEST_F(SalesTest, UpdateSale_BothPriceAndDate) {
    test_sale_id = SalesTestHelper::createTestSale(conn(), test_vehicle_id, test_customer_id);
    
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "UPDATE Sales SET sale_price = $1, date = $2 WHERE id = $3 "
        "RETURNING sale_price, date",
        77777.77, "2026-01-20", test_sale_id
    );
    
    txn.commit();
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_DOUBLE_EQ(r[0]["sale_price"].as<double>(), 77777.77);
    EXPECT_EQ(r[0]["date"].c_str(), std::string("2026-01-20"));
}

TEST_F(SalesTest, UpdateSale_NonExistentId) {
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "UPDATE Sales SET sale_price = $1 WHERE id = $2 "
        "RETURNING id",
        50000.00, "00000000-0000-0000-0000-000000000000"
    );
    
    EXPECT_EQ(r.size(), 0);
}

// ========================================
// GET SALES BY VEHICLE (GET /sales/vehicles/<id>)
// ========================================

TEST_F(SalesTest, GetSalesByVehicle_HasSales) {
    test_sale_id = SalesTestHelper::createTestSale(conn(), test_vehicle_id, test_customer_id);
    
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "SELECT s.id AS sale_id, s.date, s.sale_price "
        "FROM Sales s "
        "WHERE s.vehicle_id = $1",
        test_vehicle_id
    );
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]["sale_id"].c_str(), test_sale_id);
}

TEST_F(SalesTest, GetSalesByVehicle_NoSales) {
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "SELECT s.id AS sale_id FROM Sales s WHERE s.vehicle_id = $1",
        test_vehicle_id
    );
    
    EXPECT_EQ(r.size(), 0);
}

// ========================================
// GET SALES BY CUSTOMER (GET /sales/customers/<id>)
// ========================================

TEST_F(SalesTest, GetSalesByCustomer_HasSales) {
    test_sale_id = SalesTestHelper::createTestSale(conn(), test_vehicle_id, test_customer_id);
    
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "SELECT s.id AS sale_id, s.sale_price "
        "FROM Sales s "
        "WHERE s.customer_id = $1",
        test_customer_id
    );
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]["sale_id"].c_str(), test_sale_id);
}

TEST_F(SalesTest, GetSalesByCustomer_NoSales) {
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "SELECT s.id AS sale_id FROM Sales s WHERE s.customer_id = $1",
        test_customer_id
    );
    
    EXPECT_EQ(r.size(), 0);
}

// ========================================
// INVOICE CALCULATION TESTS
// ========================================

TEST(InvoiceCalculationsTest, TaxCalculation) {
    double base_price = 28500.00;
    double sales_tax_rate = 0.13;
    double sales_tax = base_price * sales_tax_rate;
    
    EXPECT_DOUBLE_EQ(sales_tax, 3705.00);
}

TEST(InvoiceCalculationsTest, TotalWithFees) {
    double base_price = 28500.00;
    double documentation_fee = 500.00;
    double registration_fee = 120.00;
    double total_before_tax = base_price + documentation_fee + registration_fee;
    
    EXPECT_DOUBLE_EQ(total_before_tax, 29120.00);
}

TEST(InvoiceCalculationsTest, FinalTotal) {
    double total_before_tax = 29120.00;
    double sales_tax = 3785.60;
    double total_amount = total_before_tax + sales_tax;
    
    EXPECT_NEAR(total_amount, 32905.60, 0.01);
}

TEST(InvoiceCalculationsTest, ProfitCalculation) {
    double sale_price = 28500.00;
    double market_price = 27000.00;
    double profit = sale_price - market_price;
    double profit_percentage = (profit / market_price) * 100;
    
    EXPECT_DOUBLE_EQ(profit, 1500.00);
    EXPECT_NEAR(profit_percentage, 5.56, 0.01);
}

// ========================================
// EDGE CASES
// ========================================

TEST_F(SalesTest, EdgeCase_VeryLargePrice) {
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "INSERT INTO Sales (vehicle_id, customer_id, date, sale_price) "
        "VALUES ($1, $2, CURRENT_DATE, 9999999.99) "
        "RETURNING id AS sale_id, sale_price",
        test_vehicle_id, test_customer_id
    );
    
    txn.commit();
    
    EXPECT_NEAR(r[0]["sale_price"].as<double>(), 9999999.99, 0.01);
    
    test_sale_id = r[0]["sale_id"].c_str();
}

TEST_F(SalesTest, EdgeCase_FutureDate) {
    pqxx::work txn(conn());
    
    pqxx::result r = txn.exec_params(
        "INSERT INTO Sales (vehicle_id, customer_id, date, sale_price) "
        "VALUES ($1, $2, '2027-12-31', 30000.00) "
        "RETURNING id, date",
        test_vehicle_id, test_customer_id
    );
    
    txn.commit();
    
    EXPECT_EQ(r[0]["date"].c_str(), std::string("2027-12-31"));
    
    test_sale_id = r[0]["id"].c_str();
}

// ========================================
// DATA VALIDATION TESTS
// ========================================

TEST_F(SalesTest, Validation_UUIDLength) {
    EXPECT_EQ(test_vehicle_id.length(), 36);
    EXPECT_EQ(test_customer_id.length(), 36);
}

TEST_F(SalesTest, Validation_UUIDFormat) {
    // UUID format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    EXPECT_EQ(test_vehicle_id[8], '-');
    EXPECT_EQ(test_vehicle_id[13], '-');
    EXPECT_EQ(test_vehicle_id[18], '-');
    EXPECT_EQ(test_vehicle_id[23], '-');
}

// ========================================
// MAIN (REQUIRED BY GTEST)
// ========================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}