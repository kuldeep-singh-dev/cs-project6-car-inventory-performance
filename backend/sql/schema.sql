-- Drop tables if they exist (in reverse order of dependencies)
DROP TABLE IF EXISTS Test_Drive_Record CASCADE;
DROP TABLE IF EXISTS Sales CASCADE;
DROP TABLE IF EXISTS Images CASCADE;
DROP TABLE IF EXISTS Customers CASCADE;
DROP TABLE IF EXISTS Vehicles CASCADE;

-- Drop ENUM types if they exist
DROP TYPE IF EXISTS fuel_type_enum CASCADE;
DROP TYPE IF EXISTS transmission_enum CASCADE;
DROP TYPE IF EXISTS status_enum CASCADE;

-- Enable UUID extension (for older PostgreSQL versions)
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Create ENUM types
CREATE TYPE fuel_type_enum AS ENUM ('Gasoline', 'Diesel', 'Electric', 'Hybrid');
CREATE TYPE transmission_enum AS ENUM ('Manual', 'Automatic', 'CVT');
CREATE TYPE status_enum AS ENUM ('Available', 'Sold');

-- 1. Vehicles Table
CREATE TABLE Vehicles (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    vin VARCHAR(17) NOT NULL UNIQUE,
    make VARCHAR(50) NOT NULL,
    model VARCHAR(50) NOT NULL,
    year INTEGER NOT NULL,
    odometer INTEGER NOT NULL,
    fuel_type fuel_type_enum NOT NULL,
    transmission transmission_enum NOT NULL,
    trim VARCHAR(50),
    market_price NUMERIC(10,2) NOT NULL,
    status status_enum NOT NULL DEFAULT 'Available'
);

-- 2. Customers Table
CREATE TABLE Customers (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    address TEXT,
    ph_number VARCHAR(20) NOT NULL,
    email VARCHAR(100) NOT NULL UNIQUE,
    driving_licence VARCHAR(50) NOT NULL UNIQUE
);

-- 3. Images Table
CREATE TABLE Images (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    vehicle_id UUID NOT NULL REFERENCES Vehicles(id) ON DELETE CASCADE,
    img_url TEXT NOT NULL
);

-- 4. Sales Table
CREATE TABLE Sales (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    vehicle_id UUID NOT NULL REFERENCES Vehicles(id) ON DELETE RESTRICT,
    customer_id UUID NOT NULL REFERENCES Customers(id) ON DELETE RESTRICT,
    date DATE NOT NULL,
    sale_price NUMERIC(10,2) NOT NULL
);

-- 5. Test_Drive_Record Table
CREATE TABLE Test_Drive_Record (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    vehicle_id UUID NOT NULL REFERENCES Vehicles(id) ON DELETE CASCADE,
    customer_id UUID NOT NULL REFERENCES Customers(id) ON DELETE CASCADE,
    date DATE NOT NULL,
    comments TEXT
);

-- Create indexes for better query performance
CREATE INDEX idx_images_vehicle_id ON Images(vehicle_id);
CREATE INDEX idx_sales_vehicle_id ON Sales(vehicle_id);
CREATE INDEX idx_sales_customer_id ON Sales(customer_id);
CREATE INDEX idx_sales_date ON Sales(date);
CREATE INDEX idx_test_drive_vehicle_id ON Test_Drive_Record(vehicle_id);
CREATE INDEX idx_test_drive_customer_id ON Test_Drive_Record(customer_id);
CREATE INDEX idx_test_drive_date ON Test_Drive_Record(date);
CREATE INDEX idx_vehicles_status ON Vehicles(status);
CREATE INDEX idx_vehicles_make_model ON Vehicles(make, model);
CREATE INDEX idx_vehicles_year ON Vehicles(year);
CREATE INDEX idx_customers_email ON Customers(email);
CREATE INDEX idx_customers_last_name ON Customers(last_name);