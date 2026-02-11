import {BrowserRouter, Routes, Route, Navigate} from 'react-router-dom';
import MainLayout from "../layouts/MainLayout";

import VehiclesPage from "../pages/Vehicle/VehiclesPage";
import AddVehiclePage from "../pages/Vehicle/AddVehiclePage";
import ViewVehicle from "../pages/Vehicle/ViewVehicle";
import EditVehiclePage from "../pages/Vehicle/EditVehiclePage";
import DashboardPage from "../pages/DashboardPage";
import SalesPage from "../pages/Sales/SalesPage";
import AddSalePage from "../pages/Sales/AddSalesPage";

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
        </Route>

        {/* Fallback */}
        <Route path="*" element={<Navigate to="/vehicles" replace />} />
      </Routes>
    </BrowserRouter>
  );
};

export default AppRoutes;
