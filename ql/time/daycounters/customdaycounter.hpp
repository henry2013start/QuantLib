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

/*! \file customdaycounter.hpp
    \brief custom day counter
*/

#ifndef quantlib_custom_day_counter_hpp
#define quantlib_custom_day_counter_hpp

#include <ql/time/calendar.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/daycounter.hpp>
#include <utility>

namespace QuantLib {

    //! custom day count convention
    /*! \ingroup daycounters */
    class CustomDayCounter : public DayCounter {
    private:
        class Impl final : public DayCounter::Impl {
        private:
            std::vector<Date> dates_;
            std::vector<Time> timeInYear_;
            Time yearFraction_(const Date& d1) const;
        public:
            std::string name() const override;
            Date::serial_type dayCount(const Date& d1, const Date& d2) const override;
            Time
                yearFraction(const Date& d1, const Date& d2, const Date&, const Date&) const override;
            explicit Impl(const std::vector<Date>& dates, const std::vector<Time>& timeInYear);
        };
    public:
        CustomDayCounter(const std::vector<Date>& dates, const std::vector<Time>& timeInYear)
            : DayCounter(ext::shared_ptr<DayCounter::Impl>(new CustomDayCounter::Impl(dates, timeInYear))) {}
    };

}

#endif
