import { NavLink } from "react-router-dom";
import "./Header.css";

type HeaderProps = {
  title: string;
  onToggleSidebar: () => void;
};

const Header = ({ title, onToggleSidebar }: HeaderProps) => {
  return (
    <header className="topbar">
      <button className="iconBtn" onClick={onToggleSidebar} aria-label="Open menu">
        ☰
      </button>

      <div className="topbarTitle">
        <div className="titleText">{title}</div>
      </div>

      <div className="topbarRight">
        <nav className="quickNav" aria-label="Quick navigation">
          <NavLink to="/vehicles" className={({isActive}) => isActive ? "quickLink active" : "quickLink"}>Vehicles</NavLink>
          <NavLink to="/sales" className={({isActive}) => isActive ? "quickLink active" : "quickLink"}>Sales</NavLink>
          <NavLink to="/customers" className={({isActive}) => isActive ? "quickLink active" : "quickLink"}>Customers</NavLink>
          <NavLink to="/testdrive" className={({isActive}) => isActive ? "quickLink active" : "quickLink"}>Test Drive</NavLink>
        </nav>
      </div>
    </header>
  );
};

export default Header;
