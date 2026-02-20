import { useNavigate } from "react-router-dom";
import type { Vehicle } from "../types/vehicle";
import { imageService } from "../services/imageService";
import "./VehicleInfoCard.css";

interface Props {
  vehicle: Vehicle;
}

const VehicleInfoCard = ({ vehicle }: Props) => {
  const navigate = useNavigate();
  
  const thumbnailUrl = vehicle.first_image 
    ? imageService.getUrl(vehicle.first_image)
    : "";

  return (
    <div
      className="vehicleCard"
      onClick={() => navigate(`/vehicles/${vehicle.id}`)}
    >
      <div className="vehicleCardImage">
        {thumbnailUrl ? (
          <img 
            src={thumbnailUrl} 
            alt={`${vehicle.make} ${vehicle.model}`}
            style={{ width: '100%', height: '100%', objectFit: 'cover' }}
          />
        ) : (
          <div style={{ textAlign: 'center', padding: '20px' }}>No Image</div>
        )}
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