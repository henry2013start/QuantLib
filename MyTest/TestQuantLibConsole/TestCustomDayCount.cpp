#include <iostream>
#include <ql/time/daycounters/customdaycounter.hpp>
#include "TestUtil.hpp"
#include "TestCustomDayCount.hpp"

using namespace QuantLib;

void TestCustomDayCount::testCustomDayCountTwoNodes()
{
    int year = 2024;
    Date d1 = Date(28, Month::Feb, year);
    Date d2 = Date(2, Month::Mar, year);

    std::vector<Date> dates(2);
    std::vector<Time> timeInYear(2);

    dates[0] = d1;
    dates[1] = d2;

    timeInYear[0] = 0.0;
    timeInYear[1] = 1.0;

    CustomDayCounter cust_dc(dates, timeInYear);

    Date d3 = Date(29, Month::Feb, year);
    Date d4 = Date(2, Month::Mar, year);
    Time t = cust_dc.yearFraction(d3, d4);

    assertEqual(t, 2.0 / 3.0, 1e-10);
}

void TestCustomDayCount::testCustomDayCountMultipleNodesInterp()
{
    const double tol = 1e-10;

    std::vector<Date> dates(3);
    std::vector<Time> timeInYear(3);

    dates[0] = Date(4, Month::Mar, 2024);;
    dates[1] = Date(6, Month::Mar, 2024);;
    dates[2] = Date(9, Month::Mar, 2024);;

    timeInYear[0] = 0.0;
    timeInYear[1] = 1.0;
    timeInYear[2] = 2.0;

    CustomDayCounter cust_dc(dates, timeInYear);

    int num_days = dates[2] - dates[0];
    double expected_time_list[6] = { 0.0, 0.5, 1.0, 1.0 + 1.0 / 3.0, 1.0 + 2.0 / 3.0, 2.0 };
    for (int i = 0; i <= num_days; ++i) {
        const Date this_date = dates[0] + i;
        const Time this_t = cust_dc.yearFraction(dates[0], this_date);
        // std::cout << this_date << this_t << std::endl;
        assertEqual(this_t, expected_time_list[i], tol);
    }

    Date d1(5, Month::Mar, 2024);
    Date d2(7, Month::Mar, 2024);
    const Time this_t = cust_dc.yearFraction(d1, d2);
    assertEqual(this_t, 0.5 + 1.0 / 3.0, tol);
}

void TestCustomDayCount::testCustomDayCountMultipleExterp()
{
    const double tol = 1e-10;

    std::vector<Date> dates(3);
    std::vector<Time> timeInYear(3);

    dates[0] = Date(4, Month::Mar, 2024);;
    dates[1] = Date(6, Month::Mar, 2024);;
    dates[2] = Date(9, Month::Mar, 2024);;

    timeInYear[0] = 0.0;
    timeInYear[1] = 1.0;
    timeInYear[2] = 2.0;

    CustomDayCounter cust_dc(dates, timeInYear);

    std::vector<Date> date_list(2);
    date_list[0] = Date(3, Month::Mar, 2024);
    date_list[1] = Date(10, Month::Mar, 2024);
    bool has_error = false;
    try {
        cust_dc.yearFraction(dates[0], date_list[0]);
    }
    catch (const std::logic_error& exec) {
        //std::cout << exec.what() << std::endl;
        has_error = true;
    }
    assertTrue(has_error);

    const Time this_time = cust_dc.yearFraction(dates[0], date_list[1]);
    const double expected_time = 2.0 + 1.0 / 3.0;
    assertEqual(this_time, expected_time, tol);
}

void TestCustomDayCount::runTests()
{
    testCustomDayCountTwoNodes();
    testCustomDayCountMultipleNodesInterp();
    testCustomDayCountMultipleExterp();

    std::cout << "tests passed" << std::endl;
}
