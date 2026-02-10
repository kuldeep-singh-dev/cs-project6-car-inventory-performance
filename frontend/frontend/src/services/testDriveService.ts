import api from "../services/api";
import type { TestDrive } from "../types/testDrive";
import type { Customer } from "../types/customer";
import type { Vehicle } from "../types/vehicle";

export const testDriveService = {
  getAll: async (): Promise<TestDrive[]> => {
    const res = await api.get<TestDrive[]>("/test-drives");
    return res.data;
  },

  getById: async (id: string): Promise<TestDrive> => {
    const res = await api.get<TestDrive>(`/test-drives/${id}`);
    return res.data;
  },

  create: async (data: Omit<TestDrive, "id">): Promise<TestDrive> => {
    const res = await api.post<TestDrive>("/test-drives", data);
    return res.data;
  },

  update: async (
    id: string,
    data: Partial<TestDrive>
  ): Promise<TestDrive> => {
    const res = await api.patch<TestDrive>(`/test-drives/${id}`, data);
    return res.data;
  },

  delete: async (id: string): Promise<void> => {
    await api.delete(`/test-drives/${id}`);
  },

  // helper endpoints (as per SDD)
  getCustomers: async (): Promise<Customer[]> => {
    const res = await api.get<Customer[]>("/customers");
    return res.data;
  },

  getVehicles: async (): Promise<Vehicle[]> => {
    const res = await api.get<Vehicle[]>("/vehicles");
    return res.data;
  },
};
