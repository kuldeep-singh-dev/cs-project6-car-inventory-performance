import api from "./api";

export interface VehicleImage {
  id: string;
  vehicle_id: string;
  img_url: string;
}

export const imageService = {
  // Get all images for a vehicle
  getByVehicle: async (vehicleId: string): Promise<VehicleImage[]> => {
    const res = await api.get(`/vehicles/${vehicleId}/images`);
    return res.data;
  },

  // Upload image for a vehicle
  upload: async (vehicleId: string, file: File) => {
    const formData = new FormData();
    formData.append("file", file);
    
    const res = await api.post(`/vehicles/${vehicleId}/images`, formData, {
      headers: { "Content-Type": "multipart/form-data" }
    });
    return res.data;
  },

  // Delete an image
  delete: async (imageId: string) => {
    await api.delete(`/images/${imageId}`);
  },

  // Get full image URL
  getUrl: (imgUrl: string): string => {
    // If it's an external URL (Unsplash), return as-is
    if (imgUrl.startsWith('http')) {
      return imgUrl;
    }
    // If it's a local upload, use the API endpoint
    return `/api${imgUrl}`;  // Goes through Vite proxy
  }
};