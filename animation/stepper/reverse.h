/*
 * animation/stepper/reverse.h
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

#include <memory>

#include <animation/stepper/stepper.h>
#include <animation/property.h>

#pragma once

namespace animation
{
    namespace stepper
    {
        class ReverseStepper :
            public Stepper
        {
            public:

                ReverseStepper (Stepper::Shared const &stepper);
                ~ReverseStepper();
                
                ANIMATION_DECLARE_READONLY_PROPERTY (ReverseStepper, Stepper, Stepper::Shared);

            protected:

                float Step (unsigned int ms) override;

            private:

                struct Private;
                std::unique_ptr <Private> priv;
        };

        inline std::shared_ptr <ReverseStepper> Reverse (Stepper::Shared const &stepper)
        {
            return std::make_shared <ReverseStepper> (stepper);
        }
    }
}
