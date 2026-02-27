//This page lists all customers in a table.
// - allows navigation to add/edit pages. 
// - fetches customer data from the backend and handles loading/error states.

import React, { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { customerService } from "../../services/customerService";
import type { Customer } from "../../types/customer";
import "./CustomersPage.css";

// Displays a table of all customers with options to view details or add a new customer.
export default function CustomersPage() {
  // State for customer data, loading status, and error messages
  const [customers, setCustomers] = useState<Customer[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const navigate = useNavigate();

  // Fetch customers on component mount
  useEffect(() => {
    setLoading(true);
    setError(null);

    // Fetch all customers from the backend
    customerService
      .getAll()
      .then((data) => setCustomers(data))
      .catch(() => setError("Failed to load customers."))
      .finally(() => setLoading(false));
  }, []);

  return (
    <div className="custPage">
      <div className="custHeaderRow">
        <h1 className="custTitle">Customers</h1>
        <button className="custAddButton" onClick={() => navigate("/customers/add")}>
          + Add Customer
        </button>
      </div>

      {loading && <p>Loading...</p>}
      {error && <p style={{ color: "crimson", fontWeight: 700 }}>{error}</p>}

      {!loading && !error && (
        <div className="custTableWrap">
          <table className="custTable">
            <thead className="custTableHead">
              <tr>
                <th>Firstname</th>
                <th>Lastname</th>
                <th>Address</th>
                <th>Phone</th>
                <th>Email</th>
                <th>Licence</th>
              </tr>
            </thead>
            <tbody>
              {customers.map((c) => (
                <tr
                  key={c.id}
                  className="custRow"
                  onClick={() => navigate(`/customers/${c.id}`)}
                >
                  <td>{c.first_name}</td>
                  <td>{c.last_name}</td>
                  <td>{c.address ?? "—"}</td>
                  <td>{c.ph_number}</td>
                  <td>{c.email}</td>
                  <td>{c.driving_licence}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
    </div>
  );
}