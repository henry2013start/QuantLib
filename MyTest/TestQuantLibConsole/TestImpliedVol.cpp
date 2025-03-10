#include <iostream>
#include <ql/exercise.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/pricingengines/vanilla/fdblackscholesvanillaengine.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/quote.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/settings.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/time/daycounters/Actual36525.hpp>
#include <ql/time/daycounters/voltime.hpp>
#include "TestUtil.hpp"
#include "TestImpliedVol.hpp"


using namespace QuantLib;

void TestImpliedVol::testImpliedVolCalc()
{
    // date set up
    UnitedStates calendar = UnitedStates(UnitedStates::NYSE);
    Date modelDate(28, Feb, 2024);
    Date settlementDate(modelDate);
    Settings::instance().evaluationDate() = modelDate;

    // option parameters
    Option::Type opt_type(Option::Call);
    Real stock = 100;
    Real strike = 100;
    Spread dividendYield = 0.01;
    Rate riskFreeRate = 0.05;
    Volatility volatility = 0.345;
    Date maturity(20, Dec, 2024);
    DayCounter rate_dc = Actual36525();
    DayCounter vol_dc = VolTime(calendar);

    ext::shared_ptr<Exercise>
        americanExercise(new AmericanExercise(maturity));

    Handle<Quote>
        underlyingH(ext::shared_ptr<Quote>(new SimpleQuote(stock)));

    Handle<YieldTermStructure> flatRateTermStructure(ext::shared_ptr<YieldTermStructure>(
        new FlatForward(
            settlementDate,
            riskFreeRate,
            rate_dc)));

    Handle<YieldTermStructure> flatBorrowTS(ext::shared_ptr<YieldTermStructure>(
        new FlatForward(settlementDate,
            dividendYield,
            rate_dc)));

    Handle<BlackVolTermStructure> flatVolTS(ext::shared_ptr<BlackVolTermStructure>(
        new BlackConstantVol(
            settlementDate,
            calendar,
            volatility,
            vol_dc)));

    DividendSchedule discreteDiv;
    discreteDiv.push_back(ext::make_shared<FixedDividend>(0.5, Date(30, Month::Sep, 2024)));

    ext::shared_ptr<StrikedTypePayoff> payoff(
        new PlainVanillaPayoff(
            opt_type,
            strike));

    ext::shared_ptr<BlackScholesMertonProcess> bsmProcess(
        new BlackScholesMertonProcess(
            underlyingH,
            flatBorrowTS,
            flatRateTermStructure,
            flatVolTS));

    VanillaOption americanOption(
        payoff,
        americanExercise);

    Size time_steps = 500;
    Size grid_points = 1000;
    ext::shared_ptr<PricingEngine> price_engine = ext::shared_ptr<PricingEngine>(new FdBlackScholesVanillaEngine(bsmProcess, discreteDiv, time_steps, grid_points));

    // computing the option price with the analytic Black-Scholes formulae
    americanOption.setPricingEngine(price_engine);

    Real opt_px = americanOption.NPV();

    std::cout << opt_px << std::endl;

    Real accuracy = 1e-10;
    Volatility iv_calc = americanOption.impliedVolatility(
        opt_px, bsmProcess, discreteDiv, time_steps, grid_points, FdBlackScholesVanillaEngine::CashDividendModel::Spot, accuracy
    );
    Volatility iv_diff = abs(iv_calc - volatility);

    assertEqual(volatility, iv_calc, accuracy);
}

void TestImpliedVol::runTests()
{
    testImpliedVolCalc();

    std::cout << "tests passed" << std::endl;
}
