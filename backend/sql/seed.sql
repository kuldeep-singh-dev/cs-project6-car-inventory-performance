-- Seed data for Car Dealership Database
-- This will generate 500+ vehicles, 500+ customers, and 100+ sales and test drive records

-- Enable UUID extension (for older PostgreSQL versions)
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Clear existing data (only if tables exist)
DO $$ 
BEGIN
    IF EXISTS (SELECT FROM information_schema.tables WHERE table_name = 'vehicles') THEN
        TRUNCATE TABLE Test_Drive_Record, Sales, Images, Customers, Vehicles CASCADE;
    END IF;
END $$;

-- Generate large dataset using PostgreSQL's generate_series
-- This approach is more efficient than individual INSERT statements

-- Insert 600 Vehicles with realistic data
INSERT INTO Vehicles (id, vin, make, model, year, odometer, fuel_type, transmission, trim, market_price, status)
SELECT 
    uuid_generate_v4(),
    LPAD((row_number() OVER ())::text, 17, '0'),
    makes[1 + floor(random() * array_length(makes, 1))::int],
    models[1 + floor(random() * array_length(models, 1))::int],
    2016 + floor(random() * 9)::int,
    5000 + floor(random() * 145000)::int,
    fuel_types[1 + floor(random() * array_length(fuel_types, 1))::int]::fuel_type_enum,
    transmissions[1 + floor(random() * array_length(transmissions, 1))::int]::transmission_enum,
    CASE WHEN random() > 0.3 THEN trims[1 + floor(random() * array_length(trims, 1))::int] ELSE NULL END,
    15000 + floor(random() * 45000)::numeric(10,2),
    CASE WHEN random() > 0.7 THEN 'Sold' ELSE 'Available' END::status_enum
FROM 
    generate_series(1, 600),
    (SELECT ARRAY['Toyota', 'Honda', 'Ford', 'Chevrolet', 'Nissan', 'Hyundai', 'Kia', 'Mazda', 'Subaru', 'Volkswagen'] AS makes) m,
    (SELECT ARRAY['Camry', 'Corolla', 'RAV4', 'Highlander', 'Civic', 'Accord', 'CR-V', 'Pilot', 'F-150', 'Escape', 'Explorer', 'Silverado', 'Equinox', 'Tahoe', 'Altima', 'Rogue', 'Sentra', 'Elantra', 'Tucson', 'Santa Fe', 'Forte', 'Sportage', 'Sorento', 'CX-5', 'CX-9', 'Mazda3', 'Outback', 'Forester', 'Legacy', 'Jetta', 'Tiguan', 'Atlas'] AS models) mo,
    (SELECT ARRAY['Gasoline', 'Diesel', 'Electric', 'Hybrid'] AS fuel_types) f,
    (SELECT ARRAY['Manual', 'Automatic', 'CVT'] AS transmissions) t,
    (SELECT ARRAY['Base', 'LE', 'SE', 'XLE', 'Limited', 'Sport', 'EX', 'EX-L', 'Touring', 'LX', 'XLT', 'Lariat', 'King Ranch', 'Platinum', 'LS', 'LT', 'LTZ', 'Premier', 'High Country'] AS trims) tr;

-- Insert 600 Customers with realistic data
INSERT INTO Customers (id, first_name, last_name, address, ph_number, email, driving_licence)
SELECT 
    uuid_generate_v4(),
    first_names[1 + floor(random() * array_length(first_names, 1))::int],
    last_names[1 + floor(random() * array_length(last_names, 1))::int],
    CASE WHEN random() > 0.2 THEN 
        LPAD(floor(random() * 9999 + 1)::text, 5, '0') || ' ' || 
        street_names[1 + floor(random() * array_length(street_names, 1))::int] || ' ' ||
        street_types[1 + floor(random() * array_length(street_types, 1))::int] || ', ' ||
        cities[1 + floor(random() * array_length(cities, 1))::int] || ', ON ' ||
        -- Canadian postal code format: A1A 1A1
        chr(65 + floor(random() * 26)::int) || floor(random() * 10)::text ||
        chr(65 + floor(random() * 26)::int) || ' ' ||
        floor(random() * 10)::text || chr(65 + floor(random() * 26)::int) ||
        floor(random() * 10)::text
    ELSE NULL END,
    -- Phone numbers with Canadian area codes (416/647 Toronto, 905 GTA, 289 Hamilton/Niagara, 519 Waterloo, 226 Guelph)
    area_codes[1 + floor(random() * array_length(area_codes, 1))::int] || 
    LPAD(floor(random() * 9000000 + 1000000)::text, 7, '0'),
    lower(first_names[1 + floor(random() * array_length(first_names, 1))::int]) || '.' ||
    lower(last_names[1 + floor(random() * array_length(last_names, 1))::int]) || 
    floor(random() * 999 + 1)::text || '@' ||
    domains[1 + floor(random() * array_length(domains, 1))::int],
    states[1 + floor(random() * array_length(states, 1))::int] || '-' ||
    LPAD((row_number() OVER ())::text, 8, '0')
