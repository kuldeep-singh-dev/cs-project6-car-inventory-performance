import { useEffect, useState } from "react";
import { testDriveService } from "../../services/testDriveService";
import type { TestDriveRev } from "../../types/testDrive";
import TestDriveTable from "../../components/TestDriveTable";
import { useNavigate, useLocation } from "react-router-dom";
import "./TestDrivePage.css";

const TestDrivePage = () => {
  const navigate = useNavigate();
  const location = useLocation();

  const customerId = location.state?.customerId;
  const vehicleId = location.state?.vehicleId;

  const [testDrives, setTestDrives] = useState<TestDriveRev[]>([]);
  const [loading, setLoading] = useState(true);

  const fetchTestDrives = async () => {
    setLoading(true);
    try {
      if (customerId) {
        const data = await testDriveService.getByCustomerId(customerId);
        setTestDrives(data);
        return;
      }

      if (vehicleId) {
        const data = await testDriveService.getByVehicleId(vehicleId);
        setTestDrives(data);
        return;
      }

      const data = await testDriveService.getAll();
      setTestDrives(data);

    } catch (err) {
      console.error("Failed to fetch test drives:", err);
      setTestDrives([]);
    } finally {
      setLoading(false);
    }
  };
  
  useEffect(() => {
    fetchTestDrives();
  }, [customerId, vehicleId]);

  if (loading) return <p>Loading test drives...</p>;

  return (
    <div className="tdPage">
      <div className="tdHeader">
        <h1 className="tdTitle">Test Drive Records</h1>

        <div className="tdHeaderActions">
          <button onClick={() => navigate("/testdrive/customer")} className="tdBtnSmall">
            By Customer
          </button>

          <button onClick={() => navigate("/testdrive/vehicle")} className="tdBtnSmall">
            By Vehicle
          </button>
        </div>
      </div>

      <div className="tdActions">
        <button className="tdBtn"
        onClick={() => {
        window.location.href = "/api/testdrive/export/csv";
        }}

        >Export CSV</button>
        <button className="tdBtnPrimary" onClick={() => navigate("/testdrive/add")}>
          Add
        </button>
      </div>

        <div className="tdTableWrap">
        {testDrives.length === 0 ? (
            <p className="tdNoResults">No test drives found.</p>
        ) : (
        <TestDriveTable testDrives={testDrives} />
        )}
</div>

    </div>
  );
};

export default TestDrivePage;