
import "./InvoiceModal.css";
import type { Vehicle } from "../types/vehicle";

type CustomerLite = {
  id: string;
  first_name: string;
  last_name: string;
};

export type InvoiceData = {
  saleId: string;
  date: string; // YYYY-MM-DD
  salePrice: number;
  customer: CustomerLite;
  vehicle: Vehicle;
};

type Props = {
  open: boolean;
  invoice: InvoiceData | null;
  onClose: () => void;
};

const money = (n: number) =>
  n.toLocaleString(undefined, { style: "currency", currency: "USD" });

/**
 * Minimal PDF generator (no external libraries).
 * Produces a single-page invoice PDF with basic text layout.
 */
function buildInvoicePdf(invoice: InvoiceData): Uint8Array {
  const pageW = 612; // 8.5in * 72
  const pageH = 792; // 11in * 72
  const left = 48;

  const safe = (s: string) =>
    String(s).replace(/\\/g, "\\\\").replace(/\(/g, "\\(").replace(/\)/g, "\\)");

  const lines: Array<{ x: number; y: number; size: number; text: string }> = [];

  // Header
  lines.push({ x: left, y: pageH - 70, size: 22, text: "INVOICE" });
  lines.push({ x: left, y: pageH - 95, size: 10, text: "DealerDrive Auto Sales" });
  lines.push({ x: left, y: pageH - 110, size: 10, text: "123 King St W, Kitchener, ON" });
  lines.push({ x: left, y: pageH - 125, size: 10, text: "Phone: (555) 555-5555" });

  // Meta (right)
  lines.push({ x: 360, y: pageH - 95, size: 10, text: `Invoice #: ${invoice.saleId}` });
  lines.push({ x: 360, y: pageH - 110, size: 10, text: `Invoice Date: ${invoice.date}` });

  // Bill to
  lines.push({ x: left, y: pageH - 165, size: 12, text: "Bill To:" });
  lines.push({
    x: left,
    y: pageH - 185,
    size: 11,
    text: `${invoice.customer.first_name} ${invoice.customer.last_name}`,
  });

  // Vehicle details
  const startY = pageH - 235;
  lines.push({ x: left, y: startY, size: 12, text: "Vehicle Details" });

  const v = invoice.vehicle;
  const details = [
    `VIN: ${v.vin}`,
    `Vehicle: ${v.year} ${v.make} ${v.model}${v.trim ? " " + v.trim : ""}`,
    `Odometer: ${v.odometer.toLocaleString()} km`,
    `Transmission: ${String(v.transmission)}`,
    `Fuel Type: ${String(v.fuel_type)}`,
  ];

  let y = startY - 18;
  for (const d of details) {
    lines.push({ x: left, y, size: 10, text: d });
    y -= 14;
  }

  // Totals
  y -= 10;
  lines.push({ x: 360, y, size: 12, text: "Total" });
  lines.push({ x: 360, y: y - 18, size: 16, text: money(invoice.salePrice) });

  // Footer
  lines.push({
    x: left,
    y: 80,
    size: 9,
    text: "Thank you for your business. Please retain this invoice for your records.",
  });

  // Content stream
  const content = lines
    .map((l) => `BT /F1 ${l.size} Tf ${l.x} ${l.y} Td (${safe(l.text)}) Tj ET`)
    .join("\n") + "\n";

  const enc = new TextEncoder();

  const obj: string[] = [];
  // 1 catalog, 2 pages, 3 page, 4 contents, 5 font
  obj.push("<< /Type /Catalog /Pages 2 0 R >>");
  obj.push("<< /Type /Pages /Kids [3 0 R] /Count 1 >>");
  obj.push(
    `<< /Type /Page /Parent 2 0 R /MediaBox [0 0 ${pageW} ${pageH}] /Contents 4 0 R /Resources << /Font << /F1 5 0 R >> >> >>`
  );
  obj.push(`<< /Length ${enc.encode(content).length} >>\nstream\n${content}endstream`);
  obj.push("<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>");

  // Assemble file
  let pdf = "%PDF-1.4\n";
  const offsets: number[] = [0]; // xref requires object 0 entry

  const addObj = (n: number, body: string) => {
    offsets[n] = enc.encode(pdf).length;
    pdf += `${n} 0 obj\n${body}\nendobj\n`;
  };

  addObj(1, obj[0]);
  addObj(2, obj[1]);
  addObj(3, obj[2]);
  addObj(4, obj[3]);
  addObj(5, obj[4]);

  const xrefStart = enc.encode(pdf).length;
  pdf += "xref\n";
  pdf += `0 6\n`;
  // object 0
  pdf += "0000000000 65535 f \n";
  for (let i = 1; i <= 5; i++) {
    const off = offsets[i] ?? 0;
    pdf += `${String(off).padStart(10, "0")} 00000 n \n`;
  }
  pdf += "trailer\n";
  pdf += "<< /Size 6 /Root 1 0 R >>\n";
  pdf += "startxref\n";
  pdf += `${xrefStart}\n`;
  pdf += "%%EOF\n";

  return enc.encode(pdf);
}

