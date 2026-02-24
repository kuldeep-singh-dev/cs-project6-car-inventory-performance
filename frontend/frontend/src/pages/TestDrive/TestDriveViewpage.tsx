import { useEffect, useState } from "react";
import { useNavigate, useParams } from "react-router-dom";
import { testDriveService } from "../../services/testDriveService";
import type { TestDriveRev } from "../../types/testDrive";

import TestDriveView from "../../components/TestDriveView";
import "./TestDriveViewPage.css";

const TestDriveViewPage = () => {
  const navigate = useNavigate();
  const { id } = useParams<{ id: string }>();

  const [testDrive, setTestDrive] = useState<TestDriveRev | null>(null);
  const [loading, setLoading] = useState(true);

  const fetchTestDrive = async () => {
    if (!id) return;

    setLoading(true);
    try {
      const data = await testDriveService.getById(id);
      setTestDrive(data);
    } catch (err) {
      console.error("Failed to fetch test drive:", err);
      setTestDrive(null);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchTestDrive();
  }, [id]);

  if (loading) return <p>Loading test drive...</p>;
  if (!testDrive) return <p>Test drive not found.</p>;

  return (
    <div className="tdViewPage">
      <h1 className="tdViewTitle">View Test Drive</h1>

      <div className="tdViewLayout">
        {/* LEFT COLUMN */}
        <div className="tdViewDetails">
          <TestDriveView testDrive={testDrive} />
        </div>

        {/* RIGHT COLUMN — stacked buttons */}
        <div className="tdViewButtons">
          <button
            className="tdViewBtnAdd"
            onClick={() => navigate("/testdrive/add")}
          >
            Add New
          </button>

          <button
            className="tdViewBtnEdit"
            onClick={() => navigate(`/testdrive/${id}/edit`)}
          >
            Edit
          </button>
        </div>
      </div>
    </div>
  );
};

export default TestDriveViewPage;