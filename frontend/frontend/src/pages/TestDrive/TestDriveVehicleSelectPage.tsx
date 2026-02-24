import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { vehicleService } from "../../services/vehicleService";
import type { Vehicle } from "../../types/vehicle";
import "./TestDriveCustomerSelectPage.css";

const TestDriveVehicleSelectPage = () => {
  const navigate = useNavigate();
  const [vehicles, setVehicles] = useState<Vehicle[]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const load = async () => {
      try {
        const data = await vehicleService.getAll();
        setVehicles(data);
      } catch (err) {
        console.error("Failed to load vehicles:", err);
        setVehicles([]);
      } finally {
        setLoading(false);
      }
    };

    load();
  }, []);

  if (loading) return <p>Loading vehicles...</p>;

  return (
    <div className="tdSelectPage">
      <h1 className="tdSelectTitle">Select Vehicle</h1>

      <div className="tdTableWrap">
        <table className="td-table">
          <thead>
            <tr>
              <th>Vehicle</th>
              <th>Select</th>
            </tr>
          </thead>

          <tbody>
            {vehicles.map((v) => (
              <tr key={v.id}>
                <td>{v.year} {v.make} {v.model}</td>
                <td>
                  <button
                    className="tdBtnPrimary"
                    onClick={() =>
                      navigate("/testdrive", { state: { vehicleId: v.id } })
                    }
                  >
                    View Test Drives
                  </button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
};

export default TestDriveVehicleSelectPage;