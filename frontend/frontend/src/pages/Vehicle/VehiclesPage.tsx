import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import VehicleInfoCard from "../../components/VehicleInfoCard";
import VehicleFilter from "../../components/VehicleFilter";
import type { FilterValues } from "../../components/VehicleFilter";
import { vehicleService } from "../../services/vehicleService";
import type { Vehicle } from "../../types/vehicle";
import { VehicleStatus } from "../../types/enums";
import "./VehiclesPage.css";

const VehiclesPage = () => {
  const navigate = useNavigate();
  const [vehicles, setVehicles] = useState<Vehicle[]>([]);
  const [filteredVehicles, setFilteredVehicles] = useState<Vehicle[]>([]);
  const [loading, setLoading] = useState(true);

  const [filters, setFilters] = useState<FilterValues>({
    available: false,
    sold: false,
    priceMin: "",
    priceMax: "",
    fuelType: "",
    odometerMin: "",
    odometerMax: "",
  });

  const fetchVehicles = async () => {
    setLoading(true);
    try {
      const data = await vehicleService.getAll();
      setVehicles(data);
      setFilteredVehicles(data);
    } catch (err) {
      console.error("Failed to fetch vehicles:", err);
      setVehicles([]);
      setFilteredVehicles([]);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchVehicles();
  }, []);

  const applyFilters = () => {
    let filtered = [...vehicles];

    if (filters.available && !filters.sold) {
      filtered = filtered.filter((v) => v.status === VehicleStatus.AVAILABLE);
    } else if (!filters.available && filters.sold) {
      filtered = filtered.filter((v) => v.status === VehicleStatus.SOLD);
    }

    if (filters.priceMin) {
      filtered = filtered.filter((v) => v.market_price >= Number(filters.priceMin));
    }
    if (filters.priceMax) {
      filtered = filtered.filter((v) => v.market_price <= Number(filters.priceMax));
    }

    if (filters.fuelType) {
      filtered = filtered.filter((v) => v.fuel_type === filters.fuelType);
    }

    if (filters.odometerMin) {
      filtered = filtered.filter((v) => v.odometer >= Number(filters.odometerMin));
    }
    if (filters.odometerMax) {
      filtered = filtered.filter((v) => v.odometer <= Number(filters.odometerMax));
    }

    setFilteredVehicles(filtered);
  };

  if (loading) return <p className="vehiclesLoading">Loading vehicles...</p>;

  return (
    <div className="vehiclesPage">
      <div className="vehiclesHeader">
        <h1 className="vehiclesTitle">Vehicles</h1>
        <button className="vehiclesAddBtn" onClick={() => navigate("/addvehicle")}>
          Add
        </button>
      </div>

      <div className="vehiclesBody">
        <VehicleFilter filters={filters} setFilters={setFilters} applyFilters={applyFilters} />

        <div className="vehiclesGrid">
          {filteredVehicles.length === 0 ? (
            <p>No vehicles match the filters.</p>
          ) : (
            filteredVehicles.map((vehicle) => (
              <VehicleInfoCard key={vehicle.id} vehicle={vehicle} />
            ))
          )}
        </div>
      </div>
    </div>
  );
};

export default VehiclesPage;
