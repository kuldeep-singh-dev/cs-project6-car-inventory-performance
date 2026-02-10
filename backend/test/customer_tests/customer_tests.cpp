
#include "modules/customer/customer.h"
#include "db/db_connection.h"

#include <pqxx/pqxx>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    int g_failed = 0;
    int g_passed = 0;

    void expect(bool condition, const std::string &message)
    {
        if (!condition)
        {
            ++g_failed;
            std::cerr << "[FAIL] " << message << "\n";
        }
        else
        {
            ++g_passed;
            std::cout << "[PASS] " << message << "\n";
        }
    }

    template <typename Fn>
    bool throwsInvalidArg(Fn &&fn)
    {
        try
        {
            fn();
            return false;
        }
        catch (const std::invalid_argument &)
        {
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    template <typename Fn>
    bool throwsUniqueViolation(Fn &&fn)
    {
        try
        {
            fn();
            return false;
        }
        catch (const pqxx::unique_violation &)
        {
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

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

}

int main()
{
    auto connPtr = getDbConnection();
    pqxx::connection &conn = *connPtr;

    std::string suffix = uniqueSuffix();

    // Test: Create customer (happy path)
    Customer created;
    try
    {
        created = createCustomer(
            conn,
            "TestFirst",
            "TestLast",
            "5190000000",
            "test" + suffix + "@example.com",
            "DL-" + suffix,
            std::optional<std::string>("123 Test St"));
        expect(!created.id.empty(), "POST/createCustomer returns customer with id");
        expect(created.email.find("@") != std::string::npos, "Created customer has valid email");
    }
    catch (const std::exception &e)
    {
        expect(false, std::string("Create customer failed unexpectedly: ") + e.what());
        return 1;
    }

    // Test: GET by id returns the same customer
    {
        auto fetched = getCustomerById(conn, created.id);
        expect(fetched.has_value(), "GET/getCustomerById returns a value for existing id");
        if (fetched)
        {
            expect(fetched->email == created.email, "Fetched customer matches created email");
            expect(fetched->driving_licence == created.driving_licence, "Fetched customer matches driving_licence");
        }
    }

    // Test: GET all includes created customer
    {
        auto all = getAllCustomers(conn);
        bool found = false;
        for (const auto &c : all)
        {
            if (c.id == created.id)
            {
                found = true;
                break;
            }
        }
        expect(found, "GET/getAllCustomers includes newly created customer");
    }

    // Test: POST missing required fields
    {
        bool ok = throwsInvalidArg([&]()
                                   { (void)createCustomer(conn, "", "Last", "123", "x" + suffix + "@example.com", "DL-X" + suffix, std::nullopt); });
        expect(ok, "POST/createCustomer throws invalid_argument when required fields missing");
    }

    // Test: POST invalid email format
    {
        bool ok = throwsInvalidArg([&]()
                                   { (void)createCustomer(conn, "A", "B", "123", "not-an-email", "DL-Y" + suffix, std::nullopt); });
        expect(ok, "POST/createCustomer throws invalid_argument for invalid email format");
    }

    // Test: POST duplicates (email and driving_licence)
    {
        bool ok = throwsUniqueViolation([&]()
                                        { (void)createCustomer(
                                              conn,
                                              "Dup",
                                              "Email",
                                              "123",
                                              created.email,      // duplicate email
                                              "DL-DUP-" + suffix, // unique DL
                                              std::nullopt); });
        expect(ok, "POST/createCustomer throws unique_violation for duplicate email");
    }

    {
        bool ok = throwsUniqueViolation([&]()
                                        { (void)createCustomer(
                                              conn,
                                              "Dup",
                                              "DL",
                                              "123",
                                              "other" + suffix + "@example.com", // unique email
                                              created.driving_licence,           // duplicate licence
                                              std::nullopt); });
        expect(ok, "POST/createCustomer throws unique_violation for duplicate driving_licence");
    }

    // Test: PATCH update phone/address (happy path)
    Customer updated;
    try
    {
        updated = patchCustomer(
            conn,
            created.id,
            std::nullopt,
            std::nullopt,
            std::optional<std::string>("5191111111"),
            std::nullopt,
            std::nullopt,
            std::optional<std::string>("999 Updated Ave"));
        expect(updated.ph_number == "5191111111", "PATCH/patchCustomer updates ph_number");
        expect(updated.address == "999 Updated Ave", "PATCH/patchCustomer updates address");
    }
    catch (const std::exception &e)
    {
        expect(false, std::string("PATCH update failed unexpectedly: ") + e.what());
    }

    // Test: PATCH invalid email format
    {
        bool ok = throwsInvalidArg([&]()
                                   { (void)patchCustomer(
                                         conn,
                                         created.id,
                                         std::nullopt,
                                         std::nullopt,
                                         std::nullopt,
                                         std::optional<std::string>("bademail"),
                                         std::nullopt,
                                         std::nullopt); });
        expect(ok, "PATCH/patchCustomer throws invalid_argument for invalid email");
    }

    // Test: PATCH no fields provided
    {
        bool ok = throwsInvalidArg([&]()
                                   { (void)patchCustomer(conn, created.id, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt); });
        expect(ok, "PATCH/patchCustomer throws invalid_argument when no fields provided");
    }

    // Test: PATCH uniqueness conflict (email)
    std::string other_id;
    {
        // create another customer
        Customer other = createCustomer(
            conn,
            "Other",
            "Customer",
            "5192222222",
            "other" + suffix + "2@example.com",
            "DL-OTHER-" + suffix,
            std::nullopt);
        other_id = other.id;

        bool ok = throwsUniqueViolation([&]()
                                        { (void)patchCustomer(
                                              conn,
                                              other_id,
                                              std::nullopt,
                                              std::nullopt,
                                              std::nullopt,
                                              std::optional<std::string>(updated.email), // set to existing email
                                              std::nullopt,
                                              std::nullopt); });
        expect(ok, "PATCH/patchCustomer throws unique_violation when updating to existing email");
    }

    // Cleanup
    deleteCustomerById(conn, created.id);
    if (!other_id.empty())
        deleteCustomerById(conn, other_id);

    std::cout << "\nCustomer tests completed. Passed: " << g_passed << ", Failed: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
