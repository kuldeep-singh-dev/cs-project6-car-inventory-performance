export interface BillOfSale {
  id: string;
  vehicle_id: string;
  customer_id: string;
  date: string;        // ISO date
  sale_price: number;
}
