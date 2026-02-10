import VehicleViewInfo from "../../components/VehicleViewInfo";
import { useParams, useNavigate } from "react-router-dom";
import { useEffect, useState } from "react";
import { vehicleService } from "../../services/vehicleService";
import type { Vehicle } from "../../types/vehicle";

const ViewVehicle = () => {
  const navigate = useNavigate();
  const { id } = useParams<{ id: string }>();
  const [vehicle, setVehicle] = useState<Vehicle | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    if (!id) return;

    const fetchVehicle = async () => {
      try {
        setLoading(true);
        const data = await vehicleService.getById(id);
        setVehicle(data);
      } catch (err: any) {
        console.error(err);
        setError(err?.response?.data || "Failed to load vehicle");
      } finally {
        setLoading(false);
      }
    };

    fetchVehicle();
  }, [id]);

  if (loading) return <p>Loading vehicle...</p>;
  if (error) return <p style={{ color: "red" }}>{error}</p>;
  if (!vehicle) return <p>Vehicle not found</p>;

  return (
    <div>
      <div style={{ padding: "20px", position: "relative" }}>
        <h1 style={{ textAlign: "center", margin: 0, fontSize: "2rem" }}>
          Vehicle Details
        </h1>

        {/* Add + Edit buttons */}
        <div style={{ position: "absolute", right: 20, top: 20, display: "flex", gap: 10 }}>
          <button
            style={{
              padding: "8px 16px",
              backgroundColor: "#111827",
              color: "#fff",
              border: "none",
              borderRadius: "6px",
              cursor: "pointer",
            }}
            onClick={() => navigate("/addvehicle")}
          >
            Add
          </button>

          <button
            style={{
              padding: "8px 16px",
              backgroundColor: "#2563eb",
              color: "#fff",
              border: "none",
              borderRadius: "6px",
              cursor: "pointer",
            }}
            onClick={() => navigate(`/vehicles/edit/${vehicle.id}`)}
          >
            Edit
          </button>
        </div>

        <div style={{ marginTop: "60px" }}>
          <VehicleViewInfo vehicle={vehicle} />
        </div>
      </div>
    </div>
  );
};

export default ViewVehicle;
