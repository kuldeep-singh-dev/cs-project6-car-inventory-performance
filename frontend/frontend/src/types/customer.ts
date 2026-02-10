export type Customer = {
  id: string;
  first_name: string;
  last_name: string;
  address?: string;
  ph_number: string;
  email: string;
  driving_licence: string;
};

export type CustomerCreate = {
  first_name: string;
  last_name: string;
  address?: string;
  ph_number: string;
  email: string;
  driving_licence: string;
};

export type CustomerUpdate = Partial<Omit<CustomerCreate, "driving_licence">> & {
  driving_licence?: string;
};
