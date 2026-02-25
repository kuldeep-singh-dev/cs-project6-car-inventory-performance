// This service module provides functions to interact with the backend API for customer-related operations.
import api from "./api";
import type { Customer, CustomerCreate, CustomerUpdate } from "../types/customer";

export const customerService = {
  // Fetches all customers from the backend.
  // GET /customers
  getAll: async (): Promise<Customer[]> => {
    const res = await api.get<Customer[]>("/customers");
    // We return only the data, not the full axios response object.
    return res.data;
  },

  // Fetches a single customer by id.
  // GET /customers/:id
  getById: async (id: string): Promise<Customer> => {
    const res = await api.get<Customer>(`/customers/${id}`);
    return res.data;
  },

  // Creates a new customer.
  // POST /customers
  // The backend generates the id and returns the full created object.
  create: async (payload: CustomerCreate): Promise<Customer> => {
    const res = await api.post<Customer>("/customers", payload);
    return res.data;
  },

  // Partially updates a customer.
  // PATCH /customers/:id
  // Only fields included in the payload will be updated.
  patch: async (id: string, payload: CustomerUpdate): Promise<Customer> => {
    const res = await api.patch<Customer>(`/customers/${id}`, payload);
    return res.data;
  },

  // Legacy names for backward compatibility

  // Fetches all customers (legacy name).
  getCustomers: async (): Promise<Customer[]> => {
    return customerService.getAll();
  },

  // Fetches a single customer by id (legacy name).
  getCustomerById: async (id: string): Promise<Customer> => {
    return customerService.getById(id);
  },

  // Creates a new customer (legacy name).
  createCustomer: async (payload: CustomerCreate): Promise<Customer> => {
    return customerService.create(payload);
  },

  // Partially updates a customer (legacy name).
  updateCustomer: async (id: string, payload: CustomerUpdate): Promise<Customer> => {
    return customerService.patch(id, payload);
  },
  
};

export default customerService;