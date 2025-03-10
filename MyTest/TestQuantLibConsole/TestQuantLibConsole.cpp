// TestQuantLibConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/shared_ptr.hpp>
//#include <ql/any.hpp>
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
#include <time.h>
#include "ManualTestOptPricing.hpp"
#include "TestQuantLibConsole.h"
#include "TestCustomDayCount.hpp"
#include "TestImpliedVol.hpp"
#include "TestVolTime.hpp"

using namespace QuantLib;

void priceOptionOnPde_temp()
{
    double spot = 100;
    Date calc_date= Date(28, Month::Feb, 2024);
    Date expiry = Date(28, Month::Apr, 2024);
    double strike = 100;
    Option::Type opt_type = Option::Call;

    Actual36525 rate_dc = Actual36525();
    UnitedStates calendar = UnitedStates(UnitedStates::NYSE);
    Business252 vol_dc = Business252(calendar);

    Real rate = 0.0;
    Real div_yield = 0.0;
    Real vol = 0.2;

    Settings::instance().evaluationDate() = calc_date;

    /*
    boost::shared_ptr<> payoff(new PlainVanillaPayoff(type, strike));
    boost::shared_ptr<> europeanExercise(new EuropeanExercise(maturity));
    VanillaOption european_opt(payoff, europeanExercise);
    */
}

void priceOption()
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
    Spread dividendYield = 0.00;
    Rate riskFreeRate = 0.00;
    Volatility volatility = 0.20;
    Date maturity(28, May, 2024);
    DayCounter rate_dc = Actual36525();
    //DayCounter vol_dc = Actual365Fixed();
    DayCounter vol_dc = Business252(calendar);
    //DayCounter vol_dc = VolTime(calendar);

    boost::shared_ptr<Exercise>
        europeanExercise(new EuropeanExercise(maturity));

    Handle<Quote>
        underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(stock)));

    // bootstrap the yield/dividend/vol curves
    Handle<YieldTermStructure> flatRateTermStructure(boost::shared_ptr<YieldTermStructure>(
        new FlatForward(
            settlementDate,
            riskFreeRate,
            rate_dc)));

    Handle<YieldTermStructure> flatDividendTS(boost::shared_ptr<YieldTermStructure>(
        new FlatForward(settlementDate,
            dividendYield,
            rate_dc)));

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

    // our option is European-style
    VanillaOption europeanOption(
        payoff,
        europeanExercise);

    //boost::shared_ptr<PricingEngine> price_engine = boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(bsmProcess));
    int time_steps = 200;
    int grid_points = 200;
    boost::shared_ptr<PricingEngine> price_engine = boost::shared_ptr<PricingEngine>(new FdBlackScholesVanillaEngine(bsmProcess, time_steps, grid_points));

    // computing the option price with the analytic Black-Scholes formulae
    europeanOption.setPricingEngine(price_engine);

    // outputting
    std::cout << "Option type = " << opt_type << std::endl;
    std::cout << "Maturity = " << maturity << std::endl;
    std::cout << "Stock price = " << stock << std::endl;
    std::cout << "Strike = " << strike << std::endl;
    std::cout << "Risk-free interest rate = " << riskFreeRate << std::endl;
    std::cout << "Dividend yield = " << dividendYield << std::endl;
    std::cout << "Volatility = " << volatility << std::endl << std::endl;
    std::cout << "European Option value = " << std::setprecision(15) << europeanOption.NPV() << std::endl;
}

int temp()
{
    VolTime my_dc = VolTime();
    std::cout << my_dc.name() << std::endl;

    int year = 2019;
    Date d1 = Date(28, Month::Feb, year);
    //Date d2 = Date(6, Month::Mar, year);
    Date d2 = Date(28, Month::Feb, year + 1);

    Real yearFraction = my_dc.yearFraction(d1, d2);
    std::cout << yearFraction << std::endl;

    return 0;
}

