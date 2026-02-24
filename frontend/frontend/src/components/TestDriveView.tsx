import type { TestDriveRev } from "../types/testDrive";

interface Props {
  testDrive: TestDriveRev;
}

const boxStyle: React.CSSProperties = {
  border: "1px solid #ddd",
  borderRadius: "6px",
  padding: "12px 16px",
  minWidth: "350px",
};
const TestDriveView = ({ testDrive }: Props) => {
  //const customer = testDrive.fi;
  //const vehicle = testDrive.vehicle_id;

  return (
    <div
      style={{
        display: "flex",
        gap: "80px",
        padding: "40px",
        alignItems: "flex-start",
      }}
    >
      {/* License Image */}
      <div
        style={{
          width: "320px",
          height: "220px",
          border: "1px solid #ddd",
          borderRadius: "8px",
          display: "flex",
          alignItems: "center",
          justifyContent: "center",
        }}
      >
        <h3 style={{ color: "#888" }}>License Image</h3>
      </div>

      {/* Info Boxes */}
      <div
        style={{
          display: "flex",
          flexDirection: "column",
          gap: "14px",
        }}
      >
        <div style={boxStyle}>
          <strong>Customer Name: </strong>
          {testDrive.firstName && testDrive.lastName
            ? `${testDrive.firstName} ${testDrive.lastName}`
          : "-"}
        </div>
        <div style={boxStyle}>
          <strong>Vehicle: </strong>
            {`${testDrive.make} ${testDrive.model}`}
        </div>

        <div style={boxStyle}>
          <strong>Test Drive Date: </strong>
            {new Date(testDrive.date).toLocaleString()}
        </div>

        <div style={boxStyle}>
          <strong>Notes: </strong>{testDrive.comment ?? "-"}
        </div>
      </div>
    </div>
  );
};

export default TestDriveView;

