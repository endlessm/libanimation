/*
 * tests/ostream_point_operator.h
 *
 * Copyright 2018 Endless Mobile, Inc.
 *
 * libanimation is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * libanimation is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with eos-companion-app-service.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * A simple helper to output the contents of animation::Point and
 * animation::PointView when tests fail.
 */
#pragma once

#include <animation/geometry.h>    // for Point
#include <iomanip>                      // for operator<<, setprecision
#include <iosfwd>                       // for ostream
#include <ostream>                      // for basic_ostream, char_traits, etc

namespace animation
{
    namespace geometry
    {
        inline std::ostream &
        operator<< (std::ostream &lhs, Point const &p)
        {
            namespace agd = animation::geometry::dimension;

            return lhs << std::setprecision (10)
                       << "x: "
                       << agd::get <0> (p)
                       << " y: "
                       << agd::get <1> (p);
        }

        template <typename NumericType>
        inline std::ostream &
        operator<< (std::ostream                                    &lhs,
                    animation::geometry::PointView <NumericType> const &p)
        {
            namespace agd = animation::geometry::dimension;

            Point point;
            agd::assign (point, p);
            return lhs << point;
        }

        inline std::ostream &
        operator<< (std::ostream &lhs, Vector4D const &p)
        {
            namespace agd = animation::geometry::dimension;

            return lhs << std::setprecision (10)
                       << "x: "
                       << agd::get <0> (p)
                       << " y: "
                       << agd::get <1> (p)
                       << " z: "
                       << agd::get <2> (p)
                       << " w: "
                       << agd::get <3> (p);
        }
    }
}
