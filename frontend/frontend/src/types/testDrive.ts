export type TestDriveSend = {
  id: string;
  customerId: string;
  vehicleId: string;
  date: string;
  comments?: string;
};
export type TestDriveRev = {
  id:string,
  firstName:string,
  lastName:string,
  make:string,
  model:string, 
  date:string,
  comment:string
};
export type TestDrivePost = {
  customerId: string;
  vehicleId: string;
  date: string;
  comment: string;
};
export type TestDrivePatch = {
  date?: string;
  comment?: string;
};