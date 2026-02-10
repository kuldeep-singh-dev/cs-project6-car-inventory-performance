
import { useState } from "react";
import type { Vehicle } from "../../types/vehicle";
import { Transmission, FuelType, VehicleStatus } from "../../types/enums";
import { useNavigate } from "react-router-dom";
import { vehicleService } from "../../services/vehicleService";

const AddVehiclePage = () => {
  const navigate = useNavigate();

  const [vehicle, setVehicle] = useState<Partial<Vehicle>>({
    transmission: Transmission.AUTOMATIC,
    fuel_type: FuelType.GASOLINE,
    status: VehicleStatus.AVAILABLE,
    trim: "",
  });

  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

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
      // Build payload EXACTLY as backend expects
      const vehicleToSend: Omit<Vehicle, "id"> = {
        vin: vehicle.vin!,
        make: vehicle.make!,
        model: vehicle.model!,
        year: Number(vehicle.year),
        odometer: Number(vehicle.odometer),
        fuel_type: vehicle.fuel_type!,
        transmission: vehicle.transmission!,
        trim: vehicle.trim ?? "",
        market_price: Number(vehicle.market_price),
        status: vehicle.status!,
        };


      await vehicleService.create(vehicleToSend);
      navigate("/vehicles");
    } catch (err: any) {
      console.error("Failed to add vehicle:", err);
      setError(err?.response?.data || "Failed to create vehicle");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div>

      <div style={{ padding: "20px", maxWidth: "600px", margin: "0 auto" }}>
        <h1>Add Vehicle</h1>

        {error && <p style={{ color: "red" }}>{error}</p>}

        <form
          onSubmit={handleSubmit}
          style={{ display: "flex", flexDirection: "column", gap: "12px" }}
        >
          <input name="vin" placeholder="VIN" onChange={handleChange} required />
          <input name="make" placeholder="Make" onChange={handleChange} required />
          <input name="model" placeholder="Model" onChange={handleChange} required />

          <input
            name="year"
            type="number"
            placeholder="Year"
            onChange={handleChange}
            required
          />

          <input
            name="odometer"
            type="number"
            placeholder="Odometer"
            onChange={handleChange}
            required
          />

          <select
            name="transmission"
            onChange={handleChange}
            value={vehicle.transmission}
          >
            <option value={Transmission.AUTOMATIC}>Automatic</option>
            <option value={Transmission.MANUAL}>Manual</option>
            <option value={Transmission.CVT}>CVT</option>
          </select>

          <select name="fuel_type" onChange={handleChange} value={vehicle.fuel_type}>
            <option value={FuelType.GASOLINE}>Gasoline</option>
            <option value={FuelType.DIESEL}>Diesel</option>
            <option value={FuelType.HYBRID}>Hybrid</option>
            <option value={FuelType.ELECTRIC}>Electric</option>
          </select>

          <input
            name="market_price"
            type="number"
            placeholder="Market Price"
            onChange={handleChange}
            required
          />

          <select name="status" onChange={handleChange} value={vehicle.status}>
            <option value={VehicleStatus.AVAILABLE}>Available</option>
            <option value={VehicleStatus.SOLD}>Sold</option>
          </select>

          <input
            name="trim"
            placeholder="Trim (optional)"
            onChange={handleChange}
          />

          <button type="submit" disabled={loading}>
            {loading ? "Adding..." : "Add Vehicle"}
          </button>
        </form>
      </div>
    </div>
  );
};

export default AddVehiclePage;
