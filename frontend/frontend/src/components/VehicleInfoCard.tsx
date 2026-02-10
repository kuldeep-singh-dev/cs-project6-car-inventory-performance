import { useNavigate } from "react-router-dom";
import type { Vehicle } from "../types/vehicle";
import "./VehicleInfoCard.css";

interface Props {
  vehicle: Vehicle;
}

const VehicleInfoCard = ({ vehicle }: Props) => {
  const navigate = useNavigate();

  return (
    <div
      className="vehicleCard"
      onClick={() => navigate(`/vehicles/${vehicle.id}`)}
    >
      <div className="vehicleCardImage">
        <h3>Car Image</h3>
      </div>

      <div className="vehicleCardContent">
        <h3 className="vehicleTitle">
          {vehicle.make} {vehicle.model}
        </h3>
        <p>Year: {vehicle.year}</p>
        <p>Price: ${vehicle.market_price}</p>
        <p>Status: {vehicle.status}</p>
      </div>
    </div>
  );
};

export default VehicleInfoCard;
