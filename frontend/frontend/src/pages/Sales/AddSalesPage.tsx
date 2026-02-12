import { useEffect, useMemo, useState } from "react";
import { useNavigate } from "react-router-dom";
import "./AddSalesPage.css";
import { salesService } from "../../services/salesService";
import { vehicleService } from "../../services/vehicleService";
import type { Vehicle } from "../../types/vehicle";
import { customerService } from "../../services/customerService";


/** Minimal customer shape (adjust to your real Customer type if you already have it) */
type CustomerLite = {
  id: string;
  first_name: string;
  last_name: string;
};

const AddSalePage = () => {
  const navigate = useNavigate();

  const [vehicles, setVehicles] = useState<Vehicle[]>([]);
  const [customers, setCustomers] = useState<CustomerLite[]>([]);

  const [vehicleId, setVehicleId] = useState("");
  const [customerId, setCustomerId] = useState("");
  const [date, setDate] = useState("");
  const [salePrice, setSalePrice] = useState<string>("");

  const [loading, setLoading] = useState(true);
  const [saving, setSaving] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const selectedVehicle = useMemo(
    () => vehicles.find((v) => v.id === vehicleId) || null,
    [vehicles, vehicleId]
  );

  const fetchFormData = async () => {
    setLoading(true);
    setError(null);
    try {
      const v = await vehicleService.getAll();
      setVehicles(Array.isArray(v) ? v : []);


    const c = await customerService.getAll();
     setCustomers(Array.isArray(c) ? c : []);

    } catch (e: any) {
      setError(e?.response?.data || "Failed to load form data");
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchFormData();
  }, []);

  const handleGenerate = async () => {
    setError(null);

    if (!customerId || !vehicleId || !date || !salePrice) {
      setError("Please fill all required fields.");
      return;
    }

    const price = Number(salePrice);
    if (Number.isNaN(price) || price <= 0) {
      setError("Purchase price must be a positive number.");
      return;
    }

    setSaving(true);
    try {
      const created = await salesService.create({
        customer_id: customerId,
        vehicle_id: vehicleId,
        date,
        sale_price: price,
      });

      // Later: navigate to invoice page and generate PDF
      // For now: go back to list
      navigate("/sales");
      console.log("Created sale:", created);
    } catch (e: any) {
      setError(e?.response?.data?.error || e?.response?.data || "Failed to create sale");
    } finally {
      setSaving(false);
    }
  };

  if (loading) return <div className="addSaleState">Loading...</div>;

  return (
    <div className="addSalePage">
      <h1 className="addSaleTitle">Generate Bill of Sale</h1>

      {error && <div className="addSaleError">{error}</div>}

      <div className="addSaleGrid">
        <div className="addSaleField">
          <label>Select Customer</label>
          <select
            value={customerId}
            onChange={(e) => setCustomerId(e.target.value)}
          >
            <option value="">Choose...</option>
            {customers.map((c) => (
              <option key={c.id} value={c.id}>
                {c.first_name} {c.last_name}
              </option>
            ))}
          </select>
          {customers.length === 0 && (
            <small className="hint">
              Customers not loaded yet (add your customerService, then we’ll connect it).
            </small>
          )}
        </div>

        <div className="addSaleField">
          <label>Select Vehicle</label>
          <select
            value={vehicleId}
            onChange={(e) => setVehicleId(e.target.value)}
          >
            <option value="">Choose...</option>
            {vehicles.map((v) => (
              <option key={v.id} value={v.id}>
                {v.make} {v.model} ({v.year})
              </option>
            ))}
          </select>
        </div>

        <div className="addSaleField">
          <label>Sale Date</label>
          <input type="date" value={date} onChange={(e) => setDate(e.target.value)} />
        </div>

        <div className="addSaleField">
          <label>Purchase Price ($)</label>
          <input
            type="number"
            value={salePrice}
            onChange={(e) => setSalePrice(e.target.value)}
            placeholder="e.g. 25000"
          />
        </div>

        <div className="addSaleField addSaleWide">
          <label>Odometer Reading</label>
          <input
            type="text"
            value={selectedVehicle ? String(selectedVehicle.odometer) : ""}
            placeholder="Select a vehicle"
            disabled
          />
        </div>
      </div>

      <div className="addSaleActions">
        <button className="btnGhost" onClick={() => navigate("/sales")} disabled={saving}>
          Cancel
        </button>

        <button className="btnPrimary" onClick={handleGenerate} disabled={saving}>
          {saving ? "Generating..." : "Generate"}
        </button>
      </div>
    </div>
  );
};

export default AddSalePage;
