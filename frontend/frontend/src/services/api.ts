// const DEFAULT_BASE_URL = "http://localhost:3000";

// export const API_BASE_URL: string =
//   (import.meta as any).env?.VITE_API_BASE_URL || DEFAULT_BASE_URL;

// export type ApiError = {
//   status: number;
//   message: string;
//   raw?: unknown;
// };

// async function parseJsonSafe(res: Response): Promise<any> {
//   const text = await res.text();
//   if (!text) return null;
//   try {
//     return JSON.parse(text);
//   } catch {
//     return text;
//   }
// }

// export async function apiRequest<T>(
//   path: string,
//   options: RequestInit = {}
// ): Promise<T> {
//   const url = `${API_BASE_URL}${path}`;

//   const res = await fetch(url, {
//     ...options,
//     headers: {
//       "Content-Type": "application/json",
//       ...(options.headers || {}),
//     },
//   });

//   const data = await parseJsonSafe(res);
//   if (!res.ok) {
//     const message =
//       (data && typeof data === "object" && "error" in data && (data as any).error) ||
//       res.statusText ||
//       "Request failed";
//     const err: ApiError = { status: res.status, message, raw: data };
//     throw err;
//   }

//   return data as T;
// }
import axios from "axios";

const api=axios.create ({
    baseURL: import.meta.env.VITE_API_BASE_URL,
    headers:{
        "Content-Type":"application/json",
    },
});

export default api;
