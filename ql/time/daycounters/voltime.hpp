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

/*! \file voltime.hpp
    \brief vol time day counter
*/

#ifndef quantlib_voltime_day_counter_hpp
#define quantlib_voltime_day_counter_hpp

#include <ql/time/calendar.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/time/daycounters/business252.hpp>
#include <utility>

namespace QuantLib {

    //! vol time day count convention
    /*! \ingroup daycounters */
    class VolTime : public DayCounter {
    private:
        class Impl final : public DayCounter::Impl {
        private:
            std::string calendar_name_;
            Business252 bus_calendar_;
            Real businessDayWeight_;
            Real holidayDayWeight_;
            Time yearFractionInternal(const Date& d1, const Date& d2) const;
        public:
            std::string name() const override;
            Date::serial_type dayCount(const Date& d1, const Date& d2) const override;
            Time yearFraction(const Date& d1, const Date& d2, const Date&, const Date&) const override;
            explicit Impl(const Calendar& c, const Real businessDayWeight, const Real holidayDayWeight) 
                : calendar_name_(c.name()), bus_calendar_(Business252(c)), businessDayWeight_(businessDayWeight), holidayDayWeight_(holidayDayWeight){}
        };
    public:
        VolTime(const Calendar& c = UnitedStates(UnitedStates::Market::NYSE), const Real businessDayWeight = 0.0035770176615247, const Real holidayDayWeight = 0.0008942544153812)
            : DayCounter(ext::shared_ptr<DayCounter::Impl>(new VolTime::Impl(c, businessDayWeight, holidayDayWeight))) {}
    };

}

#endif
