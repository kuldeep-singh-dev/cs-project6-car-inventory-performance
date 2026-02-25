import FeatureButton from "../components/FeatureButtons";
import "./DashboardPage.css";

const DashboardPage = () => {
  return (
    <div className="dash">
      <section className="dashHero">
        <div className="dashQuote">
          <div className="dashQuoteMark">“</div>
          <p className="dashQuoteText">
            Every vehicle in your inventory is an opportunity - manage them wisely, sell them confidently.
          </p>
          <div className="dashQuoteBy"></div>
        </div>
      </section>

      <section className="dashSection">
        <div className="dashSectionHeader">
          <h2 className="dashH2">Quick actions</h2>
          <p className="muted"></p>
        </div>
        <FeatureButton />
      </section>
    </div>
  );
};

export default DashboardPage;
