import { Outlet } from "react-router-dom";
import Header from "../components/Header";
import "./MainLayout.css";

const MainLayout = () => {
  return (
    <div className="appShell">
      <Header />
      <main className="appMain">
        <Outlet />
      </main>
    </div>
  );
};

export default MainLayout;
