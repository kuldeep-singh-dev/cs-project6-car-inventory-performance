-- =========================================================
-- EXTENSIONS
-- =========================================================
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
CREATE EXTENSION IF NOT EXISTS "pgcrypto";

-- =========================================================
-- ENUM TYPES
-- =========================================================
CREATE TYPE fuel_type_enum AS ENUM ('Gasoline', 'Diesel', 'Electric', 'Hybrid');
CREATE TYPE transmission_enum AS ENUM ('Manual', 'Automatic', 'CVT');
CREATE TYPE status_enum AS ENUM ('Available', 'Sold');

-- =========================================================
-- TABLES
-- =========================================================
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

CREATE TABLE Customers (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    address TEXT,
    ph_number VARCHAR(20) NOT NULL,
    email VARCHAR(100) NOT NULL UNIQUE,
    driving_licence VARCHAR(50) NOT NULL UNIQUE
);

CREATE TABLE Images (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    vehicle_id UUID NOT NULL REFERENCES Vehicles(id) ON DELETE CASCADE,
    img_url TEXT NOT NULL
);

CREATE TABLE Sales (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    vehicle_id UUID NOT NULL REFERENCES Vehicles(id) UNIQUE,
    customer_id UUID NOT NULL REFERENCES Customers(id),
    date DATE NOT NULL,
    sale_price NUMERIC(10,2) NOT NULL
);

CREATE TABLE Test_Drive_Record (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    vehicle_id UUID NOT NULL REFERENCES Vehicles(id),
    customer_id UUID NOT NULL REFERENCES Customers(id),
    date DATE NOT NULL,
    comments TEXT
);

-- =========================================================
-- INDEXES
-- =========================================================
CREATE INDEX idx_vehicles_status ON vehicles(status);
CREATE INDEX idx_vehicles_make ON vehicles(make);
CREATE INDEX idx_vehicles_year ON vehicles(year);
CREATE INDEX idx_vehicles_make_model ON vehicles(make, model);
CREATE INDEX idx_sales_customer_id ON sales(customer_id);
CREATE INDEX idx_sales_date ON sales(date);
CREATE INDEX idx_images_vehicle_id ON images(vehicle_id);
CREATE INDEX idx_test_drive_vehicle_id ON test_drive_record(vehicle_id);
CREATE INDEX idx_test_drive_customer_id ON test_drive_record(customer_id);

-- =========================================================
-- CLEAN EXISTING DATA (SAFE)
-- =========================================================
TRUNCATE TABLE
    Test_Drive_Record,
    Sales,
    Images,
    Customers,
    Vehicles
CASCADE;

-- =========================================================
-- SEED VEHICLES (5000)
-- =========================================================
INSERT INTO Vehicles (vin, make, model, year, odometer, fuel_type, transmission, trim, market_price, status)
SELECT
    LPAD((row_number() OVER ())::text, 17, '0'),
    makes[1 + floor(random() * array_length(makes, 1))::int],
    models[1 + floor(random() * array_length(models, 1))::int],
    2016 + floor(random() * 9)::int,
    5000 + floor(random() * 145000)::int,
    fuel_types[1 + floor(random() * array_length(fuel_types, 1))::int]::fuel_type_enum,
    transmissions[1 + floor(random() * array_length(transmissions, 1))::int]::transmission_enum,
    CASE
        WHEN random() > 0.3
        THEN trims[1 + floor(random() * array_length(trims, 1))::int]
        ELSE NULL
    END,
    15000 + floor(random() * 45000)::numeric(10,2),
    CASE WHEN random() > 0.7 THEN 'Sold' ELSE 'Available' END::status_enum
FROM generate_series(1,5000),
     (SELECT ARRAY['Toyota','Honda','Ford','Chevrolet','Nissan','Hyundai','Kia','Mazda','Subaru','Volkswagen'] AS makes) m,
     (SELECT ARRAY['Camry','Civic','Accord','CR-V','F-150','Escape','RAV4','Corolla','CX-5','Outback'] AS models) mo,
     (SELECT ARRAY['Gasoline','Diesel','Electric','Hybrid'] AS fuel_types) f,
     (SELECT ARRAY['Manual','Automatic','CVT'] AS transmissions) t,
     (SELECT ARRAY['Base','SE','XLE','Sport','Limited'] AS trims) tr;

-- =========================================================
-- SEED CUSTOMERS (5000)
-- =========================================================
INSERT INTO Customers (first_name, last_name, ph_number, email, driving_licence)
SELECT
    first_names[1 + floor(random() * array_length(first_names, 1))::int],
    last_names[1 + floor(random() * array_length(last_names, 1))::int],
    '416' || LPAD((random()*9999999)::int::text, 7, '0'),
    lower(first_names[1 + floor(random() * array_length(first_names, 1))::int]) || '.' ||
    lower(last_names[1 + floor(random() * array_length(last_names, 1))::int]) || gs || '@gmail.com',
    'ON-' || LPAD(gs::text, 8, '0')
