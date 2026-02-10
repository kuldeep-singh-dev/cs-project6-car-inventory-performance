import { Link } from "react-router-dom";

const Sidebar=()=>{
    return (
        <aside style={{width:"200px", padding: "1rem", borderRight: "1px solid #ddd"}}>
            <h3>DealerDocs</h3>
            <nav>
                <ul>
                    <li><Link to="/dashboard">Dashboard</Link></li>
                    <li><Link to="/vehicles">Vehicles</Link></li>
                    <li><Link to="/customers">Customers</Link></li>
                    <li><Link to="/sales">Sales</Link></li>
                    <li><Link to="/records">Records</Link></li>
                    <li><Link to="/documents">Documents</Link></li>
                    <li><Link to="/settings">Settings</Link></li>
                    
                </ul>
            </nav>
        </aside>
    );
};

export default Sidebar;