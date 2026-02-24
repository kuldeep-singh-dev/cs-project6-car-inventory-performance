#include <gtest/gtest.h>
#include <pqxx/pqxx>
#include <string>
#include <memory>
#include <fstream>
#include <filesystem>
#include "../../src/db/db_connection.h"

namespace fs = std::filesystem;

// ========================================
// TEST HELPERS
// ========================================

class ImagesTestHelper {
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

    static std::string createTestImage(pqxx::connection& conn, const std::string& vehicle_id, const std::string& img_url = "https://images.unsplash.com/photo-1560958089-b8a1929cea89?w=800") {
        pqxx::work txn(conn);
        pqxx::result r = txn.exec_params(
            "INSERT INTO Images (vehicle_id, img_url) "
            "VALUES ($1, $2) "
            "RETURNING id",
            vehicle_id, img_url
        );
        txn.commit();
        return r[0]["id"].c_str();
    }

    static void deleteTestImage(pqxx::connection& conn, const std::string& image_id) {
        pqxx::work txn(conn);
        txn.exec_params("DELETE FROM Images WHERE id = $1", image_id);
        txn.commit();
    }

    static void deleteTestVehicle(pqxx::connection& conn, const std::string& vehicle_id) {
        pqxx::work txn(conn);
        txn.exec_params("DELETE FROM Vehicles WHERE id = $1", vehicle_id);
        txn.commit();
    }

    static void createTestImageFile(const std::string& filepath) {
        fs::create_directories(fs::path(filepath).parent_path());
        std::ofstream file(filepath, std::ios::binary);
        // Write some dummy binary data
        const unsigned char dummy_data[] = {0xFF, 0xD8, 0xFF, 0xE0}; // JPEG header
        file.write(reinterpret_cast<const char*>(dummy_data), sizeof(dummy_data));
        file.close();
    }

    static void deleteTestImageFile(const std::string& filepath) {
        if (fs::exists(filepath)) {
            fs::remove(filepath);
        }
    }
};

// ========================================
// TEST FIXTURE
// ========================================

class ImagesTest : public ::testing::Test {
protected:
    std::shared_ptr<pqxx::connection> conn;
    std::string test_vehicle_id;
    std::string test_image_id;

    void SetUp() override {
        conn = getDbConnection();
        ASSERT_NE(conn, nullptr);
        ASSERT_TRUE(conn->is_open());
        
        test_vehicle_id = ImagesTestHelper::createTestVehicle(*conn);
    }

    void TearDown() override {
        try {
            if (!test_image_id.empty()) {
                ImagesTestHelper::deleteTestImage(*conn, test_image_id);
            }
            if (!test_vehicle_id.empty()) {
                ImagesTestHelper::deleteTestVehicle(*conn, test_vehicle_id);
            }
        } catch (...) {
            // Ignore cleanup errors
        }
    }
};

// ========================================
// DATABASE CONNECTION TESTS
// ========================================

TEST(DatabaseTest, CanConnectToDatabase) {
    auto conn = getDbConnection();
    ASSERT_NE(conn, nullptr);
    EXPECT_TRUE(conn->is_open());
}

// ========================================
// CREATE IMAGE TESTS (POST /vehicles/<id>/images)
// ========================================

TEST_F(ImagesTest, CreateImage_Success) {
    pqxx::work txn(*conn);
    
    std::string img_url = "https://images.unsplash.com/photo-1560958089-b8a1929cea89?w=800";
    
    pqxx::result r = txn.exec_params(
        "INSERT INTO Images (vehicle_id, img_url) "
        "VALUES ($1, $2) "
        "RETURNING id, vehicle_id, img_url",
        test_vehicle_id, img_url
    );
    
    txn.commit();
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]["vehicle_id"].c_str(), test_vehicle_id);
    EXPECT_EQ(r[0]["img_url"].c_str(), img_url);
    
    test_image_id = r[0]["id"].c_str();
}

