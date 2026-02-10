<<<<<<< HEAD
import api from "../services/api";
import type { Customer } from "../types/customer";

export const getCustomers = async (): Promise<Customer[]> => {
  const response = await api.get<Customer[]>("/customers");
  return response.data;
};

export const getCustomerById = async (id: string): Promise<Customer> => {
  const response = await api.get<Customer>(`/customers/${id}`);
  return response.data;
};

export const createCustomer = async (
  customer: Omit<Customer, "id">
): Promise<Customer> => {
  const response = await api.post<Customer>("/customers", customer);
  return response.data;
};

export const updateCustomer = async (
  id: string,
  data: Partial<Customer>
): Promise<Customer> => {
  const response = await api.patch<Customer>(`/customers/${id}`, data);
  return response.data;
};
=======
import { apiRequest } from "./api";
import type { Customer, CustomerCreate, CustomerUpdate } from "../types/customer";

export function getCustomers(): Promise<Customer[]> {
  return apiRequest<Customer[]>("/customers", { method: "GET" });
}

export function getCustomerById(id: string): Promise<Customer> {
  return apiRequest<Customer>(`/customers/${encodeURIComponent(id)}`, { method: "GET" });
}

export function createCustomer(payload: CustomerCreate): Promise<Customer> {
  return apiRequest<Customer>("/customers", {
    method: "POST",
    body: JSON.stringify(payload),
  });
}

export function patchCustomer(id: string, payload: CustomerUpdate): Promise<Customer> {
  return apiRequest<Customer>(`/customers/${encodeURIComponent(id)}`, {
    method: "PATCH",
    body: JSON.stringify(payload),
  });
}
>>>>>>> e735043910701c413a4c8227c8fdac0467f0595e
