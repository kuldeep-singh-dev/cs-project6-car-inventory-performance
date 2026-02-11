import { useEffect, useMemo, useState } from "react";
import { useNavigate } from "react-router-dom";
import VehicleInfoCard from "../../components/VehicleInfoCard";
import VehicleFilter from "../../components/VehicleFilter";
import type { FilterValues } from "../../components/VehicleFilter";
import { vehicleService } from "../../services/vehicleService";
import type { Vehicle } from "../../types/vehicle";
import "./VehiclesPage.css";

const VehiclesPage = () => {
  const navigate = useNavigate();
  const [vehicles, setVehicles] = useState<Vehicle[]>([]);
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
      setVehicles(Array.isArray(data) ? data : []);
    } catch (err) {
      console.error("Failed to fetch vehicles:", err);
      setVehicles([]);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchVehicles();
  }, []);

  // Filter vehicles without needing a separate "filteredVehicles" state (less bugs)
  const filteredVehicles = useMemo(() => {
    let filtered = [...vehicles];

    // Status filter
    if (filters.available && !filters.sold) {
      filtered = filtered.filter((v) => v.status === "Available");
    } else if (!filters.available && filters.sold) {
      filtered = filtered.filter((v) => v.status === "Sold");
    }
    // If both checked or none checked => show all (good UX)

    // Market price
    if (filters.priceMin) {
      filtered = filtered.filter((v) => v.market_price >= Number(filters.priceMin));
    }
    if (filters.priceMax) {
      filtered = filtered.filter((v) => v.market_price <= Number(filters.priceMax));
    }

    // Fuel type
    if (filters.fuelType) {
      filtered = filtered.filter((v) => v.fuel_type === filters.fuelType);
    }

    // Odometer
    if (filters.odometerMin) {
      filtered = filtered.filter((v) => v.odometer >= Number(filters.odometerMin));
    }
    if (filters.odometerMax) {
      filtered = filtered.filter((v) => v.odometer <= Number(filters.odometerMax));
    }

    return filtered;
  }, [vehicles, filters]);

  if (loading) return <div className="vehiclesLoading">Loading...</div>;

  return (
    <div className="vehiclesPage">
      <div className="vehiclesHeader">
        <div className="vehiclesHeaderSpacer" />
        <h1 className="vehiclesTitle">Vehicles</h1>

        <button className="vehiclesAddBtn" onClick={() => navigate("/addvehicle")}>
          Add Vehicle
        </button>
      </div>

      <div className="vehiclesLayout">
        {/* Sticky filter column */}
        <aside className="vehiclesSidebar">
          <div className="vehiclesSticky">
            <VehicleFilter filters={filters} setFilters={setFilters} applyFilters={() => { /* optional now */ }} />
          </div>
        </aside>

        {/* Scroll only the cards area */}
        <section className="vehiclesContent">
          {filteredVehicles.length === 0 ? (
            <div className="vehiclesEmpty">No vehicles match the filters.</div>
          ) : (
            <div className="vehiclesGrid">
              {filteredVehicles.map((vehicle) => (
                <VehicleInfoCard key={vehicle.id} vehicle={vehicle} />
              ))}
            </div>
          )}
        </section>
      </div>
    </div>
  );
};

export default VehiclesPage;
