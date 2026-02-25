import {BrowserRouter, Routes, Route, Navigate} from 'react-router-dom';
import MainLayout from "../layouts/MainLayout";

import VehiclesPage from "../pages/Vehicle/VehiclesPage";
import AddVehiclePage from "../pages/Vehicle/AddVehiclePage";
import ViewVehicle from "../pages/Vehicle/ViewVehicle";
import EditVehiclePage from "../pages/Vehicle/EditVehiclePage";
import DashboardPage from "../pages/DashboardPage";
import SalesPage from "../pages/Sales/SalesPage";
import TestDrivePage from '../pages/TestDrive/TestDrivePage';
import TestDriveViewPage from '../pages/TestDrive/TestDriveViewpage';
import AddTestDrivePage from '../pages/TestDrive/AddTestDrivePage';
import EditTestDrivePage from '../pages/TestDrive/EditTestDrivePage';
import TestDriveCustomerSelectPage from '../pages/TestDrive/TestDriveCustomerSelectPage'; 
import TestDriveVehicleSelectPage from '../pages/TestDrive/TestDriveVehicleSelectPage';
import AddSalePage from '../pages/Sales/AddSalesPage';
import CustomersPage from '../pages/Customer/CustomersPage';
import AddCustomerPage from '../pages/Customer/AddCustomerPage';
import CustomerDetailsPage from '../pages/Customer/CustomerDetailsPage';
import EditCustomerPage from '../pages/Customer/EditCustomerPage';

const AppRoutes = () => {
  return (
    <BrowserRouter>
      <Routes>
        {/* Default route goes to Vehicles */}
        <Route path="/" element={<Navigate to="/dashboard" replace />} />

        {/* Layout wrapper */}
        <Route element={<MainLayout />}>
          <Route path="/dashboard" element={<DashboardPage />} />
          <Route path="/vehicles" element={<VehiclesPage />} />
          <Route path="/addvehicle" element={<AddVehiclePage />} />
          <Route path="/vehicles/:id" element={<ViewVehicle />} />
          <Route path="/vehicles/edit/:id" element={<EditVehiclePage />} />
          <Route path="/sales" element={<SalesPage />} />
          <Route path="/sales/add" element={<AddSalePage />} />
          <Route path="/testdrive" element={<TestDrivePage />} />
          <Route path="/testdrive/:id" element={<TestDriveViewPage />} />
          <Route path="/testdrive/add" element={<AddTestDrivePage />} />
          <Route path="/testdrive/:id/edit" element={<EditTestDrivePage />} />
          <Route path="/testdrive/customer" element={<TestDriveCustomerSelectPage />} />
          <Route path="/testdrive/vehicle" element={<TestDriveVehicleSelectPage />} />
          <Route path="/customers" element={<CustomersPage />} />
          <Route path="/customers/add" element={<AddCustomerPage />} />
          <Route path="/customers/:id" element={<CustomerDetailsPage />} />
          <Route path="/customers/:id/edit" element={<EditCustomerPage />} />


        </Route>

        {/* Fallback */}
        <Route path="*" element={<Navigate to="/vehicles" replace />} />
      </Routes>
    </BrowserRouter>
  );
};

export default AppRoutes;
