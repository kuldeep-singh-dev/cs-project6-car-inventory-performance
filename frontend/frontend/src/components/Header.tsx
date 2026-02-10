import {Link} from "react-router-dom";
const Header=()=>{
    return(
        <div
      style={{
        width: "100%",
        height: "60px",
        display: "flex",
        alignItems: "center",
        justifyContent: "space-between",
        padding: "0 15px",
        boxSizing: "border-box",
        borderBottom: "1px solid #ccc",
        flexWrap: "nowrap",
      }}
    >
      
      <div style={{ display: "flex", alignItems: "center", whiteSpace: "nowrap" }}>
        <Link to="/dashboard">Home</Link>
      </div>

   
      <div
        style={{
          display: "flex",
          alignItems: "center",
          gap: "20px",
          whiteSpace: "nowrap",
        }}
      >
        <Link to="/vehicles">Vehicle</Link>
        <Link to="/customers">Customers</Link>
        <Link to="/settings">Settings</Link>
      </div>
    </div>
    );
};

export default Header;