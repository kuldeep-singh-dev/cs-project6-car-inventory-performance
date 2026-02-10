import { useNavigate } from "react-router-dom";

const FeatureButton=()=>{
    const navigate = useNavigate();
    return(
        <div style={{display:"flex", flexWrap:"wrap",justifyContent:"center", gap:"20px", marginTop:"40px",}}>
        <button onClick={() => navigate("/vehicles")}>
          Vehicle Inventory
        </button>

        <button onClick={()=>navigate("/customers")}>
            Customers
        </button>
        <button onClick={()=>navigate("/sales")}>Sales</button>
        <button onClick={()=>navigate("/records")}>Record</button>
        </div>
        
    );
};

export default FeatureButton;