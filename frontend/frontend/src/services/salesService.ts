// import api from "./api";
// import type {
//   SaleSummary,
//   SaleCreatePayload,
//   SaleCreateResponse,
//   SaleUpdatePayload,
// } from "../types/sales";

// export const salesService = {
//   getAll: async (): Promise<SaleSummary[]> => {
//     const res = await api.get<SaleSummary[]>("/sales");
//     return res.data;
//   },

//   create: async (payload: SaleCreatePayload): Promise<SaleCreateResponse> => {
//     const res = await api.post<SaleCreateResponse>("/sales", payload);
//     return res.data;
//   },

//   update: async (saleId: string, payload: SaleUpdatePayload): Promise<SaleCreateResponse> => {
//     const res = await api.put<SaleCreateResponse>(`/sales/${saleId}`, payload);
//     return res.data;
//   },

//   // Invoice JSON (later you’ll generate PDF from this)
//   getInvoice: async (saleId: string) => {
//     const res = await api.get(`/sales/id/${saleId}`);
//     return res.data;
//   },

//   // CSV downloads
//   exportAllCsv: async (): Promise<Blob> => {
//     const res = await api.get("/sales/export/csv", { responseType: "blob" });
//     return res.data;
//   },

//   weeklyReportCsv: async (start: string, end: string): Promise<Blob> => {
//     const res = await api.get("/sales/weekly-report", {
//       params: { start, end },
//       responseType: "blob",
//     });
//     return res.data;
//   },
// };
import api from "./api";
import type {
  SaleSummary,
  SaleCreatePayload,
  SaleCreateResponse,
  SaleUpdatePayload,
} from "../types/sales";

export const salesService = {
  // For the table
  getAll: async (): Promise<SaleSummary[]> => {
    const res = await api.get<SaleSummary[]>("/sales");
    return res.data;
  },

  // Create sale (for your Add Sale page later)
  create: async (payload: SaleCreatePayload): Promise<SaleCreateResponse> => {
    const res = await api.post<SaleCreateResponse>("/sales", payload);
    return res.data;
  },

  update: async (
    saleId: string,
    payload: SaleUpdatePayload
  ): Promise<SaleCreateResponse> => {
    const res = await api.put<SaleCreateResponse>(`/sales/${saleId}`, payload);
    return res.data;
  },

  // Export ALL sales from backend (not required for “table export”, but kept)
  exportAllCsv: async (): Promise<Blob> => {
    const res = await api.get("/sales/export/csv", { responseType: "blob" });
    return res.data;
  },

  // Weekly report CSV from backend
  weeklyReportCsv: async (start: string, end: string): Promise<Blob> => {
    const res = await api.get(`/sales/weekly-report?start=${start}&end=${end}`, {
      responseType: "blob",
    });
    return res.data;
  },
};
