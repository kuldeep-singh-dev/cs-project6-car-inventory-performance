import { useEffect, useState } from "react";
import { testDriveService } from "../../services/testDriveService";
import type { TestDriveRev } from "../../types/testDrive";
import TestDriveTable from "../../components/TestDriveTable";
import { Navigate, useNavigate } from "react-router-dom";
import "./TestDrivePage.css";

const TestDrivePage = () => {
  const navigate = useNavigate();
  const [testDrives, setTestDrives] = useState<TestDriveRev[]>([]);
  const [loading, setLoading] = useState(true);

  const fetchTestDrives = async () => {
    setLoading(true);
    try {
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
  }, []);

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
        <button className="tdBtn">Export CSV</button>
        <button className="tdBtnPrimary"
            onClick={() => navigate("/testdrive/add")}>
            Add
        </button>
      </div>

      <div className="tdTableWrap">
        <TestDriveTable testDrives={testDrives} />
      </div>
    </div>
  );
};

export default TestDrivePage;