/*
 * animation/bounce/bounce.h
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
 * An animation that causes a surface to bounce onto screen, gently
 * following an attenuating sine wave.
 */

#include <array>
#include <memory>

#include <animation/geometry.h>
#include <animation/property.h>
#include <animation/query/geometry_query.h>
#include <animation/stepper/stepper.h>
#include <animation/transform/transform.h>

#pragma once

namespace animation
{
    namespace bounce
    {
        class BounceAnimation :
            public animation::transform::TransformAnimation
        {
            public:

                BounceAnimation ();
                BounceAnimation (float                                                initialScale,
                                 float                                                maximumScale,
                                 unsigned int                                         nBounce,
                                 animation::BoxQuery <animation::Point>::Shared const &target,
                                 animation::stepper::Stepper::Shared            const &stepper);
                ~BounceAnimation ();

                float * const Matrix () const;
                float Progress () const;
                bool Step (unsigned int ms);
                std::array<animation::Vector4D, 4> Extremes (std::array<animation::Point, 4> const &corners) const;

                ANIMATION_DECLARE_PROPERTY (BounceAnimation, InitialScale, float)
                ANIMATION_DECLARE_PROPERTY (BounceAnimation, MaximumScale, float)
                ANIMATION_DECLARE_PROPERTY (BounceAnimation, NBounce, unsigned int)
                ANIMATION_DECLARE_PROPERTY (BounceAnimation, Stepper, animation::stepper::Stepper::Shared)
                ANIMATION_DECLARE_PROPERTY (BounceAnimation, Target, animation::BoxQuery <animation::Point>::Shared)

            private:

                struct Private;
                std::unique_ptr <Private> priv;
        };
    }
}
