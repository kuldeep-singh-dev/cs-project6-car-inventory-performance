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
  const [currentPage, setCurrentPage] = useState(1);
  
  const itemsPerPage = 50;

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

  // Paginate the filtered results
  const paginatedVehicles = useMemo(() => {
    const startIndex = (currentPage - 1) * itemsPerPage;
    const endIndex = startIndex + itemsPerPage;
    return filteredVehicles.slice(startIndex, endIndex);
  }, [filteredVehicles, currentPage]);

  const totalPages = Math.ceil(filteredVehicles.length / itemsPerPage);

  // Reset to page 1 when filters change
  useEffect(() => {
    setCurrentPage(1);
  }, [filters]);

  const handlePageChange = (page: number) => {
    if (page >= 1 && page <= totalPages) {
      setCurrentPage(page);
      // Scroll the content area to top
      document.querySelector('.vehiclesContent')?.scrollTo({ top: 0, behavior: 'smooth' });
    }
  };

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
            <>
              <div className="vehiclesStats">
                Showing {((currentPage - 1) * itemsPerPage) + 1} - {Math.min(currentPage * itemsPerPage, filteredVehicles.length)} of {filteredVehicles.length} vehicles
              </div>

              <div className="vehiclesGrid">
                {paginatedVehicles.map((vehicle) => (
                  <VehicleInfoCard key={vehicle.id} vehicle={vehicle} />
                ))}
              </div>

              {/* Pagination Controls */}
              {totalPages > 1 && (
                <div className="pagination">
                  <button 
                    onClick={() => handlePageChange(1)}
                    disabled={currentPage === 1}
                    className="paginationButton"
                  >
                    First
                  </button>
                  
                  <button 
                    onClick={() => handlePageChange(currentPage - 1)}
                    disabled={currentPage === 1}
                    className="paginationButton"
                  >
                    Previous
                  </button>

                  <span className="pageInfo">
                    Page {currentPage} of {totalPages}
                  </span>

                  <button 
                    onClick={() => handlePageChange(currentPage + 1)}
                    disabled={currentPage === totalPages}
                    className="paginationButton"
                  >
                    Next
                  </button>

                  <button 
                    onClick={() => handlePageChange(totalPages)}
                    disabled={currentPage === totalPages}
                    className="paginationButton"
                  >
                    Last
                  </button>
                </div>
              )}
            </>
          )}
        </section>
      </div>
    </div>
  );
};

export default VehiclesPage;