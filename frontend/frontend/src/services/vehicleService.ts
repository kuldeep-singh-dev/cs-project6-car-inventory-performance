import api from "./api";
import type { Vehicle } from "../types/vehicle";


type CreateVehicleResponse = { id: string };

export const vehicleService = {
  getAll: async (): Promise<Vehicle[]> => {
    const res = await api.get<Vehicle[]>("/vehicles");
    return res.data;
  },

  getById: async (id: string): Promise<Vehicle> => {
    const res = await api.get<Vehicle>(`/vehicles/${id}`);
    return res.data;
  },

  create: async (vehicle: Omit<Vehicle, "id">): Promise<CreateVehicleResponse> => {
    const res = await api.post<CreateVehicleResponse>("/vehicles", vehicle);
    return res.data;
  },

  update: async (
    id: string,
    data: Partial<Vehicle>
  ): Promise<Vehicle> => {
    const res = await api.put<Vehicle>(`/vehicles/${id}`, data);
    return res.data;
  },

 
};
