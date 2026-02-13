import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { customerService } from "../../services/customerService";
import type { Customer } from "../../types/customer";
import "./TestDriveCustomerSelectPage.css";

const TestDriveCustomerSelectPage = () => {
  const navigate = useNavigate();
  const [customers, setCustomers] = useState<Customer[]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const load = async () => {
      try {
        const data = await customerService.getAll();
        setCustomers(data);
      } catch (err) {
        console.error("Failed to load customers:", err);
        setCustomers([]);
      } finally {
        setLoading(false);
      }
    };

    load();
  }, []);

  if (loading) return <p>Loading customers...</p>;

  return (
    <div className="tdSelectPage">
      <h1 className="tdSelectTitle">Select Customer</h1>

      <div className="tdTableWrap">
        <table className="td-table">
          <thead>
            <tr>
              <th>Name</th>
              <th>Select</th>
            </tr>
          </thead>

          <tbody>
            {customers.map((c) => (
              <tr key={c.id}>
                <td>{c.first_name} {c.last_name}</td>
                <td>
                  <button
                    className="tdBtnPrimary"
                    onClick={() =>
                      navigate("/testdrive", { state: { customerId: c.id } })
                    }
                  >
                    View Test Drives
                  </button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
};

export default TestDriveCustomerSelectPage;