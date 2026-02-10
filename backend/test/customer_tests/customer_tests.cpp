#include <gtest/gtest.h>

#include "db/db_connection.h"
#include "modules/customer/customer.h"

#include <pqxx/pqxx>

#include <chrono>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace
{

    std::string uniqueSuffix()
    {
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::ostringstream oss;
        oss << now;
        return oss.str();
    }

    void deleteCustomerById(pqxx::connection &conn, const std::string &id)
    {
        pqxx::work txn(conn);
        txn.exec_params("DELETE FROM Customers WHERE id = $1", id);
        txn.commit();
    }

    class CustomerDbFixture : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            conn_ = getDbConnection();
            suffix_ = uniqueSuffix();
        }

        void TearDown() override
        {
            if (!conn_)
                return;
            for (const auto &id : created_ids_)
            {
                try
                {
                    deleteCustomerById(*conn_, id);
                }
                catch (...)
                {
                    // Best-effort cleanup; avoid masking test failures.
                }
            }
            created_ids_.clear();
        }

        pqxx::connection &conn() { return *conn_; }

        Customer makeCustomer(const std::string &first = "TestFirst",
                              const std::string &last = "TestLast",
                              const std::string &phone = "5190000000",
                              const std::optional<std::string> &address = std::optional<std::string>("123 Test St"))
        {
            Customer c = createCustomer(
                conn(),
                first,
                last,
                phone,
                "test" + suffix_ + "@example.com",
                "DL-" + suffix_,
                address);
            created_ids_.push_back(c.id);
            return c;
        }

        std::shared_ptr<pqxx::connection> conn_;
        std::string suffix_;
        std::vector<std::string> created_ids_;
    };

}

// Tests that creating a customer with valid data succeeds and returns a customer with an ID 
// and the expected email format.
TEST_F(CustomerDbFixture, CreateCustomer_HappyPath)
{
    Customer created = makeCustomer();
    EXPECT_FALSE(created.id.empty());
    EXPECT_NE(created.email.find('@'), std::string::npos);
}

// Tests that getCustomerById returns the correct customer
TEST_F(CustomerDbFixture, GetCustomerById_ReturnsCreatedCustomer)
{
    Customer created = makeCustomer();
    auto fetched = getCustomerById(conn(), created.id);
    ASSERT_TRUE(fetched.has_value());
    EXPECT_EQ(fetched->email, created.email);
    EXPECT_EQ(fetched->driving_licence, created.driving_licence);
}

// Tests that getAllCustomers includes a created customer
TEST_F(CustomerDbFixture, GetAllCustomers_IncludesCreatedCustomer)
{
    Customer created = makeCustomer();
    auto all = getAllCustomers(conn());
    bool found = false;
    for (const auto &c : all)
    {
        if (c.id == created.id)
        {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

// Tests that creating a customer with missing required fields throws an exception
TEST_F(CustomerDbFixture, CreateCustomer_MissingRequiredFields_Throws)
{
    EXPECT_THROW(
        (void)createCustomer(conn(), "", "Last", "123", "x" + suffix_ + "@example.com", "DL-X" + suffix_, std::nullopt),
        std::invalid_argument);
}

// Tests invalid email format
TEST_F(CustomerDbFixture, CreateCustomer_InvalidEmail_Throws)
{
    EXPECT_THROW(
        (void)createCustomer(conn(), "A", "B", "123", "not-an-email", "DL-Y" + suffix_, std::nullopt),
        std::invalid_argument);
}

// Tests uniqueness constraint on email
TEST_F(CustomerDbFixture, CreateCustomer_DuplicateEmail_ThrowsUniqueViolation)
{
    Customer created = makeCustomer();

    EXPECT_THROW(
        (void)createCustomer(conn(), "Dup", "Email", "123", created.email, "DL-DUP-" + suffix_, std::nullopt),
        pqxx::unique_violation);
}

// Tests uniqueness constraint on driving licence
TEST_F(CustomerDbFixture, CreateCustomer_DuplicateDrivingLicence_ThrowsUniqueViolation)
{
    Customer created = makeCustomer();

    EXPECT_THROW(
        (void)createCustomer(conn(), "Dup", "DL", "123", "other" + suffix_ + "@example.com", created.driving_licence, std::nullopt),
        pqxx::unique_violation);
}

// Tests patching the phone number and address of an existing customer
TEST_F(CustomerDbFixture, PatchCustomer_UpdatesPhoneAndAddress)
{
    Customer created = makeCustomer();

    Customer updated = patchCustomer(
        conn(),
        created.id,
        std::nullopt,
        std::nullopt,
        std::optional<std::string>("5191111111"),
        std::nullopt,
        std::nullopt,
        std::optional<std::string>("999 Updated Ave"));

    EXPECT_EQ(updated.ph_number, "5191111111");
    EXPECT_EQ(updated.address, "999 Updated Ave");
}

// Tests patch with invalid email format
TEST_F(CustomerDbFixture, PatchCustomer_InvalidEmail_Throws)
{
    Customer created = makeCustomer();

    EXPECT_THROW(
        (void)patchCustomer(
            conn(),
            created.id,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::optional<std::string>("bademail"),
            std::nullopt,
            std::nullopt),
        std::invalid_argument);
}

// Tests patch with no fields provided
TEST_F(CustomerDbFixture, PatchCustomer_NoFieldsProvided_Throws)
{
    Customer created = makeCustomer();

    EXPECT_THROW(
        (void)patchCustomer(conn(), created.id, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt),
        std::invalid_argument);
}

// Tests uniqueness constraint on email during patch
TEST_F(CustomerDbFixture, PatchCustomer_UniquenessConflictOnEmail_ThrowsUniqueViolation)
{
    Customer c1 = makeCustomer();

    // Creates a second customer with unique email and licence
    std::string other_suffix = uniqueSuffix();
    Customer c2 = createCustomer(
        conn(),
        "Other",
        "Customer",
        "5192222222",
        "other" + other_suffix + "@example.com",
        "DL-OTHER-" + other_suffix,
        std::nullopt);
    created_ids_.push_back(c2.id);

    EXPECT_THROW(
        (void)patchCustomer(
            conn(),
            c2.id,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::optional<std::string>(c1.email),
            std::nullopt,
            std::nullopt),
        pqxx::unique_violation);
}