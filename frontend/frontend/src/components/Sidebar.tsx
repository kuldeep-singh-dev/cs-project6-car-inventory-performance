import { NavLink } from "react-router-dom";
import "./Sidebar.css";

type SidebarProps = {
  variant?: "desktop" | "mobile";
  onNavigate?: () => void;
};

const navItems = [
  { to: "/dashboard", label: "Dashboard", icon: "🏠" },
  { to: "/vehicles", label: "Vehicles", icon: "🚗" },
  { to: "/sales", label: "Sales", icon: "🧾" },
  { to: "/testdrive", label: "Test Drive", icon: "🛣️" },
  { to: "/customers", label: "Customers", icon: "👤" },
];

const Sidebar = ({ variant = "desktop", onNavigate }: SidebarProps) => {
  return (
    <aside className={variant === "desktop" ? "sidebar desktop" : "sidebar mobile"}>
      <div className="sideBrand">
        <div className="sideMark" />
        <div className="sideText">
          <strong>DealerDocs</strong>
          <span>Car inventory dashboard</span>
        </div>
      </div>

      <nav className="sideNav">
        {navItems.map((item) => (
          <NavLink
            key={item.to}
            to={item.to}
            onClick={onNavigate}
            className={({ isActive }) => (isActive ? "sideLink active" : "sideLink")}
          >
            <span className="sideIcon" aria-hidden="true">
              {item.icon}
            </span>
            <span>{item.label}</span>
          </NavLink>
        ))}
      </nav>

      <div className="sideFooter">
        <div className="sideHint">
          <div className="dot" />
          <span className="muted">Tip: use the search bar to find by VIN.</span>
        </div>
      </div>
    </aside>
  );
};

export default Sidebar;
