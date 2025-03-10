#include <iostream>
#include <ql/exercise.hpp>
#include <ql/instruments/payoffs.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/option.hpp>
#include <ql/pricingengines/vanilla/analyticeuropeanengine.hpp>
#include <ql/pricingengines/vanilla/fdblackscholesvanillaengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/quote.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/settings.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>
#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/Actual36525.hpp>
#include <ql/time/daycounters/Business252.hpp>
#include <ql/time/daycounters/voltime.hpp>
#include <ql/time/daycounters/customdaycounter.hpp>
#include "ManualTestOptPricing.hpp"


double TestOptPricing::calcPrice(const Date& expiry, const bool isCall, const bool withDiv)
{
    // date set up
    UnitedStates calendar = UnitedStates(UnitedStates::NYSE);
    Date modelDate(26, Jun, 2024);
    Date settlementDate(modelDate);
    Settings::instance().evaluationDate() = modelDate;

    // option parameters
    Option::Type opt_type(isCall ? Option::Call : Option::Put);
    Real stock = 100;
    Real strike = 100;
    Spread dividendYield = 0.01;
    Rate riskFreeRate = 0.05;
    Volatility volatility = 0.4;
    Date maturity(expiry);
    DayCounter rate_dc = VolTime(calendar);
    DayCounter vol_dc = VolTime(calendar);
    bool use_flat_rate = true;

    std::vector<ext::shared_ptr<Dividend> > div_curve;
    if (withDiv) {
        Date ex_div_date(maturity);
        ext::shared_ptr<Dividend> one_div(new FixedDividend(2.0, ex_div_date));
        div_curve.push_back(one_div);
    }

    const std::vector<Date> bermudan_dates = calendar.businessDayList(modelDate, maturity);
    boost::shared_ptr<Exercise> bermudanExercise(new BermudanExercise(bermudan_dates));

    Handle<Quote> underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(stock)));

    Handle<YieldTermStructure> flatRateTermStructure, flatDividendTS;
    if (use_flat_rate)
    {
        // bootstrap the yield/dividend/vol curves
        flatRateTermStructure = Handle<YieldTermStructure>(boost::shared_ptr<YieldTermStructure>(
            new FlatForward(
                settlementDate,
                riskFreeRate,
                rate_dc)));

        flatDividendTS = Handle<YieldTermStructure>(boost::shared_ptr<YieldTermStructure>(
            new FlatForward(settlementDate,
                dividendYield,
                rate_dc)));
    }
    else
    {
        std::vector<double> rates_vector(bermudan_dates.size(), riskFreeRate);
        std::vector<double> borrow_vector(bermudan_dates.size(), dividendYield);
        flatRateTermStructure = Handle<YieldTermStructure>(
            ext::shared_ptr<YieldTermStructure>(new ZeroCurve(bermudan_dates, rates_vector, rate_dc))
        );
        flatDividendTS = Handle<YieldTermStructure>(
            ext::shared_ptr<YieldTermStructure>(new ZeroCurve(bermudan_dates, borrow_vector, rate_dc))
        );
    }

    Handle<BlackVolTermStructure> flatVolTS(boost::shared_ptr<BlackVolTermStructure>(
        new BlackConstantVol(
            settlementDate,
            calendar,
            volatility,
            vol_dc)));

    boost::shared_ptr<StrikedTypePayoff> payoff(
        new PlainVanillaPayoff(
            opt_type,
            strike));

    boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
        new BlackScholesMertonProcess(
            underlyingH,
            flatDividendTS,
            flatRateTermStructure,
            flatVolTS));

    VanillaOption americanOption(
        payoff,
        bermudanExercise
    );

    //boost::shared_ptr<PricingEngine> price_engine = boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(bsmProcess));
    int time_steps = 500;
    int grid_points = 1000;
    boost::shared_ptr<PricingEngine> price_engine = boost::shared_ptr<PricingEngine>(new FdBlackScholesVanillaEngine(
        bsmProcess, div_curve, time_steps, grid_points
    ));

    // computing the option price with the analytic Black-Scholes formulae
    americanOption.setPricingEngine(price_engine);
    double opt_px = americanOption.NPV();
    return opt_px;
}

void TestOptPricing::runManualTest()
{
    Date d1(28, June, 2024);
    Date d2(29, June, 2024);
    const DayCounter vol_dc = VolTime();
    const double testVttx = vol_dc.yearFraction(d1, d2);
    std::cout << std::setprecision(15) << testVttx << std::endl;

    Date expArray[] = { Date(28, Jun, 2024), Date(2, Jul, 2024) };
    bool isCallArray[] = { true, false };
    bool withDivArray[] = { true, false };
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k) {
                double thisPx = calcPrice(expArray[i], isCallArray[j], withDivArray[k]);
                std::cout << std::setprecision(15) << expArray[i] << "," << isCallArray[j] << "," << withDivArray[k] << "," << thisPx << std::endl;
            }
}