TEST_F(ImagesTest, CreateImage_LocalUpload) {
    pqxx::work txn(*conn);
    
    std::string img_url = "/uploads/" + test_vehicle_id + "_1234567890.jpg";
    
    pqxx::result r = txn.exec_params(
        "INSERT INTO Images (vehicle_id, img_url) "
        "VALUES ($1, $2) "
        "RETURNING id, img_url",
        test_vehicle_id, img_url
    );
    
    txn.commit();
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]["img_url"].c_str(), img_url);
    
    test_image_id = r[0]["id"].c_str();
}

TEST_F(ImagesTest, CreateImage_InvalidVehicleId) {
    pqxx::work txn(*conn);
    
    EXPECT_THROW({
        txn.exec_params(
            "INSERT INTO Images (vehicle_id, img_url) "
            "VALUES ('00000000-0000-0000-0000-000000000000', $1)",
            "https://example.com/image.jpg"
        );
    }, pqxx::foreign_key_violation);
}

TEST_F(ImagesTest, CreateImage_MultipleImagesForOneVehicle) {
    pqxx::work txn(*conn);
    
    pqxx::result r1 = txn.exec_params(
        "INSERT INTO Images (vehicle_id, img_url) "
        "VALUES ($1, 'https://example.com/image1.jpg') "
        "RETURNING id",
        test_vehicle_id
    );
    
    pqxx::result r2 = txn.exec_params(
        "INSERT INTO Images (vehicle_id, img_url) "
        "VALUES ($1, 'https://example.com/image2.jpg') "
        "RETURNING id",
        test_vehicle_id
    );
    
    txn.commit();
    
    EXPECT_NE(r1[0]["id"].c_str(), r2[0]["id"].c_str());
    
    // Cleanup both images
    ImagesTestHelper::deleteTestImage(*conn, r1[0]["id"].c_str());
    ImagesTestHelper::deleteTestImage(*conn, r2[0]["id"].c_str());
}

// ========================================
// READ IMAGE TESTS (GET /vehicles/<id>/images)
// ========================================

