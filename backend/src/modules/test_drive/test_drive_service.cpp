#include "test_drive_service.h"
#include "../../db/db_connection.h"

TestDriveService::TestDriveService(ConnectionGuard& g) : guard(g) {}

crow::json::wvalue TestDriveService::getAllTestDrives() {
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec(
        "SELECT t.id, c.first_name, c.last_name, v.make, v.model, t.date, t.comments "
        "FROM test_drive_record t "
        "JOIN customers c ON t.customer_id = c.id "
        "JOIN vehicles v ON t.vehicle_id = v.id"
    );
    crow::json::wvalue testDriveJson = crow::json::wvalue::list();
    int i = 0;
    for (const auto& row : r) {
        testDriveJson[i]["id"] = row["id"].c_str();
        testDriveJson[i]["firstName"] = row["first_name"].c_str();
        testDriveJson[i]["lastName"] = row["last_name"].c_str();
        testDriveJson[i]["make"] = row["make"].c_str();
        testDriveJson[i]["model"] = row["model"].c_str();
        testDriveJson[i]["date"] = row["date"].c_str();
        testDriveJson[i]["comment"] = row["comments"].c_str();
        i++;
    }
    return testDriveJson;
}

crow::json::wvalue TestDriveService::addTestDrive(const TestDrive& testDrive) {
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec_params(
        "INSERT INTO test_drive_record (customer_id, vehicle_id, date, comments) "
        "VALUES ($1, $2, $3, $4) RETURNING id, customer_id, vehicle_id, date",
        testDrive.getCustomerId(),
        testDrive.getVehicleId(),
        testDrive.getDate(),
        testDrive.getComment()
    );
    txn.commit();
    return getTestDriveByTestId(r[0]["id"].c_str());
}

crow::json::wvalue TestDriveService::updateTestDrive(const TestDrive& testDrive) {
    pqxx::work txn(guard.get());
    pqxx::result r;
    crow::json::wvalue result = crow::json::wvalue::list();

    if (!testDrive.getDate().empty() && !testDrive.getComment().empty()) {
        r = txn.exec_params(
            "UPDATE test_drive_record SET date=$1, comments=$2 WHERE id=$3 "
            "RETURNING id, customer_id, vehicle_id, date, comments",
            testDrive.getDate(), testDrive.getComment(), testDrive.getTestDriveId());
    } else if (!testDrive.getDate().empty()) {
        r = txn.exec_params(
            "UPDATE test_drive_record SET date=$1 WHERE id=$2 "
            "RETURNING id, customer_id, vehicle_id, date, comments",
            testDrive.getDate(), testDrive.getTestDriveId());
    } else if (!testDrive.getComment().empty()) {
        r = txn.exec_params(
            "UPDATE test_drive_record SET comments=$1 WHERE id=$2 "
            "RETURNING id, customer_id, vehicle_id, date, comments",
            testDrive.getComment(), testDrive.getTestDriveId());
    }
    txn.commit();

    if (r.empty()) return result;
    result["testDriveId"] = r[0]["id"].c_str();
    result["customerId"]  = r[0]["customer_id"].c_str();
    result["vehicleId"]   = r[0]["vehicle_id"].c_str();
    result["date"]        = r[0]["date"].c_str();
    result["comment"]     = r[0]["comments"].c_str();
    return result;
}

bool TestDriveService::testDriveExists(const TestDrive& testDrive) {
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec_params(
        "SELECT id FROM test_drive_record WHERE id = $1",
        testDrive.getTestDriveId());
    return r.size() > 0;
}

string TestDriveService::getVehicleStatusFromDataBase(const string vehicleId) {
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec_params(
        "SELECT status FROM vehicles WHERE id = $1", vehicleId);
    if (r.size() > 0) return r[0]["status"].c_str();
    return "";
}

crow::json::wvalue TestDriveService::getTestDriveByCustomerId(const string customerId) {
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec_params(
        "SELECT t.id, c.first_name, c.last_name, v.make, v.model, t.date, t.comments "
        "FROM test_drive_record t "
        "JOIN customers c ON t.customer_id = c.id "
        "JOIN vehicles v ON t.vehicle_id = v.id WHERE customer_id = $1",
        customerId);
    crow::json::wvalue result = crow::json::wvalue::list();
    if (r.empty()) return result;
    int i = 0;
    for (const auto& row : r) {
        result[i]["id"]        = row["id"].c_str();
        result[i]["firstName"] = row["first_name"].c_str();
        result[i]["lastName"]  = row["last_name"].c_str();
        result[i]["make"]      = row["make"].c_str();
        result[i]["model"]     = row["model"].c_str();
        result[i]["date"]      = row["date"].c_str();
        result[i]["comment"]   = row["comments"].c_str();
        i++;
    }
    return result;
}

crow::json::wvalue TestDriveService::getTestDriveByVehicleId(const string vehicleId) {
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec_params(
        "SELECT t.id, c.first_name, c.last_name, v.make, v.model, t.date, t.comments "
        "FROM test_drive_record t "
        "JOIN customers c ON t.customer_id = c.id "
        "JOIN vehicles v ON t.vehicle_id = v.id WHERE vehicle_id = $1",
        vehicleId);
    crow::json::wvalue result = crow::json::wvalue::list();
    if (r.empty()) return result;
    int i = 0;
    for (const auto& row : r) {
        result[i]["id"]        = row["id"].c_str();
        result[i]["firstName"] = row["first_name"].c_str();
        result[i]["lastName"]  = row["last_name"].c_str();
        result[i]["make"]      = row["make"].c_str();
        result[i]["model"]     = row["model"].c_str();
        result[i]["date"]      = row["date"].c_str();
        result[i]["comment"]   = row["comments"].c_str();
        i++;
    }
    return result;
}

crow::json::wvalue TestDriveService::getTestDriveByTestId(string testId) {
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec_params(
        "SELECT t.id, c.first_name, c.last_name, v.make, v.model, t.date, t.comments "
        "FROM test_drive_record t "
        "JOIN customers c ON t.customer_id = c.id "
        "JOIN vehicles v ON t.vehicle_id = v.id WHERE t.id = $1",
        testId);
    crow::json::wvalue result;
    if (r.empty()) return result;
    result["id"]        = r[0]["id"].c_str();
    result["firstName"] = r[0]["first_name"].c_str();
    result["lastName"]  = r[0]["last_name"].c_str();
    result["make"]      = r[0]["make"].c_str();
    result["model"]     = r[0]["model"].c_str();
    result["date"]      = r[0]["date"].c_str();
    result["comment"]   = r[0]["comments"].c_str();
    return result;
}

string TestDriveService::getAllTestDrivesCSV() {
    pqxx::work txn(guard.get());
    pqxx::result r = txn.exec(
        "SELECT t.id, c.first_name, c.last_name, v.make, v.model, t.date, t.comments "
        "FROM test_drive_record t "
        "JOIN customers c ON t.customer_id = c.id "
        "JOIN vehicles v ON t.vehicle_id = v.id"
    );
    std::ostringstream csv;
    csv << "id,firstName,lastName,make,model,date,comment\n";
    for (const auto& row : r) {
        csv << row["id"].c_str() << ","
            << row["first_name"].c_str() << ","
            << row["last_name"].c_str() << ","
            << row["make"].c_str() << ","
            << row["model"].c_str() << ","
            << row["date"].c_str() << ",";
        std::string comment = row["comments"].c_str();
        if (comment.find(',') != std::string::npos)
            csv << "\"" << comment << "\"";
        else
            csv << comment;
        csv << "\n";
    }
    return csv.str();
}