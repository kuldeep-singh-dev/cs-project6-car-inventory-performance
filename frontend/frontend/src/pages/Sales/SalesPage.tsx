import { useEffect, useMemo, useState } from "react";
import { useNavigate } from "react-router-dom";
import "./SalesPage.css";
import { salesService } from "../../services/salesService";
import type { SaleSummary } from "../../types/sales";

const downloadBlob = (blob: Blob, filename: string) => {
  const url = window.URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  a.remove();
  window.URL.revokeObjectURL(url);
};

// Escape CSV values safely (commas/quotes/newlines)
const csvEscape = (value: unknown) => {
  const s = String(value ?? "");
  if (/[",\n]/.test(s)) return `"${s.replace(/"/g, '""')}"`;
  return s;
};

// Build CSV from the table rows you’re displaying
const buildTableCsv = (rows: SaleSummary[]) => {
  const header = ["Customer Name", "Vehicle Name", "Date", "Sales Price"];

  const lines = [
    header.map(csvEscape).join(","),
    ...rows.map((r) =>
      [
        r.customer,
        r.vehicle,
        r.date,
        Number(r.price).toFixed(2),
      ]
        .map(csvEscape)
        .join(",")
    ),
  ];

  return lines.join("\n");
};

const SalesPage = () => {
  const navigate = useNavigate();
  const [sales, setSales] = useState<SaleSummary[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  // Weekly report UI
  const [showWeeklyInputs, setShowWeeklyInputs] = useState(false);
  const [start, setStart] = useState("");
  const [end, setEnd] = useState("");

  const fetchSales = async () => {
    setLoading(true);
    setError(null);
    try {
      const data = await salesService.getAll();
      setSales(Array.isArray(data) ? data : []);
    } catch (e: any) {
      setError(e?.response?.data?.error || "Failed to load sales");
      setSales([]);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchSales();
  }, []);

  const rows = useMemo(() => sales, [sales]);

  // Export the same data as table
  const handleExportCsv = () => {
    const csv = buildTableCsv(rows);
    const blob = new Blob([csv], { type: "text/csv;charset=utf-8" });
    downloadBlob(blob, "sales_table_export.csv");
  };

  // Weekly report via backend CSV
  const handleWeeklyReportDownload = async () => {
    if (!start || !end) {
      alert("Please select start and end date.");
      return;
    }

    try {
      const blob = await salesService.weeklyReportCsv(start, end);
      downloadBlob(blob, `weekly_sales_report_${start}_to_${end}.csv`);
    } catch (e: any) {
      alert(e?.response?.data || "Weekly report failed.");
    }
  };

  const handleWeeklyButtonClick = () => {
    // first click: show the date inputs (calendar icons appear automatically)
    if (!showWeeklyInputs) {
      setShowWeeklyInputs(true);
      return;
    }
    // second click (or after selecting dates): download
    handleWeeklyReportDownload();
  };

  return (
    <div className="salesPage">
      <div className="salesTopBar">
        <div className="salesLeftActions">
          <button className="salesBtn" onClick={handleExportCsv}>
            Export CSV
          </button>

          <div className="weeklyBox">
            <button className="salesBtn" onClick={handleWeeklyButtonClick}>
              Weekly Report
            </button>

            {showWeeklyInputs && (
              <div className="weeklyInputs">
                <input
                  type="date"
                  value={start}
                  onChange={(e) => setStart(e.target.value)}
                  className="salesInput"
                  aria-label="Weekly report start date"
                />
                <input
                  type="date"
                  value={end}
                  onChange={(e) => setEnd(e.target.value)}
                  className="salesInput"
                  aria-label="Weekly report end date"
                />
              </div>
            )}
          </div>
        </div>

        {/* Make sure your route exists in AppRoutes */}
        <button className="salesBtnPrimary" onClick={() => navigate("/sales/add")}>
          Add
        </button>
      </div>

      <div className="salesHeader">
        <h1 className="salesTitle">Sales Record</h1>
      </div>

      {loading ? (
        <div className="salesState">Loading...</div>
      ) : error ? (
        <div className="salesState salesError">{error}</div>
      ) : (
        <div className="salesTableWrap">
          <table className="salesTable">
            <thead>
              <tr>
                <th>Customer Name</th>
                <th>Vehicle Name</th>
                <th>Date</th>
                <th>Sales Price</th>
              </tr>
            </thead>

            <tbody>
              {rows.length === 0 ? (
                <tr>
                  <td colSpan={4} className="salesEmpty">
                    No sales found.
                  </td>
                </tr>
              ) : (
                rows.map((s) => (
                  <tr key={s.sale_id} className="salesRow">
                    <td>{s.customer}</td>
                    <td>{s.vehicle}</td>
                    <td>{s.date}</td>
                    <td>${Number(s.price).toFixed(2)}</td>
                  </tr>
                ))
              )}
            </tbody>
          </table>
        </div>
      )}
    </div>
  );
};

export default SalesPage;
