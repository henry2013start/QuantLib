#include <iostream>
#include <ql/time/daycounters/voltime.hpp>
#include "TestUtil.hpp"
#include "TestVolTime.hpp"

using namespace QuantLib;

void TestVolTime::testYearFraction()
{
    const double holidayTime = 0.0008942544153812;
    const double busDayTime = 0.0035770176615247;
    const double tol = 1e-12;

    VolTime volTime;

    const int size = 7;
    Date dateList[size];
    dateList[0] = Date(18, June, 2024);
    dateList[1] = Date(19, June, 2024);
    dateList[2] = Date(20, June, 2024);
    dateList[3] = Date(21, June, 2024);
    dateList[4] = Date(22, June, 2024);
    dateList[5] = Date(23, June, 2024);
    dateList[6] = Date(24, June, 2024);

    double expTime[size - 1];
    expTime[0] = holidayTime;
    expTime[1] = busDayTime;
    expTime[2] = busDayTime;
    expTime[3] = holidayTime;
    expTime[4] = holidayTime;
    expTime[5] = busDayTime;

    double sumT = 0.0;

    for (int i = 0; i < size - 1; i++) {
        const Time thisT1 = volTime.yearFraction(dateList[i], dateList[i + 1]);
        const Time thisT2 = volTime.yearFraction(dateList[i + 1], dateList[i]);
        // std::cout << dateList[i] << " " << dateList[i + 1] << " " << thisT << std::endl;
        assertEqual(thisT1, expTime[i], tol);
        assertEqual(thisT2, -expTime[i], tol);

        sumT += thisT1;
    }

    const Time allT = volTime.yearFraction(dateList[0], dateList[size - 1]);
    assertEqual(allT, sumT, tol);
}

void TestVolTime::testYearFractionOnBound()
{
    const double holidayTime = 0.0008942544153812;
    const double busDayTime = 0.0035770176615247;
    const double tol = 1e-12;

    VolTime volTime;

    const int size = 3;
    Date d1List[size];
    Date d2List[size];
    double expT[size];

    d1List[0] = Date(29, Dec, 2199);
    d1List[1] = Date(30, Dec, 2199);
    d1List[2] = Date(31, Dec, 2199);

    d2List[0] = Date(30, Dec, 2199);
    d2List[1] = Date(31, Dec, 2199);
    d2List[2] = Date(31, Dec, 2199);

    expT[0] = busDayTime;
    expT[1] = busDayTime;
    expT[2] = 0.0;

    for (int i = 0; i < size; i++) {
        const Time t1 = volTime.yearFraction(d1List[i], d2List[i]);
        assertEqual(t1, expT[i], tol);

        const Time t2 = volTime.yearFraction(d2List[i], d1List[i]);
        assertEqual(t2, -expT[i], tol);
    }
}

void TestVolTime::runTests()
{
    testYearFraction();
    testYearFractionOnBound();

    std::cout << "tests passed" << std::endl;
}