FROM generate_series(1,5000) gs,
     (SELECT ARRAY['John','Jane','Alex','Emily','Chris','Sarah','Michael','Emma'] AS first_names) fn,
     (SELECT ARRAY['Smith','Brown','Singh','Patel','Khan','Wong','Chen','Lee'] AS last_names) ln;

-- =========================================================
-- SEED SALES (2000) - ONE CUSTOMER = ONE VEHICLE
-- =========================================================
WITH sold_vehicles AS (
    SELECT id, market_price, 
           ROW_NUMBER() OVER (ORDER BY random()) as rn
    FROM Vehicles 
    WHERE status = 'Sold'
),
random_customers AS (
    SELECT id, 
           ROW_NUMBER() OVER (ORDER BY random()) as rn
    FROM Customers
)
INSERT INTO Sales (vehicle_id, customer_id, date, sale_price)
SELECT
    sv.id,
    rc.id,
    CURRENT_DATE - (random()*700)::int,
    sv.market_price * (0.9 + random()*0.15)
FROM sold_vehicles sv
JOIN random_customers rc ON sv.rn = rc.rn
LIMIT 2000;

-- =========================================================
-- SEED TEST DRIVES (5000)
-- =========================================================
INSERT INTO Test_Drive_Record (vehicle_id, customer_id, date, comments)
SELECT
    v.id,
    c.id,
    CURRENT_DATE - (random()*365)::int,
    'Customer liked the drive.'
FROM Vehicles v
JOIN Customers c ON random() < 0.5
WHERE v.status = 'Available'
LIMIT 5000;

-- =========================================================
-- SEED IMAGES (30000) - 6 images per vehicle
-- =========================================================
INSERT INTO Images (vehicle_id, img_url)
    SELECT
        v.id,
        image_urls[1 + floor(random() * array_length(image_urls, 1))::int]
    FROM Vehicles v,
        generate_series(1, 6),  -- 6 images per vehicle
        (SELECT ARRAY[
            -- Tesla/Electric cars
            'https://images.unsplash.com/photo-1560958089-b8a1929cea89?w=800',
            
            -- Luxury/Sports cars
            'https://images.unsplash.com/photo-1552519507-da3b142c6e3d?w=800',
            'https://images.unsplash.com/photo-1583121274602-3e2820c69888?w=800',
            
            -- SUVs
            'https://images.unsplash.com/photo-1519641471654-76ce0107ad1b?w=800',
            'https://images.unsplash.com/photo-1533473359331-0135ef1b58bf?w=800',
            'https://images.unsplash.com/photo-1606664515524-ed2f786a0bd6?w=800',
            
            -- Sedans
            'https://images.unsplash.com/photo-1536700503339-1e4b06520771?w=800',
            'https://images.unsplash.com/photo-1549317661-bd32c8ce0db2?w=800',
            'https://images.unsplash.com/photo-1605559424843-9e4c228bf1c2?w=800',
            
            -- Modern cars
            'https://images.unsplash.com/photo-1619405399517-d7fce0f13302?w=800',
            'https://images.unsplash.com/photo-1600712242805-5f78671b24da?w=800',
            
            -- Classic/Vintage
            'https://images.unsplash.com/photo-1503376780353-7e6692767b70?w=800',
            'https://images.unsplash.com/photo-1494905998402-395d579af36f?w=800',
            
            -- Interior shots
            'https://images.unsplash.com/photo-1606152421802-db97b9c7a11b?w=800',
            'https://images.unsplash.com/photo-1581540222194-0def2dda95b8?w=800',
            
            -- Action/Driving shots
            'https://images.unsplash.com/photo-1568605117036-5fe5e7bab0b7?w=800',
            'https://images.unsplash.com/photo-1618843479313-40f8afb4b4d8?w=800',
            
            -- More variety
            'https://images.unsplash.com/photo-1609521263047-f8f205293f24?w=800',
            'https://images.unsplash.com/photo-1614200187524-dc4b892acf16?w=800',
            'https://images.unsplash.com/photo-1605559424843-9e4c228bf1c2?w=800'
        ] AS image_urls) urls;

-- =========================================================
-- SUMMARY
-- =========================================================
SELECT 'Vehicles' AS table, COUNT(*) FROM Vehicles
UNION ALL
SELECT 'Customers', COUNT(*) FROM Customers
UNION ALL
SELECT 'Sales', COUNT(*) FROM Sales
UNION ALL
SELECT 'Test Drives', COUNT(*) FROM Test_Drive_Record
UNION ALL
SELECT 'Images', COUNT(*) FROM Images;