FROM 
    generate_series(1, 600),
    (SELECT ARRAY['416', '647', '437', '905', '289', '365', '519', '226'] AS area_codes) ac,
    (SELECT ARRAY['James', 'John', 'Robert', 'Michael', 'William', 'David', 'Richard', 'Joseph', 'Thomas', 'Charles', 'Mary', 'Patricia', 'Jennifer', 'Linda', 'Barbara', 'Elizabeth', 'Susan', 'Jessica', 'Sarah', 'Karen', 'Emily', 'Ashley', 'Emma', 'Olivia', 'Sophia', 'Isabella', 'Mia', 'Charlotte', 'Amelia', 'Harper', 'Mohammed', 'Ahmed', 'Ali', 'Hassan', 'Fatima', 'Aisha', 'Priya', 'Ravi', 'Amit', 'Deepak', 'Rajesh', 'Mei', 'Wei', 'Chen', 'Ming', 'Liam', 'Noah', 'Ethan', 'Lucas', 'Ava'] AS first_names) fn,
    (SELECT ARRAY['Smith', 'Johnson', 'Williams', 'Brown', 'Jones', 'Garcia', 'Miller', 'Davis', 'Rodriguez', 'Martinez', 'Hernandez', 'Lopez', 'Gonzalez', 'Wilson', 'Anderson', 'Thomas', 'Taylor', 'Moore', 'Jackson', 'Martin', 'Lee', 'Thompson', 'White', 'Harris', 'Clark', 'Lewis', 'Robinson', 'Walker', 'Young', 'Hall', 'Khan', 'Ahmed', 'Singh', 'Patel', 'Kumar', 'Sharma', 'Wong', 'Chen', 'Li', 'Wang', 'Kim', 'Park', 'Nguyen', 'Tran', 'Rossi', 'Russo', 'DeSilva', 'Santos', 'MacDonald', 'McDonald'] AS last_names) ln,
    (SELECT ARRAY['Main', 'King', 'Queen', 'Dundas', 'Bloor', 'Yonge', 'Lakeshore', 'Hurontario', 'Eglinton', 'Steeles', 'Finch', 'Sheppard', 'York', 'Wellington', 'Victoria', 'James', 'Upper James', 'Mohawk', 'Barton', 'Niagara'] AS street_names) sn,
    (SELECT ARRAY['Street', 'Avenue', 'Boulevard', 'Drive', 'Lane', 'Road', 'Way', 'Court', 'Place', 'Circle'] AS street_types) st,
    (SELECT ARRAY['Toronto', 'Mississauga', 'Brampton', 'Markham', 'Vaughan', 'Richmond Hill', 'Oakville', 'Burlington', 'Pickering', 'Ajax', 'Whitby', 'Oshawa', 'Milton', 'Newmarket', 'Aurora', 'Hamilton', 'Stoney Creek', 'Ancaster', 'Dundas', 'Waterdown', 'Niagara Falls', 'St. Catharines', 'Welland', 'Grimsby', 'Fort Erie', 'Niagara-on-the-Lake', 'Waterloo', 'Kitchener', 'Cambridge', 'Guelph'] AS cities) c,
    (SELECT ARRAY['ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON', 'ON'] AS states) s,
    (SELECT ARRAY['gmail.com', 'yahoo.com', 'hotmail.com', 'outlook.com', 'icloud.com', 'aol.com', 'protonmail.com'] AS domains) d;

-- Insert 150 Sales records with realistic dates
WITH sold_vehicles AS (
    SELECT id, market_price FROM Vehicles WHERE status = 'Sold' LIMIT 150
),
random_customers AS (
    SELECT id FROM Customers ORDER BY random() LIMIT 150
)
INSERT INTO Sales (id, vehicle_id, customer_id, date, sale_price)
SELECT 
    uuid_generate_v4(),
    sv.id,
    rc.id,
    CURRENT_DATE - (floor(random() * 730)::int),
    sv.market_price * (0.9 + random() * 0.15)
FROM 
    sold_vehicles sv,
    random_customers rc,
    generate_series(1, 150) gs
WHERE sv.id = (SELECT id FROM sold_vehicles OFFSET (gs - 1) LIMIT 1)
  AND rc.id = (SELECT id FROM random_customers OFFSET (gs - 1) LIMIT 1);

-- Insert 120 Test Drive Records with realistic data
WITH available_vehicles AS (
    SELECT id FROM Vehicles WHERE status = 'Available' ORDER BY random() LIMIT 120
),
random_customers AS (
    SELECT id FROM Customers ORDER BY random() LIMIT 120
)
INSERT INTO Test_Drive_Record (id, vehicle_id, customer_id, date, comments)
SELECT 
    gen_random_uuid(),
    av.id,
    rc.id,
    CURRENT_DATE - (floor(random() * 365)::int),
    CASE 
        WHEN random() > 0.5 THEN comments[1 + floor(random() * array_length(comments, 1))::int]
        ELSE NULL 
    END
FROM 
    available_vehicles av,
    random_customers rc,
    generate_series(1, 120) gs,
    (SELECT ARRAY[
        'Great ride, very smooth handling.',
        'Excellent acceleration and braking.',
        'Interior quality is impressive.',
        'Customer loved the technology features.',
        'Very spacious and comfortable.',
        'Good fuel efficiency during test.',
        'Customer wants to think about it.',
        'Interested in this model, will return.',
        'Comparing with other vehicles.',
        'Loved the safety features.',
        'Perfect for their family needs.',
        'Concerned about the price.',
        'Requested information on financing.',
        'Will discuss with spouse and return.',
        'Very interested, likely to purchase.'
    ] AS comments) c
WHERE av.id = (SELECT id FROM available_vehicles OFFSET (gs - 1) LIMIT 1)
  AND rc.id = (SELECT id FROM random_customers OFFSET (gs - 1) LIMIT 1);

-- Display summary statistics
SELECT 'Vehicles' AS table_name, COUNT(*) AS row_count FROM Vehicles
UNION ALL
SELECT 'Customers', COUNT(*) FROM Customers
UNION ALL
SELECT 'Sales', COUNT(*) FROM Sales
UNION ALL
SELECT 'Test_Drive_Record', COUNT(*) FROM Test_Drive_Record;