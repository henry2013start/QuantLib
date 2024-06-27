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

#include <ql/time/daycounters/voltime.hpp>
#include <map>

namespace QuantLib {

    std::string VolTime::Impl::name() const {
        std::ostringstream out;
        out << "VolTime(" << calendar_name_ << ")";
        return out.str();
    }

    Date::serial_type VolTime::Impl::dayCount(const Date& d1,
        const Date& d2) const {
        throw std::logic_error("dayCount not supported in VolTime day counter. ");
    }

    Time VolTime::Impl::yearFractionInternal(const Date& d1, const Date& d2) const {
        // return year fraction from EOD d1 to EOD d2 where d1<=d2. 
        // bus_calendar_.dayCount calc count from SOD d1 to SOD d2. 
        // this is why we need to calc day count from d1+1 to d2+1
        // 2199/12/31 is last day and it is a business day

        Date d1_next, d2_next;
        Date::serial_type busDayAdj = 0;
        if (d1.year() < 2199 || d1.month() < 12 || d1.dayOfMonth() < 31) {
            d1_next = d1 + 1;
        }
        else {
            d1_next = d1;
            busDayAdj -= 1;
        }

        if (d2.year() < 2199 || d2.month() < 12 || d2.dayOfMonth() < 31) {
            d2_next = d2 + 1;
        } else {
            d2_next = d2;
            busDayAdj += 1;
        }

        Date::serial_type numBusinessDays = this->bus_calendar_.dayCount(d1_next, d2_next);
        numBusinessDays += busDayAdj;

        Date::serial_type numHolidays = d2 - d1 - numBusinessDays;

        Real yearFraction = numBusinessDays * this->businessDayWeight_ + numHolidays * this->holidayDayWeight_;
        return yearFraction;
    }

    Time VolTime::Impl::yearFraction(const Date& d1, const Date& d2, const Date&, const Date&) const {
        if (d1 <= d2)
            return yearFractionInternal(d1, d2);
        else
            return -yearFractionInternal(d2, d1);
    }

}
