/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Piter Dias
 Copyright (C) 2011 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/time/daycounters/customdaycounter.hpp>
#include <map>

namespace QuantLib {
    CustomDayCounter::Impl::Impl(const std::vector<Date>& dates, const std::vector<Time>& timeInYear)
        : dates_(dates), timeInYear_(timeInYear)
    {
        if (dates.size() != timeInYear.size())
            throw std::invalid_argument("dates and timeInYear do not have the same size. ");

        if (dates.size() <= 1)
            throw std::invalid_argument("size of dates is <= 1. ");

        if (timeInYear[0] != 0.0)
            throw std::invalid_argument("First element of timeInYear is not 0. ");
    }

    std::string CustomDayCounter::Impl::name() const {
        return "CustomDayCounter";
    }

    Date::serial_type CustomDayCounter::Impl::dayCount(const Date& d1,
        const Date& d2) const {
        throw std::logic_error("dayCount not supported in CustomDayCounter. ");
    }

    Time CustomDayCounter::Impl::yearFraction_(const Date& d1) const {
        auto iter = std::lower_bound(this->dates_.begin(), this->dates_.end(), d1);
        if (iter == this->dates_.begin())
        {
            if (d1 == this->dates_[0])
                return this->timeInYear_[0];
            else
                throw std::logic_error("Input date is before the first pillar. ");
        }

        __int64 idx = iter - this->dates_.begin();
        if (iter == this->dates_.end())
            // if it is after the last pillar, do extrapolation 
            idx -= 1;

        const Real days1 = d1 - this->dates_[idx - 1];
        const Real days2 = this->dates_[idx] - this->dates_[idx - 1];

        const Time res = this->timeInYear_[idx - 1] + (this->timeInYear_[idx] - this->timeInYear_[idx - 1]) / days2 * days1;
        return res;
    }

    Time CustomDayCounter::Impl::yearFraction(const Date& d1,
        const Date& d2,
        const Date&,
        const Date&) const {

        const Time t1 = this->yearFraction_(d1);
        const Time t2 = this->yearFraction_(d2);
        const Time t = t2 - t1;
        return t;
    }

}
