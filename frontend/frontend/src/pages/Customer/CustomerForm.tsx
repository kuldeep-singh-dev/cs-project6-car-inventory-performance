import React, { useMemo, useState } from "react";
import type { CustomerCreate, CustomerUpdate } from "../../types/customer";
import "./CustomerForm.css";

/// This component is used for BOTH adding and editing customers.
type CustomerFormMode = "add" | "edit";

// Form values used for both add (POST) and edit (PATCH) payloads.
type CustomerFormValues = {
  first_name: string;
  last_name: string;
  address?: string;
  ph_number: string;
  email: string;
  driving_licence: string;
};

// Props for the CustomerForm component
type Props = {
  mode: CustomerFormMode;
  title: string;
  initialValues?: Partial<CustomerCreate>;
  submitLabel: string;
  onCancel: () => void;
  onSubmit: (payload: CustomerCreate | CustomerUpdate) => Promise<void>;
};

// Validation regex patterns
// Driving licence must be in format ON-12345678 (ON- followed by 8 digits)
const LICENCE_REGEX = /^ON-\d{8}$/;

// Phone number must be exactly 10 digits (no formatting)
const PHONE_REGEX = /^\d{10}$/;

// Simple email regex for basic validation.
const EMAIL_REGEX = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;

// Normalization functions to trim whitespace and standardize formats
function normalizeText(v: string) {
  return (v ?? "").trim();
}

// For driving licence, also convert to uppercase for consistency
function normalizeLicence(v: string) {
  return normalizeText(v).toUpperCase();
}

