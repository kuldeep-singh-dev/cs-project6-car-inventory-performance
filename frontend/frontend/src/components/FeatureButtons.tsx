import { useNavigate } from "react-router-dom";
import "./FeatureButtons.css";

const FeatureButton = () => {
  const navigate = useNavigate();

  const items = [
    { title: "Vehicle Inventory", to: "/vehicles", icon: "🚗" },
    { title: "Sales",  to: "/sales", icon: "🧾" },
    { title: "Test Drives", to: "/testdrive", icon: "🛣️" },
    { title: "Customers", to: "/customers", icon: "👤" },
  ];

  return (
    <div className="featureGrid">
      {items.map((it) => (
        <button key={it.title} className="featureCard" onClick={() => navigate(it.to)}>
          <div className="featureIcon" aria-hidden="true">{it.icon}</div>
          <div className="featureText">
            <div className="featureTitle">{it.title}</div>
            <div className="featureDesc">{it.desc}</div>
          </div>
          <div className="featureArrow" aria-hidden="true">→</div>
        </button>
      ))}
    </div>
  );
};

export default FeatureButton;