double test_perf_given_day_counter(const DayCounter& dayCounter)
{
    const Date d1 = Date(28, Month::Feb, 2024);
    int n = 10000;

    clock_t start_t, end_t;
    start_t = clock();

    double total_time = 0;
    for (int i = 1; i < n; i++) {
        const Date d2 = d1 + i;
        double this_time = dayCounter.yearFraction(d1, d2);
        total_time += this_time;
    }

    end_t = clock();
    double time_taken = double(end_t - start_t) / double(CLOCKS_PER_SEC);
    return time_taken;
}

void test_perf()
{
    double timeUsed = 0.0;

    const UnitedStates trading_cal = UnitedStates(UnitedStates::NYSE);
    Business252 bus_dc = Business252(trading_cal);
    timeUsed = test_perf_given_day_counter(bus_dc);
    std::cout << std::setprecision(10) << bus_dc.name() << " " << timeUsed << std::endl;

    VolTime vol_time_dc = VolTime(trading_cal);
    timeUsed = test_perf_given_day_counter(vol_time_dc);
    std::cout << std::setprecision(10) << vol_time_dc.name() << " " << timeUsed << std::endl;
}

void test_perf_busiday_between()
{
    const UnitedStates trading_cal = UnitedStates(UnitedStates::NYSE);
    const Date d1 = Date(28, Month::Feb, 2024);
    int n = 10000;

    clock_t start_t, end_t;
    start_t = clock();

    double total_time = 0;
    for (int i = 1; i < n; i++) {
        const Date d2 = d1 + i;
        int days_between = trading_cal.businessDaysBetween(d1, d2);
        total_time += days_between;
    }

    end_t = clock();
    double time_taken = double(end_t - start_t) / double(CLOCKS_PER_SEC);
    std::cout << std::setprecision(10) << time_taken << std::endl;
}

void test_busiday_between()
{
    const UnitedStates trading_cal = UnitedStates(UnitedStates::NYSE);
    const Business252 bus_dc(trading_cal);
    const Date d1 = Date(2, Month::Jan, 2020);
    int n = 365*20;

    clock_t start_t, end_t;
    start_t = clock();
    double all_sum = 0;

    for (int i = 1; i < n; i++) {
        const Date d2 = d1 + i;
        //int days1 = trading_cal.businessDaysBetween(d1, d2);
        int days2 = bus_dc.dayCount(d1, d2);
        all_sum += days2;
        /*
        if (days1 != days2) {
            std::cout << d1 << " " << d2 << std::endl;
            break;
        }
        */
    }
    std::cout << "all good. " << std::endl;

    end_t = clock();
    double time_taken = double(end_t - start_t) / double(CLOCKS_PER_SEC);
    std::cout << std::setprecision(10) << time_taken << std::endl;
}

double testBermudanOpt()
{
    // date set up
    UnitedStates calendar = UnitedStates(UnitedStates::NYSE);
    Date modelDate(1, Mar, 2024);
    Date settlementDate(modelDate);
    Settings::instance().evaluationDate() = modelDate;

    // option parameters
    Option::Type opt_type(Option::Call);
    Real stock = 100;
    Real strike = 100;
    Spread dividendYield = 0.00;
    Rate riskFreeRate = 0.05;
    Volatility volatility = 0.3;
    Date maturity(20, December, 2024);
    DayCounter rate_dc = Actual36525();
    DayCounter vol_dc = Actual36525();
    bool use_flat_rate = true;

    Date ex_div_date(4, Sep, 2024);
    std::vector<ext::shared_ptr<Dividend> > div_curve(1);
    ext::shared_ptr<Dividend> one_div(new FixedDividend(25.5, ex_div_date));
    div_curve[0] = one_div;

    boost::shared_ptr<Exercise>
        americanExercise(new AmericanExercise(maturity));

    const std::vector<Date> bermudan_dates = calendar.businessDayList(modelDate, maturity);
    boost::shared_ptr<Exercise> bermudanExercise(new BermudanExercise(bermudan_dates));

    Handle<Quote>
        underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(stock)));

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
        //americanExercise
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

