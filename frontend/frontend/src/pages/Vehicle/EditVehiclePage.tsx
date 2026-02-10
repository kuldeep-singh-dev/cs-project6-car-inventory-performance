import Header from "../../components/Header";
import { useState, useEffect } from "react";
import type { Vehicle } from "../../types/vehicle";
import { Transmission, FuelType, VehicleStatus } from "../../types/enums";
import { useNavigate, useParams } from "react-router-dom";
import { vehicleService } from "../../services/vehicleService";

const EditVehiclePage = () => {
  const { id } = useParams<{ id: string }>();
  const navigate = useNavigate();

  const [vehicle, setVehicle] = useState<Partial<Vehicle>>({});
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    if (!id) return;

    const fetchVehicle = async () => {
      setLoading(true);
      try {
        const data = await vehicleService.getById(id);
        setVehicle(data);
      } catch (err: any) {
        console.error("Failed to load vehicle:", err);
        setError(err?.response?.data || "Failed to load vehicle");
      } finally {
        setLoading(false);
      }
    };

    fetchVehicle();
  }, [id]);

  const handleChange = (
    e: React.ChangeEvent<HTMLInputElement | HTMLSelectElement>
  ) => {
    const { name, value } = e.target;

    setVehicle((prev) => ({
      ...prev,
      [name]:
        name === "year" || name === "odometer" || name === "market_price"
          ? Number(value)
          : value,
    }));
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError(null);
    setLoading(true);

    try {
      if (
        !vehicle.vin ||
        !vehicle.make ||
        !vehicle.model ||
        vehicle.year == null ||
        vehicle.odometer == null ||
        !vehicle.fuel_type ||
        !vehicle.transmission ||
        vehicle.market_price == null ||
        !vehicle.status
      ) {
        alert("Please fill all required fields");
        setLoading(false);
        return;
      }

      const vehicleToSend: Partial<Vehicle> = {
        vin: vehicle.vin,
        make: vehicle.make,
        model: vehicle.model,
        year: Number(vehicle.year),
        odometer: Number(vehicle.odometer),
        fuel_type: vehicle.fuel_type,
        transmission: vehicle.transmission,
        trim: vehicle.trim ?? "",
        market_price: Number(vehicle.market_price),
        status: vehicle.status ?? VehicleStatus.AVAILABLE,
      };

      await vehicleService.update(id!, vehicleToSend);
      navigate("/vehicles");
    } catch (err: any) {
      console.error("Failed to update vehicle:", err);
      setError(err?.response?.data || "Failed to update vehicle");
    } finally {
      setLoading(false);
    }
  };

  if (loading) return <p>Loading...</p>;

  return (
    <div>
      <Header />
      <div style={{ padding: "20px", maxWidth: "600px", margin: "0 auto" }}>
        <h1>Edit Vehicle</h1>
        {error && <p style={{ color: "red" }}>{error}</p>}

        <form
          onSubmit={handleSubmit}
          style={{ display: "flex", flexDirection: "column", gap: "12px" }}
        >
          <input
            name="vin"
            placeholder="VIN"
            value={vehicle.vin || ""}
            onChange={handleChange}
            required
          />
          <input
            name="make"
            placeholder="Make"
            value={vehicle.make || ""}
            onChange={handleChange}
            required
          />
          <input
            name="model"
            placeholder="Model"
            value={vehicle.model || ""}
            onChange={handleChange}
            required
          />
          <input
            name="year"
            type="number"
            placeholder="Year"
            value={vehicle.year ?? ""}
            onChange={handleChange}
            required
          />
          <input
            name="odometer"
            type="number"
            placeholder="Odometer"
            value={vehicle.odometer ?? ""}
            onChange={handleChange}
            required
          />

          <select
            name="transmission"
            value={vehicle.transmission ?? Transmission.AUTOMATIC}
            onChange={handleChange}
          >
            <option value={Transmission.AUTOMATIC}>Automatic</option>
            <option value={Transmission.MANUAL}>Manual</option>
            <option value={Transmission.CVT}>CVT</option>
          </select>

          <select
            name="fuel_type"
            value={vehicle.fuel_type ?? FuelType.GASOLINE}
            onChange={handleChange}
          >
            <option value={FuelType.GASOLINE}>Gasoline</option>
            <option value={FuelType.DIESEL}>Diesel</option>
            <option value={FuelType.HYBRID}>Hybrid</option>
            <option value={FuelType.ELECTRIC}>Electric</option>
          </select>

          <input
            name="market_price"
            type="number"
            placeholder="Market Price"
            value={vehicle.market_price ?? ""}
            onChange={handleChange}
            required
          />

          <select
            name="status"
            value={vehicle.status ?? VehicleStatus.AVAILABLE}
            onChange={handleChange}
          >
            <option value={VehicleStatus.AVAILABLE}>Available</option>
            <option value={VehicleStatus.SOLD}>Sold</option>
          </select>

          <input
            name="trim"
            placeholder="Trim (optional)"
            value={vehicle.trim ?? ""}
            onChange={handleChange}
          />

          <button type="submit" disabled={loading}>
            {loading ? "Updating..." : "Save"}
          </button>
        </form>
      </div>
    </div>
  );
};

export default EditVehiclePage;
