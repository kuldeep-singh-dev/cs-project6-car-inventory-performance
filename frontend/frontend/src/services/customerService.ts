// import { api } from "./api";
// import type { Customer, CustomerCreate, CustomerUpdate } from "../types/customer";

// export function getCustomers(): Promise<Customer[]> {
//   return api.get<Customer[]>("/customers");
// }

// export function getCustomerById(id: string): Promise<Customer> {
//   return apiRequest<Customer>(`/customers/${encodeURIComponent(id)}`, { method: "GET" });
// }

// export function createCustomer(payload: CustomerCreate): Promise<Customer> {
//   return apiRequest<Customer>("/customers", {
//     method: "POST",
//     body: JSON.stringify(payload),
//   });
// }

// export function patchCustomer(id: string, payload: CustomerUpdate): Promise<Customer> {
//   return apiRequest<Customer>(`/customers/${encodeURIComponent(id)}`, {
//     method: "PATCH",
//     body: JSON.stringify(payload),
//   });
// }
