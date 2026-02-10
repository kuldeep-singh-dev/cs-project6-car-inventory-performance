import type { Vehicle } from "../types/vehicle";
import "./VehicleViewInfo.css";

interface Props {
  vehicle: Vehicle;
}

const VehicleViewInfo = ({ vehicle }: Props) => {
  return (
    <div className="vehicleView">
      {/* Left Section */}
      <div className="vehicleLeft">
        <h1 className="vehicleHeading">{`${vehicle.year} ${vehicle.make} ${vehicle.model}`}</h1>

        <div className="vehicleMainImage">
          <h3>Car Image</h3>
        </div>

        <div className="vehicleThumbRow">
          <div className="thumb"></div>
          <div className="thumb"></div>
          <div className="thumb"></div>
          <div className="thumb"></div>
        </div>

        <p>{vehicle.trim ? `Trim: ${vehicle.trim}` : "About Car"}</p>
      </div>

      {/* Right Section */}
      <div className="vehicleRight">
        <div className="infoBox">Make: {vehicle.make}</div>
        <div className="infoBox">Model: {vehicle.model}</div>
        <div className="infoBox">Year: {vehicle.year}</div>

        {vehicle.trim && <div className="infoBox">Trim: {vehicle.trim}</div>}

        <div className="infoBox">Odometer: {vehicle.odometer}</div>
        <div className="infoBox">Market Price: ${vehicle.market_price}</div>
        <div className="infoBox">Transmission: {vehicle.transmission}</div>
        <div className="infoBox">Fuel Type: {vehicle.fuel_type}</div>
        <div className="infoBox">Status: {vehicle.status}</div>
      </div>
    </div>
  );
};

export default VehicleViewInfo;
