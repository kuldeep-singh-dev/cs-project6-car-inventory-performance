// import api from "../services/api";
// import type { Customer, CustomerCreate, CustomerUpdate } from "../types/customer";

// export function getCustomers(): Promise<Customer[]> {
//   return api.get<Customer[]>("/customers");
// }

// export function getCustomerById(id: string): Promise<Customer> {
//   return api.get<Customer>(`/customers/${encodeURIComponent(id)}`);
// }

// export function createCustomer(payload: CustomerCreate): Promise<Customer> {
//   return api.post<Customer>("/customers", {
//     body: JSON.stringify(payload),
//   });
// }
    
// export function patchCustomer(id: string, payload: CustomerUpdate): Promise<Customer> {
//   return api.patch<Customer>(`/customers/${encodeURIComponent(id)}`, {
//     body: JSON.stringify(payload),
//   });
// }
