/*
 * animation-glib/stepper/stepper-internal.cp
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
 * steppers, implementation.
 */

#include <memory>

#include <animation/stepper/stepper.h>
#include <animation-glib/stepper/stepper.h>
#include <animation-glib/stepper/stepper-internal.h>


namespace as = animation::stepper;
namespace asg = animation::stepper::glib;

namespace animation {
    namespace stepper {
        namespace glib {
            struct StepperWrapper::Private
            {
                AnimationStepper *baseStepper;

                Private (AnimationStepper *baseStepper) :
                    baseStepper (ANIMATION_STEPPER (g_object_ref (baseStepper)))
                {
                }

                ~Private ()
                {
                    g_clear_object (&baseStepper);
                }
            };
        }
    }
}

ANIMATION_DEFINE_PROPERTY (asg::StepperWrapper, BaseStepper, AnimationStepper *, priv->baseStepper)

asg::StepperWrapper::StepperWrapper (AnimationStepper *baseStepper) :
    priv (std::make_unique <asg::StepperWrapper::Private> (baseStepper))
{
}

asg::StepperWrapper::~StepperWrapper ()
{
}

float
asg::StepperWrapper::Step (unsigned int ms)
{
    return animation_stepper_step (priv->baseStepper, ms);
}
