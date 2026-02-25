import React from "react";
import { useNavigate } from "react-router-dom";
import customerService from "../../services/customerService";
import type { CustomerCreate } from "../../types/customer";
import CustomerForm from "./CustomerForm";

// Page for adding a new customer. 
// - Uses the CustomerForm component in "add" mode.

// When the form is submitted, it calls the customerService to create a new customer.
// Then navigates back to the customer list.
export default function AddCustomerPage() {
  const navigate = useNavigate();

  return (
    // Renders the CustomerForm component in "add" mode with empty initial values and handles form submission to create a new customer.
    <CustomerForm
      mode="add"
      title="Add Customer"
      submitLabel="Add Customer"
      onCancel={() => navigate("/customers")}
      onSubmit={async (payload) => {
        await customerService.create(payload as CustomerCreate);
        navigate("/customers");
      }}
    />
  );
}