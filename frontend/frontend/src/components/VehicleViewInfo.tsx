import { useState, useEffect } from "react";
import type { Vehicle } from "../types/vehicle";
import { imageService, type VehicleImage } from "../services/imageService";
import "./VehicleViewInfo.css";

interface Props {
  vehicle: Vehicle;
}

const VehicleViewInfo = ({ vehicle }: Props) => {

  const [images, setImages] = useState<VehicleImage[]>([]);
  const [selectedImage, setSelectedImage] = useState<string>("");
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchImages = async () => {
      try {
        const vehicleImages = await imageService.getByVehicle(vehicle.id);
        setImages(vehicleImages);
        
        // Set first image as selected by default
        if (vehicleImages.length > 0) {
          setSelectedImage(imageService.getUrl(vehicleImages[0].img_url));
        }
      } catch (error) {
        console.error("Failed to fetch images:", error);
      } finally {
        setLoading(false);
      }
    };

    fetchImages();
  }, [vehicle.id]);

  return (
    <div className="vehicleView">
      {/* Left Section */}
      <div className="vehicleLeft">
        <h1 className="vehicleHeading">{`${vehicle.year} ${vehicle.make} ${vehicle.model}`}</h1>

        <div className="vehicleMainImage">
          {loading ? (
            <div style={{ textAlign: 'center', padding: '50px' }}>Loading...</div>
          ) : selectedImage ? (
            <img 
              src={selectedImage} 
              alt={`${vehicle.make} ${vehicle.model}`}
              style={{ width: '100%', height: '100%', objectFit: 'cover' }}
            />
          ) : (
            <div style={{ textAlign: 'center', padding: '50px' }}>No images available</div>
          )}
        </div>

        <div className="vehicleThumbRow">
          {images.slice(0, 4).map((img) => (
            <div 
              key={img.id} 
              className={`thumb ${selectedImage === imageService.getUrl(img.img_url) ? 'active' : ''}`}
              onClick={() => setSelectedImage(imageService.getUrl(img.img_url))}
            >
              <img 
                src={imageService.getUrl(img.img_url)} 
                alt="thumbnail"
                style={{ width: '100%', height: '100%', objectFit: 'cover', cursor: 'pointer' }}
              />
            </div>
          ))}
          
          {/* Fill empty slots if less than 4 images */}
          {[...Array(Math.max(0, 4 - images.length))].map((_, i) => (
            <div key={`empty-${i}`} className="thumb empty"></div>
          ))}
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