// This component handles both adding and editing customers with shared form logic and validation.
export default function CustomerForm({
  mode,
  title,
  submitLabel,
  initialValues,
  onCancel,
  onSubmit,
}: Props) {
  // Form state
  const [first_name, setFirstName] = useState(initialValues?.first_name ?? "");
  const [last_name, setLastName] = useState(initialValues?.last_name ?? "");
  const [address, setAddress] = useState(initialValues?.address ?? "");
  const [ph_number, setPhone] = useState(initialValues?.ph_number ?? "");
  const [email, setEmail] = useState(initialValues?.email ?? "");
  const [driving_licence, setLicence] = useState(initialValues?.driving_licence ?? "");

  // UX control
  const [attempted, setAttempted] = useState(false);
  const [submitting, setSubmitting] = useState(false);
  const [formError, setFormError] = useState<string | null>(null);

  // Normalize form values for validation and submission
  // useMemo to avoid unnecessary recalculations on every render
  const normalized = useMemo(() => {
    return {
      first_name: normalizeText(first_name),
      last_name: normalizeText(last_name),
      address: normalizeText(address), // optional
      ph_number: normalizeText(ph_number),
      email: normalizeText(email),
      driving_licence: normalizeLicence(driving_licence),
    };
  }, [first_name, last_name, address, ph_number, email, driving_licence]);

  const phoneValid = PHONE_REGEX.test(normalized.ph_number);
  const emailValid = EMAIL_REGEX.test(normalized.email);
  const licenceValid = LICENCE_REGEX.test(normalized.driving_licence);

  const requiredMissing =
    normalized.first_name.length === 0 ||
    normalized.last_name.length === 0 ||
    normalized.ph_number.length === 0 ||
    normalized.email.length === 0 ||
    normalized.driving_licence.length === 0;


  // Handle form submission for both add and edit modes with appropriate validation and payload construction.
  async function handleSubmit(e: React.FormEvent) {
    e.preventDefault();
    setAttempted(true);
    setFormError(null);

    /**
     * Validation for BOTH add & edit.
     * - For add: all required fields must be present and valid.
     */
    if (requiredMissing || !phoneValid || !emailValid || !licenceValid) {
      setFormError("Please fix the highlighted fields.");
      return;
    }

    setSubmitting(true);
    try {
      if (mode === "add") {
        // POST payload (address omitted when empty)
        const payload: CustomerFormValues = {
          first_name: normalized.first_name,
          last_name: normalized.last_name,
          ph_number: normalized.ph_number,
          email: normalized.email,
          driving_licence: normalized.driving_licence,
          ...(normalized.address ? { address: normalized.address } : {}),
        };

        await onSubmit(payload);
        return;
      }

      /**
       * mode === "edit"
       * Builds PATCH payload with ONLY changed fields to avoid overwriting unchanged fields with potentially "invalid" values
       */
      const base = {
        first_name: normalizeText(initialValues?.first_name ?? ""),
        last_name: normalizeText(initialValues?.last_name ?? ""),
        address: normalizeText(initialValues?.address ?? ""),
        ph_number: normalizeText(initialValues?.ph_number ?? ""),
        email: normalizeText(initialValues?.email ?? ""),
        driving_licence: normalizeLicence(initialValues?.driving_licence ?? ""),
      };

      // Construct patch payload with only changed fields
      const patchPayload: Partial<CustomerFormValues> = {};

      if (normalized.first_name !== base.first_name) patchPayload.first_name = normalized.first_name;
      if (normalized.last_name !== base.last_name) patchPayload.last_name = normalized.last_name;

      // Address optional field
      if (normalized.address !== base.address) patchPayload.address = normalized.address;

      if (normalized.ph_number !== base.ph_number) patchPayload.ph_number = normalized.ph_number;
      if (normalized.email !== base.email) patchPayload.email = normalized.email;
      if (normalized.driving_licence !== base.driving_licence)
        patchPayload.driving_licence = normalized.driving_licence;

      // If nothing changed, just goes back (no PATCH call)
      if (Object.keys(patchPayload).length === 0) {
        onCancel();
        return;
      }

      await onSubmit(patchPayload);
    } catch {
      setFormError("Failed to save customer. Try again.");
    } finally {
      setSubmitting(false);
    }
  }

  return (
    <div className="custPage">
      <div className="custCard">
        <div className="custHeaderRow">
          <h1 className="custTitle">{title}</h1>

          <div className="custActions">
            <button type="button" className="btnSecondary" onClick={onCancel}>
              Cancel
            </button>
            <button
              type="submit"
              form="customer-form"
              className="btnPrimary"
              disabled={submitting}
            >
              {submitting ? "Saving..." : submitLabel}
            </button>
          </div>
        </div>

        <form id="customer-form" className="custForm" onSubmit={handleSubmit}>
          {formError && <div className="custError">{formError}</div>}

          <div className="custGrid">
            <label className="custField">
              <span className="custLabel">
                First name <span className="req">*</span>
              </span>
              <input
                value={first_name}
                onChange={(e) => setFirstName(e.target.value)}
                className="custInput"
                autoFocus
              />
              {attempted && normalized.first_name.length === 0 && (
                <span className="custHint warn">First name is required.</span>
              )}
            </label>

            <label className="custField">
              <span className="custLabel">
                Last name <span className="req">*</span>
              </span>
              <input
                value={last_name}
                onChange={(e) => setLastName(e.target.value)}
                className="custInput"
              />
              {attempted && normalized.last_name.length === 0 && (
                <span className="custHint warn">Last name is required.</span>
              )}
            </label>

            <label className="custField custSpan2">
              <span className="custLabel">Address</span>
              <input
                value={address}
                onChange={(e) => setAddress(e.target.value)}
                placeholder="Optional"
                className="custInput"
              />
              <span className="custHint"></span>
            </label>

            <label className="custField">
              <span className="custLabel">
                Phone number <span className="req">*</span>
              </span>
              <input
                value={ph_number}
                onChange={(e) => setPhone(e.target.value)}
                placeholder="e.g., 4165551234"
                className="custInput"
              />
              {attempted && !phoneValid && (
                <span className="custHint warn">Phone must be 10 digits.</span>
              )}
            </label>

            <label className="custField">
              <span className="custLabel">
                Email <span className="req">*</span>
              </span>
              <input
                value={email}
                onChange={(e) => setEmail(e.target.value)}
                placeholder="e.g., name@email.com"
                className="custInput"
              />
              {attempted && !emailValid && (
                <span className="custHint warn">Enter a valid email.</span>
              )}
            </label>

            <label className="custField custSpan2">
              <span className="custLabel">
                Driving licence <span className="req">*</span>
              </span>
              <input
                value={driving_licence}
                onChange={(e) => setLicence(e.target.value)}
                placeholder="ON-12345678"
                className="custInput"
              />
              {attempted && !licenceValid && (
                <span className="custHint warn">
                  Driving licence must match <b>ON-12345678</b> (ON- + 8 digits).
                </span>
              )}
            </label>
          </div>
        </form>
      </div>
    </div>
  );
}