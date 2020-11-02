/*
 * animation/stepper/linear.h
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
 * Definition for a linear stepping function - just adds to progress
 * according to how long the animation is.
 */

#include <memory>

#include <animation/stepper/stepper.h>
#include <animation/property.h>

#pragma once

namespace animation
{
    namespace stepper
    {
        class LinearStepper:
            public Stepper
        {
            public:

                LinearStepper();
                LinearStepper(unsigned int length);
                ~LinearStepper();

                static constexpr unsigned int DefaultAnimationLength = 300;

                ANIMATION_DECLARE_PROPERTY (LinearStepper, Length, float);

            protected:

                float Step (unsigned int ms) override;

            private:

                struct Private;
                std::unique_ptr <Private> priv;
        };

        inline std::shared_ptr <LinearStepper> Linear (unsigned int length)
        {
            return std::make_shared <LinearStepper> (length);
        }
    }
}
