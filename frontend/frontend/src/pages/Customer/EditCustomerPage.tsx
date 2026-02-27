// This page allows editing an existing customer's details.
// - Fetches the customer's current data and pre-fills the form.
// - On submit, sends a PATCH request with only the changed fields to update the customer.
import React, { useEffect, useState } from "react";
import { useNavigate, useParams } from "react-router-dom";
import { customerService } from "../../services/customerService";
import type { Customer, CustomerUpdate } from "../../types/customer";
import CustomerForm from "./CustomerForm";

// EditCustomerPage component for editing an existing customer's details.
export default function EditCustomerPage() {
  // Get the customer ID from the URL parameters and initialize navigation
  const { id } = useParams<{ id: string }>();
  const navigate = useNavigate();

  // State for the customer data and loading status
  const [customer, setCustomer] = useState<Customer | null>(null);
  const [loading, setLoading] = useState(true);

  // Fetches the customer's current data on component mount
  useEffect(() => {
    if (!id) return;

    // Fetches the customer data from the backend using the ID and handle loading state
    setLoading(true);
    customerService
      .getById(id)
      .then(setCustomer)
      .catch(() => setCustomer(null))
      .finally(() => setLoading(false));
  }, [id]);

  // If no ID is provided in the URL, returns null
  if (!id) return null;

  // Handles loading state, customer not found, and renders the CustomerForm with pre-filled data for editing
  if (loading) {
    return (
      <div style={{ padding: 24 }}>
        <h2>Loading...</h2>
      </div>
    );
  }

  // If the customer is not found, displays an error message and a back button.
  if (!customer) {
    return (
      <div style={{ padding: 24 }}>
        <h2>Customer not found.</h2>
        <button onClick={() => navigate("/customers")}>Back</button>
      </div>
    );
  }

  return (
    // Renders the CustomerForm component in "edit" mode with pre-filled values and handles form submission to update the customer.
    <CustomerForm
      mode="edit"
      title="Edit Customer"
      submitLabel="Save Changes"
      initialValues={{
        first_name: customer.first_name,
        last_name: customer.last_name,
        address: customer.address ?? "",
        ph_number: customer.ph_number,
        email: customer.email,
        driving_licence: customer.driving_licence,
      }}
      onCancel={() => navigate(`/customers/${id}`)}
      onSubmit={async (payload) => {
        await customerService.patch(id, payload as CustomerUpdate);
        navigate(`/customers/${id}`);
      }}
    />
  );
}