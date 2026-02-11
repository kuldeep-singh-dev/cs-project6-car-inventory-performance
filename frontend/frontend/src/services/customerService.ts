// import api from "./api";
// import type { Customer, CustomerCreate, CustomerUpdate } from "../types/customer";

// export const customerService = {
//   getAll: async (): Promise<Customer[]> => {
//     const res = await api.get<Customer[]>("/customers");
//     return res.data;
//   },

//   getById: async (id: string): Promise<Customer> => {
//     const res = await api.get<Customer>(`/customers/${id}`);
//     return res.data;
//   },

//   create: async (payload: CustomerCreate): Promise<Customer> => {
//     const res = await api.post<Customer>("/customers", payload);
//     return res.data;
//   },

//   patch: async (id: string, payload: CustomerUpdate): Promise<Customer> => {
//     const res = await api.patch<Customer>(`/customers/${id}`, payload);
//     return res.data;
//   },
// };

