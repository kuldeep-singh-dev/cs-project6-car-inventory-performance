import { Link, NavLink } from "react-router-dom";
import "./Header.css";

const Header = () => {
  return (
    <header className="topHeader">
      <div className="topHeaderLeft">
        <Link to="/dashboard" className="topHeaderBrand">
          Home
        </Link>
      </div>

      <nav className="topHeaderNav">
        <NavLink to="/vehicles" className={({ isActive }) => (isActive ? "navLink active" : "navLink")}>
          Vehicles
        </NavLink>
        <NavLink to="/customers" className={({ isActive }) => (isActive ? "navLink active" : "navLink")}>
          Customers
        </NavLink>
        <NavLink
          to="/sales"
          className={({ isActive }) =>
            isActive ? "navLink active" : "navLink"
          }
        >
          Sales
        </NavLink>
      </nav>
    </header>
  );
};

export default Header;
