import FeatureButton from "../components/FeatureButtons";
import {useState} from "react";
import Sidebar from "../components/Sidebar";

const DashboardPage = () => {
    const [showSidebar, setShowSidebar]=useState(false);
  return (
    <div style={{display:"flex"}}>
        {showSidebar && <Sidebar/>}
    <div style={{flex:1}}>
      <div
        style={{
          width: "100%",
          height: "60px",
          display: "flex",
          alignItems: "center",
          justifyContent: "space-between",
          borderBottom: "1px solid #ccc",
          padding: "0 15px",
          boxSizing: "border-box",
        }}
      >
        <div style={{ cursor: "pointer" }}
        onClick={()=>setShowSidebar(!showSidebar)}
        >☰ Menu</div>

        <div>
          <input
            type="text"
            placeholder="Search..."
            style={{
              padding: "6px 12px",
              width: "220px",
              borderRadius: "15px",
              border: "1px solid #ccc",
              outline: "none",
            }}
          />
        </div>

        <div style={{ display: "flex", alignItems: "center", gap: "8px", cursor: "pointer" }}>
          <div
            style={{
              width: "32px",
              height: "32px",
              borderRadius: "50%",
              background: "#ddd",
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
            }}
          >
            👤
          </div>
        </div>
      </div>

      <div style={{ marginTop: "2rem" }}>
        <div style={{ border: "1px dashed #ccc", height: "200px" }}>
          Thumbnail
        </div>
      </div>

      <FeatureButton />
      </div>
    </div>
  );
};

export default DashboardPage;
