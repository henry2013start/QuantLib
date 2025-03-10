#pragma once

#include <ql/time/date.hpp>

using namespace QuantLib;

class TestOptPricing
{
public:
    static double calcPrice(const Date& expiry, const bool isCall, const bool withDiv);
    static void runManualTest();
};
