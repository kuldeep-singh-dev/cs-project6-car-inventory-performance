import type { FuelType, Transmission, VehicleStatus } from "./enums";

export interface Vehicle {
  id: string;
  vin: string;
  make: string;
  model: string;
  year: number;
  odometer: number;
  fuel_type: FuelType;
  transmission: Transmission;
  trim?: string;
  market_price: number;
  status: VehicleStatus;
  first_image?: string;
}
export type CreateVehiclePayload = Omit<Vehicle, "id">;