void testBermudanOptPerf()
{
    clock_t start_t, end_t;
    start_t = clock();

    double opt_px = 0;
    int max_iter = 1;
    for (int i=0; i< max_iter; ++i)
        opt_px = testBermudanOpt();

    end_t = clock();
    double time_taken = double(end_t - start_t) / double(CLOCKS_PER_SEC);

    std::cout << "Option value = " << std::setprecision(15) << opt_px << std::endl;
    std::cout << "exec time: " << time_taken << std::endl;
    std::cout << "avg time: " << time_taken / max_iter << std::endl;
}

void testDivDiscount()
{
    // date set up
    UnitedStates calendar = UnitedStates(UnitedStates::NYSE);
    Date modelDate(1, Mar, 2024);
    Date settlementDate(modelDate);
    Settings::instance().evaluationDate() = modelDate;

    // option parameters
    Option::Type opt_type(Option::Call);
    Real stock = 150;
    Real strike = 100;
    Spread dividendYield = 0.00;
    Rate riskFreeRate = 0.05;
    Volatility volatility = 0.01;
    Date maturity(30, Sep, 2024);
    DayCounter rate_dc = Actual36525();
    DayCounter vol_dc = Actual36525();

    Date ex_div_date(4, Sep, 2024);
    std::vector<ext::shared_ptr<Dividend> > div_curve(1);
    ext::shared_ptr<Dividend> one_div(new FixedDividend(25.5, ex_div_date));
    div_curve[0] = one_div;

    boost::shared_ptr<Exercise>
        americanExercise(new AmericanExercise(maturity));

    std::vector<Date> bermudan_dates(3);
    bermudan_dates[0] = ex_div_date - 1;
    bermudan_dates[1] = ex_div_date;
    bermudan_dates[2] = maturity;
    boost::shared_ptr<Exercise> bermudanExercise(new BermudanExercise(bermudan_dates));

    Handle<Quote>
        underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(stock)));

    // bootstrap the yield/dividend/vol curves
    Handle<YieldTermStructure> flatRateTermStructure(boost::shared_ptr<YieldTermStructure>(
        new FlatForward(
            settlementDate,
            riskFreeRate,
            rate_dc)));

    Handle<YieldTermStructure> flatDividendTS(boost::shared_ptr<YieldTermStructure>(
        new FlatForward(settlementDate,
            dividendYield,
            rate_dc)));

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
        //americanExercise,
    );

    //boost::shared_ptr<PricingEngine> price_engine = boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(bsmProcess));
    int time_steps = 1000;
    int grid_points = 2000;
    boost::shared_ptr<PricingEngine> price_engine = boost::shared_ptr<PricingEngine>(new FdBlackScholesVanillaEngine(
        bsmProcess, div_curve, time_steps, grid_points
    ));

    // computing the option price with the analytic Black-Scholes formulae
    americanOption.setPricingEngine(price_engine);
    double opt_px = americanOption.NPV();
    std::cout << "Option value = " << std::setprecision(15) << opt_px << std::endl;
}

double testDivVolTime()
{
    // verify the vol time calculation for each day and div
    // this function is used for debugging

    // date set up
    UnitedStates calendar = UnitedStates(UnitedStates::NYSE);
    Date modelDate(27, Jun, 2024);
    Date settlementDate(modelDate);
    Settings::instance().evaluationDate() = modelDate;

    // option parameters
    Option::Type opt_type(Option::Call);
    Real stock = 100;
    Real strike = 100;
    Spread dividendYield = 0.00;
    Rate riskFreeRate = 0.05;
    Volatility volatility = 0.3;
    Date maturity(1, Jul, 2024);
    DayCounter rate_dc = VolTime(calendar);
    DayCounter vol_dc = VolTime(calendar);
    bool use_flat_rate = true;

    Date ex_div_date(maturity);
    std::vector<ext::shared_ptr<Dividend> > div_curve(1);
    ext::shared_ptr<Dividend> one_div(new FixedDividend(2.5, ex_div_date));
    div_curve[0] = one_div;

    const std::vector<Date> bermudan_dates = calendar.businessDayList(modelDate, maturity);
    boost::shared_ptr<Exercise> bermudanExercise(new BermudanExercise(bermudan_dates));

    Handle<Quote>
        underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(stock)));

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

