import { useState } from "react";
import { imageService, type VehicleImage } from "../services/imageService";
import "./ImageManager.css";

interface Props {
  vehicleId: string;
  images: VehicleImage[];
  onImagesUpdated: () => void; // Callback to refresh images
  onClose: () => void;
}

const ImageManager = ({ vehicleId, images, onImagesUpdated, onClose }: Props) => {
  const [uploading, setUploading] = useState(false);

  const handleUpload = async (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (!file) return;

    try {
      setUploading(true);
      await imageService.upload(vehicleId, file);
      onImagesUpdated(); // Refresh parent component
    } catch (error) {
      console.error("Upload failed:", error);
      alert("Failed to upload image");
    } finally {
      setUploading(false);
    }
  };

  const handleDelete = async (imageId: string) => {
    if (!confirm("Are you sure you want to delete this image?")) return;

    try {
      await imageService.delete(imageId);
      onImagesUpdated(); // Refresh parent component
    } catch (error) {
      console.error("Delete failed:", error);
      alert("Failed to delete image");
    }
  };

  return (
    <div className="imageManagerOverlay" onClick={onClose}>
      <div className="imageManagerModal" onClick={(e) => e.stopPropagation()}>
        <div className="imageManagerHeader">
          <h2>Manage Images</h2>
          <button className="imageManagerClose" onClick={onClose}>×</button>
        </div>

        <div className="imageManagerBody">
          {/* Upload Section */}
          <div className="uploadSection">
            <label htmlFor="imageUpload" className="uploadButton">
              {uploading ? "Uploading..." : "Upload New Image"}
            </label>
            <input
              id="imageUpload"
              type="file"
              accept="image/*"
              onChange={handleUpload}
              disabled={uploading}
              style={{ display: "none" }}
            />
          </div>

          {/* Images Grid */}
          <div className="imagesGrid">
            {images.length === 0 ? (
              <p>No images yet. Upload one above!</p>
            ) : (
              images.map((img) => (
                <div key={img.id} className="imageItem">
                  <img
                    src={imageService.getUrl(img.img_url)}
                    alt="Vehicle"
                  />
                  <button
                    className="deleteButton"
                    onClick={() => handleDelete(img.id)}
                  >
                    Delete
                  </button>
                </div>
              ))
            )}
          </div>
        </div>
      </div>
    </div>
  );
};

export default ImageManager;