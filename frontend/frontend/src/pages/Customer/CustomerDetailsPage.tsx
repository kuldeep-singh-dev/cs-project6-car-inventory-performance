// This page shows details of a single customer. 
// It is rendered when the user clicks on a customer in the CustomersPage.
import React, { useEffect, useState } from "react";
import { useNavigate, useParams } from "react-router-dom";
import { customerService } from "../../services/customerService";
import type { Customer } from "../../types/customer";
import "./CustomersPage.css";

// The component fetches the customer details using the ID from the URL params and displays them.
export default function CustomerDetailsPage() {
  // Get the customer ID from the URL parameters and set up navigation.
  const { id } = useParams<{ id: string }>();
  const navigate = useNavigate();

  // State to hold the customer details, loading status, and any error message.
  const [customer, setCustomer] = useState<Customer | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  // Fetch the customer details when the component mounts or when the ID changes.
  useEffect(() => {
    if (!id) return;

    setLoading(true);
    setError(null);

    // Fetch the customer details from the service using the ID.
    customerService
      .getById(id)
      .then((data) => setCustomer(data))
      .catch(() => setError("Failed to load customer details."))
      .finally(() => setLoading(false));
  }, [id]);

  if (!id) return null;

  // Render loading state, error state, or the customer details based on the current state.
  if (loading) {
    return (
      <div className="custDetailsPage">
        <h2>Loading...</h2>
      </div>
    );
  }

  // If there was an error fetching the customer details or if the customer was not found, show an error message and a back button.
  if (error || !customer) {
    return (
      <div className="custDetailsPage">
        <h2>{error ?? "Customer not found."}</h2>
        <button className="custBtnSecondary" onClick={() => navigate("/customers")}>
          Back
        </button>
      </div>
    );
  }

  return (
    <div className="custDetailsPage">
      <div className="custDetailsHeader">
        <h1 className="custDetailsTitle">Customer Details</h1>

        <div className="custDetailsActions">
          <button className="custBtnSecondary" onClick={() => navigate("/customers")}>
            Back
          </button>
          {/* matches AppRoutes: /customers/:id/edit */}
          <button className="custBtnPrimary" onClick={() => navigate(`/customers/${id}/edit`)}>
            Edit
          </button>
        </div>
      </div>

      <div className="custDetailsCard">
        <div className="custDetailsGrid">
          <Detail label="First name" value={customer.first_name} />
          <Detail label="Last name" value={customer.last_name} />
          <Detail label="Phone" value={customer.ph_number} />
          <Detail label="Email" value={customer.email} />
          <Detail label="Driving licence" value={customer.driving_licence} />
          <Detail label="Address" value={customer.address ? customer.address : "—"} />
        </div>
      </div>
    </div>
  );
}

function Detail({ label, value }: { label: string; value: string }) {
  return (
    <div className="custDetailsItem">
      <div className="custDetailsLabel">{label}</div>
      <div className="custDetailsValue">{value}</div>
    </div>
  );
}