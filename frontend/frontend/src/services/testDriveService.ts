import api from "../services/api";
import type { TestDriveRev, TestDriveSend, TestDrivePost, TestDrivePatch} from "../types/testDrive";
import type { Customer } from "../types/customer";
import type { Vehicle } from "../types/vehicle";

export const testDriveService = {
  getAll: async (): Promise<TestDriveRev[]> => {
    const res = await api.get<TestDriveRev[]>("/testdrive");
    return res.data;
  },

  getById: async (id: string): Promise<TestDriveRev> => {
    const res = await api.get<TestDriveRev>(`/testdrive/${id}`);
    return res.data;
  },

  create: async (data:TestDrivePost): Promise<TestDriveSend> => {
    const res = await api.post<TestDriveSend>("/testdrive/post", data);
    return res.data;
  },

  update: async (
    id: string,
    data: TestDrivePatch
  ): Promise<TestDriveSend> => {
    const res = await api.patch<TestDriveSend>(`/testdrive/${id}`, data);
    return res.data;
  },

  delete: async (id: string): Promise<void> => {
    await api.delete(`/testdrive/${id}`);
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
