/*
 * animation/stepper/reerse.cpp
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
 * Definition for a reverse stepping function. Takes another stepping
 * function and inverts its output.
 */

#include <animation/stepper/reverse.h>

namespace as = animation::stepper;

namespace animation
{
    namespace stepper
    {
        struct ReverseStepper::Private
        {
            Private(as::Stepper::Shared const &stepper) :
                stepper (stepper)
            {
            }

            as::Stepper::Shared stepper;
        };
    }
}

ANIMATION_DEFINE_READONLY_PROPERTY (as::ReverseStepper, Stepper, as::Stepper::Shared, priv->stepper)

as::ReverseStepper::ReverseStepper (Stepper::Shared const &stepper):
    priv (std::make_unique <as::ReverseStepper::Private> (stepper))
{
}

float
as::ReverseStepper::Step (unsigned int ms)
{
    return 1.0 - (*priv->stepper) (ms);
}

as::ReverseStepper::~ReverseStepper () = default;
