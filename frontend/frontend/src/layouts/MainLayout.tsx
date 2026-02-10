import { Outlet } from "react-router-dom";
// import Sidebar from "../components/Sidebar";
import Header from "../components/Header";

const MainLayout = () => {
  return (
    <div style={{ display: "flex", height: "100vh", width: "100%" }}>
      {/* Sidebar (optional but you already have it)
      <Sidebar /> */}

      <div style={{ flex: 1, display: "flex", flexDirection: "column" }}>
        <Header />

        <main style={{ width: "100%", flex: 1, overflow: "auto" }}>
          <Outlet />
        </main>
      </div>
    </div>
  );
};

export default MainLayout;
