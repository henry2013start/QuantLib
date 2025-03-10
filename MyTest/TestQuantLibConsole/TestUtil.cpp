#include <stdexcept>
#include <stdlib.h>
#include "TestUtil.hpp"

void assertEqual(const double v1, const double v2, const double tol)
{
    if (abs(v1 - v2) > tol)
        throw std::runtime_error("difference is bigger than tolerance. ");
}

void assertTrue(const bool value)
{
    if (!value)
        throw std::runtime_error("value is not true. ");
}
