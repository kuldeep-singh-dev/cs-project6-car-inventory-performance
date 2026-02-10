export const FuelType = {
  GASOLINE: "Gasoline",
  DIESEL: "Diesel",
  ELECTRIC: "Electric",
  HYBRID: "Hybrid",
} as const;

export const Transmission = {
  MANUAL: "Manual",
  AUTOMATIC: "Automatic",
  CVT: "CVT",
} as const;

export const VehicleStatus = {
  AVAILABLE: "Available",
  SOLD: "Sold",
} as const;

export type FuelType = typeof FuelType[keyof typeof FuelType];
export type Transmission = typeof Transmission[keyof typeof Transmission];
export type VehicleStatus = typeof VehicleStatus[keyof typeof VehicleStatus];
