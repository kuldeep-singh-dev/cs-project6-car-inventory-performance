import { Outlet, useLocation } from "react-router-dom";
import { useMemo, useState } from "react";
import Header from "../components/Header";
import Sidebar from "../components/Sidebar";
import "./MainLayout.css";

const titleFromPath = (pathname: string) => {
  const p = pathname.split("?")[0];
  if (p === "/dashboard") return "Dashboard";
  if (p.startsWith("/vehicles")) return "Vehicles";
  if (p.startsWith("/sales")) return "Sales";
  if (p.startsWith("/testdrive")) return "Test Drive";
  if (p.startsWith("/customers")) return "Customers";
  return "DealerDocs";
};

const MainLayout = () => {
  // Sidebar is closed by default on ALL screen sizes (per your request)
  const [sidebarOpen, setSidebarOpen] = useState(false);
  const location = useLocation();

  const pageTitle = useMemo(() => titleFromPath(location.pathname), [location.pathname]);

  return (
    <div className="shell">
      {/* Sidebar drawer (all screen sizes) */}
      <div
        className={sidebarOpen ? "sidebarOverlay open" : "sidebarOverlay"}
        onClick={() => setSidebarOpen(false)}
      />
      <div className={sidebarOpen ? "sidebarDrawer open" : "sidebarDrawer"}>
        <Sidebar onNavigate={() => setSidebarOpen(false)} />
      </div>

      <div className="mainArea">
        <Header title={pageTitle} onToggleSidebar={() => setSidebarOpen((s) => !s)} />
        <main className="content">
          <div className="container page">
            <Outlet />
          </div>
        </main>
      </div>
    </div>
  );
};

export default MainLayout;