ext::shared_ptr<YieldTermStructure>
flatRate(const ext::shared_ptr<Quote>& forward,
    const DayCounter& dc) {
    return ext::shared_ptr<YieldTermStructure>(
        new FlatForward(0, NullCalendar(), Handle<Quote>(forward), dc));
}

ext::shared_ptr<YieldTermStructure>
flatRate(Rate forward, const DayCounter& dc) {
    return flatRate(ext::shared_ptr<Quote>(new SimpleQuote(forward)),
        dc);
}

ext::shared_ptr<BlackVolTermStructure>
flatVol(const ext::shared_ptr<Quote>& vol,
    const DayCounter& dc) {
    return ext::shared_ptr<BlackVolTermStructure>(new
        BlackConstantVol(0, NullCalendar(), Handle<Quote>(vol), dc));
}

ext::shared_ptr<BlackVolTermStructure>
flatVol(Volatility vol,
    const DayCounter& dc) {
    return flatVol(ext::shared_ptr<Quote>(new SimpleQuote(vol)), dc);
}

void runRegTest()
{
    const auto dc = Actual360();
    const auto today = Date(27, February, 2021);
    Settings::instance().evaluationDate() = today;

    ext::shared_ptr<SimpleQuote> vol(new SimpleQuote(0.3));

    const auto process = ext::make_shared<BlackScholesMertonProcess>(
        Handle<Quote>(ext::make_shared<SimpleQuote>(100)),
        Handle<YieldTermStructure>(flatRate(0.08, dc)),
        Handle<YieldTermStructure>(flatRate(0.04, dc)),
        Handle<BlackVolTermStructure>(flatVol(vol, dc))
    );

    const auto maturityDate = today + Period(12, Months);
    std::vector<Date> dividendDates = { today + Period(10, Months) };
    std::vector<Real> dividendAmounts = { 10.0 };
    auto dividends = DividendVector(dividendDates, dividendAmounts);

    const Real strike = 100.0;
    VanillaOption option(
        ext::make_shared<PlainVanillaPayoff>(Option::Call, strike),
        ext::make_shared<AmericanExercise>(today, maturityDate)
    );

    option.setPricingEngine(
        ext::make_shared<FdBlackScholesVanillaEngine>(process, dividends, 200, 400));

    const Real spotNpv = option.NPV();
    const Real spotDelta = option.delta();

    vol->setValue(100 / 90. * 0.3);

    option.setPricingEngine(
        MakeFdBlackScholesVanillaEngine(process)
        .withTGrid(100)
        .withXGrid(400)
        .withCashDividends(dividendDates, dividendAmounts)
        .withCashDividendModel(FdBlackScholesVanillaEngine::Escrowed)
    );

    const Real escrowedNpv = option.NPV();
    const Real escrowedDelta = option.delta();

    const Real diffNpv = std::abs(escrowedNpv - spotNpv);
    const Real diffDelta = std::abs(escrowedDelta - spotDelta);

    std::cout << std::setprecision(15) << spotNpv << "," << escrowedNpv << "," << diffNpv << std::endl;
    std::cout << std::setprecision(15) << spotDelta << "," << escrowedDelta << "," << diffDelta << std::endl;

}

int main()
{
    //priceOption();
    //test_perf();
    //test_perf_busiday_between();
    test_busiday_between();
    testDivDiscount();
    runRegTest();
    //testBermudanOptPerf();

    TestCustomDayCount::runTests();
    TestImpliedVol::runTests();
    TestVolTime::runTests();

    testDivVolTime();
    TestOptPricing::runManualTest();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
