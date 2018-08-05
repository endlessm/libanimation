/*
 * animation/zoom/zoom.h
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
 * Animation that causes a surface to zoom from one rectangle to another.
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
    namespace zoom
    {
        class ZoomAnimation :
            public animation::transform::TransformAnimation
        {
            public:

                ZoomAnimation ();
                ZoomAnimation (animation::BoxQuery <animation::Point>::Shared const &source,
                               animation::BoxQuery <animation::Point>::Shared const &target,
                               animation::stepper::Stepper::Shared            const &stepper);
                ~ZoomAnimation ();

                float * const Matrix () const;
                float Progress () const;
                bool Step (unsigned int ms);
                std::array<animation::Vector4D, 4> Extremes (std::array<animation::Point, 4> const &corners) const;

                ANIMATION_DECLARE_PROPERTY (ZoomAnimation, Source, animation::BoxQuery <animation::Point>::Shared)
                ANIMATION_DECLARE_PROPERTY (ZoomAnimation, Target, animation::BoxQuery <animation::Point>::Shared)
                ANIMATION_DECLARE_PROPERTY (ZoomAnimation, Stepper, animation::stepper::Stepper::Shared)

            private:

                struct Private;
                std::unique_ptr <Private> priv;
        };
    }
}
