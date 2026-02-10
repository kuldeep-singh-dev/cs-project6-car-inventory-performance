import type { ChangeEvent } from "react";
import "./VehicleFilter.css";

export interface FilterValues {
  available: boolean;
  sold: boolean;
  priceMin: string;
  priceMax: string;
  fuelType: string;
  odometerMin: string;
  odometerMax: string;
}

interface Props {
  filters: FilterValues;
  setFilters: React.Dispatch<React.SetStateAction<FilterValues>>;
  applyFilters: () => void;
}

const VehicleFilter = ({ filters, setFilters, applyFilters }: Props) => {
  const handleInputChange = (
    e: ChangeEvent<HTMLInputElement | HTMLSelectElement>
  ) => {
    const { name, value } = e.target;

    let newValue: string | boolean;

    if (e.target instanceof HTMLInputElement && e.target.type === "checkbox") {
      newValue = e.target.checked;
    } else {
      newValue = value;
    }

    setFilters((prev) => ({
      ...prev,
      [name]: newValue,
    }));
  };

  return (
    <div className="vehicleFilter">
      {/* Status */}
      <div className="vehicleFilterRow">
        <label className="checkboxLabel">
          <input
            type="checkbox"
            name="available"
            checked={filters.available}
            onChange={handleInputChange}
          />
          Available
        </label>

        <label className="checkboxLabel">
          <input
            type="checkbox"
            name="sold"
            checked={filters.sold}
            onChange={handleInputChange}
          />
          Sold
        </label>
      </div>

      {/* Price */}
      <div>
        <h4 className="sectionTitle">Market Price</h4>
        <div className="twoCol">
          <input
            type="number"
            name="priceMin"
            placeholder="Min"
            className="input"
            value={filters.priceMin}
            onChange={handleInputChange}
          />
          <input
            type="number"
            name="priceMax"
            placeholder="Max"
            className="input"
            value={filters.priceMax}
            onChange={handleInputChange}
          />
        </div>
      </div>

      {/* Fuel Type */}
      <div>
        <h4 className="sectionTitle">Fuel Type</h4>
        <select
          name="fuelType"
          className="select"
          value={filters.fuelType}
          onChange={handleInputChange}
        >
          <option value="">Any</option>
          <option value="Gasoline">Gasoline</option>
          <option value="Diesel">Diesel</option>
          <option value="Hybrid">Hybrid</option>
          <option value="Electric">Electric</option>
        </select>
      </div>

      {/* Odometer */}
      <div>
        <h4 className="sectionTitle">Odometer</h4>
        <div className="twoCol">
          <input
            type="number"
            name="odometerMin"
            placeholder="Min"
            className="input"
            value={filters.odometerMin}
            onChange={handleInputChange}
          />
          <input
            type="number"
            name="odometerMax"
            placeholder="Max"
            className="input"
            value={filters.odometerMax}
            onChange={handleInputChange}
          />
        </div>
      </div>

      <button className="applyBtn" onClick={applyFilters}>
        APPLY FILTER
      </button>
    </div>
  );
};

export default VehicleFilter;
