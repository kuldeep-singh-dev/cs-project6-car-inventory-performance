import { useState, useEffect } from "react";
import { useNavigate } from "react-router-dom";
import { testDriveService } from "../../services/testDriveService";
import { customerService } from "../../services/customerService";
import { vehicleService } from "../../services/vehicleService";
import type { Customer } from "../../types/customer";
import type { Vehicle } from "../../types/vehicle";
import "./AddTestDrivePage.css";

const AddTestDrivePage = () => {
  const navigate = useNavigate();

  const [customers, setCustomers] = useState<Customer[]>([]);
  const [vehicles, setVehicles] = useState<Vehicle[]>([]);

  const [form, setForm] = useState({
    customerId: "",
    vehicleId: "",
    date: "",
    comment: "",
  });

  useEffect(() => {
    const load = async () => {
      const c = await customerService.getAll();
      const v = await vehicleService.getAll();
      setCustomers(c);
      setVehicles(v);
    };
    load();
  }, []);

  const handleChange = (
    e: React.ChangeEvent<
      HTMLInputElement | HTMLSelectElement | HTMLTextAreaElement
    >
  ) => {
    setForm({ ...form, [e.target.name]: e.target.value });
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    const created = await testDriveService.create(form);
    navigate(`/testdrive/${created.id}`);
  };

  return (
    <div className="addTDPage">
      <h1 className="addTDTitle">Add Test Drive</h1>

      <form className="addTDForm" onSubmit={handleSubmit}>
        <label className="addTDLabel">Customer</label>
        <select
          name="customerId"
          value={form.customerId}
          onChange={handleChange}
          className="addTDSelect"
        >
          <option value="">Select Customer</option>
          {customers.map((c) => (
            <option key={c.id} value={c.id}>
              {c.first_name} {c.last_name}
            </option>
          ))}
        </select>

        <label className="addTDLabel">Vehicle</label>
        <select
          name="vehicleId"
          value={form.vehicleId}
          onChange={handleChange}
          className="addTDSelect"
        >
          <option value="">Select Vehicle</option>
          {vehicles.map((v) => (
            <option key={v.id} value={v.id}>
              {v.make} {v.model} ({v.year})
            </option>
          ))}
        </select>

        <label className="addTDLabel">Date & Time</label>
        <input
          type="date"
          name="date"
          value={form.date}
          onChange={handleChange}
          className="addTDInput"
        />

        <label className="addTDLabel">Comment</label>
        <textarea
          name="comment"
          placeholder="Notes"
          value={form.comment}
          onChange={handleChange}
          className="addTDTextarea"
        />

        <button type="submit" className="addTDSaveBtn">
          Save
        </button>
      </form>
    </div>
  );
};

export default AddTestDrivePage;