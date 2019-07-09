/*
 * animation-glib/stepper/stepper-internal.h
 *
 * Copyright 2019 Endless Mobile, Inc.
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
 * Internal helper class for stepper, which wraps a GObject stepper
 * in the C++ interface, so it can be consumed directly by other C++
 * steppers.
 */
#pragma once

#ifdef __cplusplus

#include <memory>

#include <animation/property.h>
#include <animation/stepper/stepper.h>
#include <animation-glib/stepper/stepper.h>


namespace animation {
    namespace stepper {
        namespace glib {
            class StepperWrapper :
                public Stepper
            {
                public:
                
                    StepperWrapper (AnimationStepper *);
                    ~StepperWrapper ();

                    ANIMATION_DECLARE_PROPERTY (StepperWrapper, BaseStepper, AnimationStepper *);

                protected:

                    float Step (unsigned int ms) override;

                private:

                    struct Private;
                    std::unique_ptr <Private> priv;
            };
        }
    }
}

#endif
