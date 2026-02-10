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
