import type { TestDriveRev } from "../types/testDrive";
import "./TestDriveTable.css";
import { useNavigate } from "react-router-dom";

interface Props {
  testDrives: TestDriveRev[];
}

const TestDriveTable = ({ testDrives }: Props) => {
    const navigate = useNavigate();
    const handleRowClick = (id:string) =>{
        navigate(`/testdrive/${id}`);
    }
  return (
    <table className="td-table">
      <thead>
        <tr>
          <th>Firs Name</th>
          <th>Last Name</th>
          <th>Vehicle</th>
          <th>Date</th>
          <th>Comments</th>
        </tr>
      </thead>

      <tbody>
        {testDrives.map((td) => (
          <tr key={td.id} className="td-row"
          onClick = {() =>handleRowClick(td.id)}> 
            <td>{td.firstName}</td>
            <td>{td.lastName}</td>
            <td>{td.make} {td.model}</td>
            <td>{new Date(td.date).toLocaleDateString()}</td>
            <td>{td.comment ?? "-"}</td>
          </tr>
        ))}
      </tbody>
    </table>
  );
};

export default TestDriveTable;