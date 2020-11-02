/*
 * animation/glide/glide.h
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
 * An animation that causes a surface to glide onto screen, gently
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
    namespace glide
    {
        class GlideAnimation :
            public animation::transform::TransformAnimation
        {
            public:

                GlideAnimation ();
                GlideAnimation (float                                                 initialDistance,
                                float                                                 xRotationAngleDegrees,
                                float                                                 yRotationAngleDegrees,
                                float                                                 xAxisLocationUnit,
                                float                                                 yAxisLocationUnit,
                                animation::BoxQuery <animation::Point>::Shared const &viewportQuery,
                                animation::BoxQuery <animation::Point>::Shared const &target,
                                animation::stepper::Stepper::Shared            const &stepper);
                ~GlideAnimation ();

                float * const Matrix () const;
                float Progress () const;
                bool Step (unsigned int ms);
                std::array<animation::Vector4D, 4> Extremes (std::array<animation::Point, 4> const &corners) const;

                ANIMATION_DECLARE_PROPERTY (GlideAnimation, InitialDistance, float)
                ANIMATION_DECLARE_PROPERTY (GlideAnimation, XRotationAngleDegrees, float)
                ANIMATION_DECLARE_PROPERTY (GlideAnimation, YRotationAngleDegrees, float)
                ANIMATION_DECLARE_PROPERTY (GlideAnimation, XAxisLocationUnit, float)
                ANIMATION_DECLARE_PROPERTY (GlideAnimation, YAxisLocationUnit, float)
                ANIMATION_DECLARE_PROPERTY (GlideAnimation, MaximumScale, float)
                ANIMATION_DECLARE_PROPERTY (GlideAnimation, Target, animation::BoxQuery <animation::Point>::Shared)
                ANIMATION_DECLARE_PROPERTY (GlideAnimation, Viewport, animation::BoxQuery <animation::Point>::Shared)
                ANIMATION_DECLARE_PROPERTY (GlideAnimation, Stepper, animation::stepper::Stepper::Shared)

            private:

                struct Private;
                std::unique_ptr <Private> priv;
        };
    }
}