function downloadPdf(invoice: InvoiceData) {
  const bytes = buildInvoicePdf(invoice);
  const blob = new Blob([bytes], { type: "application/pdf" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = `invoice_${invoice.saleId}.pdf`;
  document.body.appendChild(a);
  a.click();
  a.remove();
  URL.revokeObjectURL(url);
}

export default function InvoiceModal({ open, invoice, onClose }: Props) {
  if (!open || !invoice) return null;

  const v = invoice.vehicle;
  return (
    <div className="invOverlay" role="dialog" aria-modal="true">
      <div className="invModal">
        <div className="invHeader">
          <div>
            <h2 className="invTitle">Invoice</h2>
            <div className="invSub">Invoice #: {invoice.saleId}</div>
          </div>
          <button className="invClose" onClick={onClose} aria-label="Close">
            ✕
          </button>
        </div>

        <div className="invBody">
          <div className="invTwoCol">
            <div className="invCard">
              <div className="invCardTitle">Seller</div>
              <div>DealerDrive Auto Sales</div>
              <div>123 King St W</div>
              <div>Kitchener, ON</div>
              <div>Phone: (555) 555-5555</div>
            </div>

            <div className="invCard">
              <div className="invCardTitle">Bill To</div>
              <div>
                {invoice.customer.first_name} {invoice.customer.last_name}
              </div>
              <div className="invMuted">Customer ID: {invoice.customer.id}</div>
              <div className="invMuted">Invoice Date: {invoice.date}</div>
            </div>
          </div>

          <div className="invSection">
            <div className="invSectionTitle">Vehicle</div>
            <div className="invGrid">
              <div>
                <div className="invLabel">Vehicle</div>
                <div className="invValue">
                  {v.year} {v.make} {v.model}
                  {v.trim ? ` ${v.trim}` : ""}
                </div>
              </div>

              <div>
                <div className="invLabel">VIN</div>
                <div className="invValue">{v.vin}</div>
              </div>

              <div>
                <div className="invLabel">Odometer</div>
                <div className="invValue">{v.odometer.toLocaleString()} km</div>
              </div>

              <div>
                <div className="invLabel">Transmission</div>
                <div className="invValue">{String(v.transmission)}</div>
              </div>

              <div>
                <div className="invLabel">Fuel Type</div>
                <div className="invValue">{String(v.fuel_type)}</div>
              </div>
            </div>
          </div>

          <div className="invTotalRow">
            <div className="invTotalLabel">Total</div>
            <div className="invTotalValue">{money(invoice.salePrice)}</div>
          </div>
        </div>

        <div className="invActions">
          <button className="invBtnGhost" onClick={onClose}>
            Close
          </button>
          <button className="invBtnPrimary" onClick={() => downloadPdf(invoice)}>
            Download PDF
          </button>
        </div>
      </div>
    </div>
  );
}
