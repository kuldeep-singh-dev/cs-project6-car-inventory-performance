import api from "./api";
import type { TestDriveRev, TestDriveSend, TestDrivePost, TestDrivePatch} from "../types/testDrive";

export const testDriveService = {
  getAll: async (): Promise<TestDriveRev[]> => {
    const res = await api.get<TestDriveRev[]>("/testdrive");
    return res.data;
  },

  getById: async (id: string): Promise<TestDriveRev> => {
    const res = await api.get<TestDriveRev>(`/testdrive/${id}`);
    return res.data;
  },

  create: async (data: TestDrivePost): Promise<TestDriveRev> => {
  try {
    const res = await api.post<TestDriveRev>("/testdrive/post", data);
    return res.data;
  } catch (err: any) {
    // Extract backend message if it exists
    const message =
      err.response?.data?.message ||
      err.response?.data ||
      "Failed to create test drive.";

    throw new Error(message);
  }
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
  getByCustomerId: async (id: string): Promise<TestDriveRev[]> => {
  const res = await api.get<TestDriveRev[]>(`/testdrive/customer/${id}`);
  return res.data;
},

getByVehicleId: async (id: string): Promise<TestDriveRev[]> => {
  const res = await api.get<TestDriveRev[]>(`/testdrive/vehicle/${id}`);
  return res.data;
},
};
