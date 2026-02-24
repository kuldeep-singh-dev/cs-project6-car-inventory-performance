export interface SaleSummary {
  sale_id: string;
  date: string;
  price: number;
  vehicle: string;   // "Make Model"
  customer: string;  // "First Last"
}

export interface SaleCreatePayload {
  vehicle_id: string;
  customer_id: string;
  date: string;        // YYYY-MM-DD
  sale_price: number;
}

export interface SaleCreateResponse {
  sale_id: string;
  vehicle_id: string;
  customer_id: string;
  date: string;
  sale_price: number;
}

export interface SaleUpdatePayload {
  sale_price?: number;
  date?: string;
}