TEST_F(ImagesTest, GetImagesByVehicle_HasImages) {
    test_image_id = ImagesTestHelper::createTestImage(*conn, test_vehicle_id);
    
    pqxx::work txn(*conn);
    
    pqxx::result r = txn.exec_params(
        "SELECT id, vehicle_id, img_url FROM Images WHERE vehicle_id = $1 ORDER BY id",
        test_vehicle_id
    );
    
    ASSERT_GE(r.size(), 1);
    
    bool found = false;
    for (const auto& row : r) {
        if (std::string(row["id"].c_str()) == test_image_id) {
            found = true;
            EXPECT_EQ(row["vehicle_id"].c_str(), test_vehicle_id);
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(ImagesTest, GetImagesByVehicle_NoImages) {
    pqxx::work txn(*conn);
    
    pqxx::result r = txn.exec_params(
        "SELECT id FROM Images WHERE vehicle_id = $1",
        test_vehicle_id
    );
    
    EXPECT_EQ(r.size(), 0);
}

TEST_F(ImagesTest, GetImagesByVehicle_MultipleImages) {
    auto img1_id = ImagesTestHelper::createTestImage(*conn, test_vehicle_id, "https://example.com/img1.jpg");
    auto img2_id = ImagesTestHelper::createTestImage(*conn, test_vehicle_id, "https://example.com/img2.jpg");
    auto img3_id = ImagesTestHelper::createTestImage(*conn, test_vehicle_id, "https://example.com/img3.jpg");
    
    pqxx::work txn(*conn);
    
    pqxx::result r = txn.exec_params(
        "SELECT id, img_url FROM Images WHERE vehicle_id = $1 ORDER BY id",
        test_vehicle_id
    );
    
    txn.commit(); // Commit before cleanup
    
    EXPECT_EQ(r.size(), 3);
    
    // Cleanup
    ImagesTestHelper::deleteTestImage(*conn, img1_id);
    ImagesTestHelper::deleteTestImage(*conn, img2_id);
    ImagesTestHelper::deleteTestImage(*conn, img3_id);
}

// ========================================
// DELETE IMAGE TESTS (DELETE /images/<id>)
// ========================================

TEST_F(ImagesTest, DeleteImage_Success) {
    test_image_id = ImagesTestHelper::createTestImage(*conn, test_vehicle_id);
    
    pqxx::work txn(*conn);
    
    pqxx::result r = txn.exec_params(
        "DELETE FROM Images WHERE id = $1 RETURNING id",
        test_image_id
    );
    
    txn.commit();
    
    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]["id"].c_str(), test_image_id);
    
    // Verify deletion
    pqxx::work verify_txn(*conn);
    pqxx::result verify = verify_txn.exec_params(
        "SELECT id FROM Images WHERE id = $1",
        test_image_id
    );
    
    EXPECT_EQ(verify.size(), 0);
    
    test_image_id.clear(); // Already deleted
}

TEST_F(ImagesTest, DeleteImage_InvalidId) {
    pqxx::work txn(*conn);
    
    pqxx::result r = txn.exec_params(
        "DELETE FROM Images WHERE id = $1 RETURNING id",
        "00000000-0000-0000-0000-000000000000"
    );
    
    EXPECT_EQ(r.size(), 0);
}

TEST_F(ImagesTest, DeleteImage_CascadeOnVehicleDelete) {
    test_image_id = ImagesTestHelper::createTestImage(*conn, test_vehicle_id);
    
    // Delete the vehicle (should cascade to images)
    pqxx::work txn(*conn);
    txn.exec_params("DELETE FROM Vehicles WHERE id = $1", test_vehicle_id);
    txn.commit();
    
    // Verify image was also deleted
    pqxx::work verify_txn(*conn);
    pqxx::result r = verify_txn.exec_params(
        "SELECT id FROM Images WHERE id = $1",
        test_image_id
    );
    
    EXPECT_EQ(r.size(), 0);
    
    test_vehicle_id.clear();
    test_image_id.clear();
}

// ========================================
// FILE OPERATIONS TESTS
// ========================================

TEST(ImageFileTest, CreateImageFile_Success) {
    std::string filepath = "/tmp/test_image_12345.jpg";
    
    ImagesTestHelper::createTestImageFile(filepath);
    
    EXPECT_TRUE(fs::exists(filepath));
    EXPECT_GT(fs::file_size(filepath), 0);
    
    ImagesTestHelper::deleteTestImageFile(filepath);
}

TEST(ImageFileTest, DeleteImageFile_Success) {
    std::string filepath = "/tmp/test_image_67890.jpg";
    
    ImagesTestHelper::createTestImageFile(filepath);
    ASSERT_TRUE(fs::exists(filepath));
    
    ImagesTestHelper::deleteTestImageFile(filepath);
    
    EXPECT_FALSE(fs::exists(filepath));
}

TEST(ImageFileTest, FileExists_Check) {
    std::string filepath = "/tmp/nonexistent_image.jpg";
    
    EXPECT_FALSE(fs::exists(filepath));
}

// ========================================
// URL VALIDATION TESTS
// ========================================

TEST_F(ImagesTest, URLFormat_ExternalURL) {
    std::string url = "https://images.unsplash.com/photo-1560958089-b8a1929cea89?w=800";
    
    EXPECT_TRUE(url.find("https://") == 0 || url.find("http://") == 0);
}

TEST_F(ImagesTest, URLFormat_LocalURL) {
    std::string url = "/uploads/vehicle_123_456789.jpg";
    
    EXPECT_TRUE(url.find("/uploads/") == 0);
}

TEST_F(ImagesTest, URLFormat_LongURL) {
    std::string long_url(500, 'a');
    long_url = "https://example.com/" + long_url + ".jpg";
    
    pqxx::work txn(*conn);
    
    pqxx::result r = txn.exec_params(
        "INSERT INTO Images (vehicle_id, img_url) VALUES ($1, $2) RETURNING id",
        test_vehicle_id, long_url
    );
    
    txn.commit();
    
    EXPECT_EQ(r.size(), 1);
    
    test_image_id = r[0]["id"].c_str();
}

// ========================================
// EDGE CASES
// ========================================

TEST_F(ImagesTest, EdgeCase_EmptyURLString) {
    pqxx::work txn(*conn);
    
    // PostgreSQL should reject empty string due to NOT NULL
    pqxx::result r = txn.exec_params(
        "INSERT INTO Images (vehicle_id, img_url) VALUES ($1, '') RETURNING id",
        test_vehicle_id
    );
    
    txn.commit();
    
    // Should still insert, just with empty string
    EXPECT_EQ(r.size(), 1);
    
    test_image_id = r[0]["id"].c_str();
}

TEST_F(ImagesTest, EdgeCase_SpecialCharactersInURL) {
    std::string url = "https://example.com/image%20with%20spaces.jpg?param=value&other=123";
    
    pqxx::work txn(*conn);
    
    pqxx::result r = txn.exec_params(
        "INSERT INTO Images (vehicle_id, img_url) VALUES ($1, $2) RETURNING id, img_url",  // ← Added 'id'
        test_vehicle_id, url
    );
    
    txn.commit();
    
    EXPECT_EQ(r[0]["img_url"].c_str(), url);
    
    test_image_id = r[0]["id"].c_str();
}

// ========================================
// DATA VALIDATION TESTS
// ========================================

TEST_F(ImagesTest, Validation_UUIDLength) {
    test_image_id = ImagesTestHelper::createTestImage(*conn, test_vehicle_id);
    
    EXPECT_EQ(test_image_id.length(), 36);
    EXPECT_EQ(test_vehicle_id.length(), 36);
}

TEST_F(ImagesTest, Validation_UUIDFormat) {
    test_image_id = ImagesTestHelper::createTestImage(*conn, test_vehicle_id);
    
    // UUID format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    EXPECT_EQ(test_image_id[8], '-');
    EXPECT_EQ(test_image_id[13], '-');
    EXPECT_EQ(test_image_id[18], '-');
    EXPECT_EQ(test_image_id[23], '-');
}

// ========================================
// COUNT AND STATISTICS TESTS
// ========================================

TEST_F(ImagesTest, CountImages_PerVehicle) {
    // Create images first (these have their own transactions)
    auto img1 = ImagesTestHelper::createTestImage(*conn, test_vehicle_id);
    auto img2 = ImagesTestHelper::createTestImage(*conn, test_vehicle_id);
    auto img3 = ImagesTestHelper::createTestImage(*conn, test_vehicle_id);
    
    // Then start transaction for counting
    pqxx::work txn(*conn);
    
    pqxx::result r = txn.exec_params(
        "SELECT COUNT(*) as count FROM Images WHERE vehicle_id = $1",
        test_vehicle_id
    );
    
    txn.commit(); // Commit before cleanup
    
    EXPECT_EQ(r[0]["count"].as<int>(), 3);
    
    // Cleanup
    ImagesTestHelper::deleteTestImage(*conn, img1);
    ImagesTestHelper::deleteTestImage(*conn, img2);
    ImagesTestHelper::deleteTestImage(*conn, img3);
}

TEST_F(ImagesTest, GetTotalImageCount) {
    // Get initial count
    pqxx::work txn(*conn);
    pqxx::result r = txn.exec("SELECT COUNT(*) as total FROM Images");
    int initial_count = r[0]["total"].as<int>();
    txn.commit(); // Commit transaction before creating new image
    
    // Create new image (has its own transaction)
    test_image_id = ImagesTestHelper::createTestImage(*conn, test_vehicle_id);
    
    // Get new count
    pqxx::work txn2(*conn);
    pqxx::result r2 = txn2.exec("SELECT COUNT(*) as total FROM Images");
    txn2.commit();
    
    EXPECT_EQ(r2[0]["total"].as<int>(), initial_count + 1);
}

// ========================================
// MAIN (REQUIRED BY GTEST)
// ========================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}