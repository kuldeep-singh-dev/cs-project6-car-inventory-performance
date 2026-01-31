# Dealership_Dashboard - Car Inventory & Sales Backend

This project is a C++ backend service using **Crow** and **PostgreSQL** for managing car inventory, sales, and test drives. Docker is used for running the frontend, the backend and the database.

---

## Prerequisites

- Docker Desktop installed
- Git installed
- Optional: CMake & GCC locally if you want to build outside Docker

---

## Project Structure

```
CSProject6/                         # Root project directory
├─ frontend/                        # Frontend application (details omitted)
├─ backend/                         # Backend (C++ / Crow / PostgreSQL)
│  ├─ CMakeLists.txt                # CMake build configuration for backend
│
│  ├─ docker/                       # Docker-related files
│  │  ├─ Dockerfile                 # Backend service image definition
│  │  ├─ docker-compose.yml         # Multi-container setup (backend + database)
│  │  └─ init.sql                   # Database initialization script (schema + seed)
│
│  ├─ sql/                          # Raw SQL scripts (non-Docker usage)
│  │  ├─ schema.sql                 # Database schema definitions
│  │  └─ seed.sql                   # Database seed data
│
│  ├─ src/                          # Backend source code
│  │  ├─ main.cpp                   # Application entry point (Crow app setup)
│  │
│  │  ├─ db/                        # Database access layer
│  │  │  ├─ db_connection.h         # Database connection interface
│  │  │  └─ db_connection.cpp       # PostgreSQL (libpqxx) implementation
│  │
│  │  ├─ external/                  # Third-party libraries
│  │  │  └─ crow/
│  │  │     └─ crow_all.h            # Crow (header-only) web framework
│  │
│  │  ├─ modules/                   # Feature-based API modules
│  │  │  ├─ customer/
│  │  │  │  ├─ customer.h
│  │  │  │  └─ customer.cpp
│  │  │  ├─ inventory/
│  │  │  │  ├─ inventory.h
│  │  │  │  └─ inventory.cpp
│  │  │  ├─ sales/
│  │  │  │  ├─ sales.h
│  │  │  │  └─ sales.cpp
│  │  │  └─ test_drive/
│  │  │     ├─ test_drive.h
│  │  │     └─ test_drive.cpp
│
│  ├─ test/                         # Unit and module tests
│  │  ├─ customer_tests/
│  │  │  └─ customer_test.cpp
│  │  ├─ inventory_tests/
│  │  │  └─ inventory_test.cpp
│  │  ├─ sales_tests/
│  │  │  └─ sales_test.cpp
│  │  └─ test_drive_tests/
│  │     └─ test_drive_test.cpp
│
├─ .gitignore                       # Git ignore rules
└─ README.md                        # Project documentation

```

---

## Running with Docker

All commands below assume you are in the `backend/docker` folder.

### 1️⃣ Start Docker Desktop

Make sure Docker Desktop is running.

---

### 2️⃣ Build & Run containers

```bash
# Navigate to docker folder
cd backend/docker

# Build and start containers
docker compose up --build
```

This will start two services:
- `dealerdrive-db` → PostgreSQL database
- `dealerdrive-backend` → C++ backend using Crow

The backend will be available at `http://localhost:3000` and PostgreSQL at port `5432`.

⚠️ **Note:** `init.sql` will only run the first time the database container is created.

---

### 3️⃣ If you need to reset database and re-run seed data

```bash
docker compose down -v  # Stops containers and removes volumes
docker compose up --build
```

This ensures `init.sql` runs again.

---

### 4️⃣ Enter backend container

To build and run the C++ backend:

```bash
docker compose exec backend bash
mkdir -p build && cd build
cmake ..
make
./Main                  # Runs the backend API
```

---

### 5️⃣ Enter database container

To connect to PostgreSQL and check data:

```bash
docker compose exec db psql -U dealerdrive -d dealerdrive
```

Run queries:

```sql
-- Check table data
SELECT COUNT(*) FROM Vehicles;
SELECT COUNT(*) FROM Customers;
SELECT COUNT(*) FROM Sales;
SELECT COUNT(*) FROM Test_Drive_Record;
```

---

## Endpoints

Example endpoint for testing Crow API:

```
GET http://localhost:3000/sales
```

- Returns a list of all sales (we will implement this next)
- More endpoints will be added for Vehicles, Customers, Test Drives

---

## Notes

- Backend and database are temporary containers. When removed, all unsaved data is lost unless using persistent volumes.
- Docker volumes for DB are automatically created unless `docker compose down -v` is used.
- Make sure ports `3000` (backend) and `5432` (PostgreSQL) are free on your machine.

---

## Database Schema

The database includes the following tables:

### Vehicles
- UUID primary key
- VIN (unique)
- Make, Model, Year
- Odometer reading
- Fuel type (Gasoline, Diesel, Electric, Hybrid)
- Transmission (Manual, Automatic, CVT)
- Trim level
- Market price
- Status (Available, Sold)

### Customers
- UUID primary key
- First name, Last name
- Address (Ontario regions: GTA, Hamilton, Niagara, Waterloo)
- Phone number (Canadian area codes)
- Email (unique)
- Driving licence (unique)

### Sales
- Links Vehicles to Customers
- Sale date and price

### Test_Drive_Record
- Links Vehicles to Customers
- Test drive date and optional comments

### Images
- Links multiple images to Vehicles
- Image URL storage

---

## Sample Data

The seed data includes:
- **600 Vehicles** - Mix of Toyota, Honda, Ford, Chevrolet, and other makes
- **600 Customers** - Located in GTA, Hamilton, Niagara, and Waterloo regions
- **150+ Sales records**
- **120+ Test drive records**

---

## Technologies Used

- **Backend**: C++ with Crow framework
- **Database**: PostgreSQL 16
- **Containerization**: Docker & Docker Compose
- **Build System**: CMake
- **SQL**: Custom schema with ENUM types and indexes

---

## License

This project is for educational purposes.

---

## Contributing

Feel free to submit issues or pull requests for improvements!