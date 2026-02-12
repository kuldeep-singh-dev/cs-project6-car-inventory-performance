import { useEffect, useState } from "react";
import { useNavigate, useParams } from "react-router-dom";
import { testDriveService } from "../../services/testDriveService";
import type { TestDrivePatch } from "../../types/testDrive";
import "./EditTestDrivePage.css";

const EditTestDrivePage = () => {
  const { id } = useParams<{ id: string }>();
  const navigate = useNavigate();

  const [form, setForm] = useState<TestDrivePatch>({
    date: "",
    comment: "",
  });

  useEffect(() => {
    const load = async () => {
      if (!id) return;
      const data = await testDriveService.getById(id);

      setForm({
        date: data.date,
        comment: data.comment,
      });
    };

    load();
  }, [id]);

  const handleChange = (
    e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>
  ) => {
    setForm({ ...form, [e.target.name]: e.target.value });
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    await testDriveService.update(id!, form);
    navigate(`/testdrive/${id}`);
  };

  return (
    <div className="editTDPage">
      <h1 className="editTDTitle">Edit Test Drive</h1>

      <form className="editTDForm" onSubmit={handleSubmit}>
        <label className="editTDLabel">Date & Time</label>
        <input
          type="date"
          name="date"
          value={form.date}
          onChange={handleChange}
          className="editTDInput"
        />

        <label className="editTDLabel">Comment</label>
        <textarea
          name="comment"
          value={form.comment}
          onChange={handleChange}
          className="editTDTextarea"
        />

        <button type="submit" className="editTDSaveBtn">
          Save Changes
        </button>
      </form>
    </div>
  );
};

export default EditTestDrivePage;