/*
 * tests/animation/bounce_animation_test.cpp
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
 * Tests for the "bounce" animation.
 */
#include <cstddef>                      // for size_t
#include <functional>                   // for bind, __bind, _1

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gmock/gmock-cardinalities.h>  // for AtLeast
#include <gmock/gmock-generated-function-mockers.h>  // for FunctionMocker, etc
#include <gmock/gmock-matchers.h>       // for AnythingMatcher, etc
#include <gmock/gmock-spec-builders.h>  // for EXPECT_CALL, etc
#include <gtest/gtest.h>                // for TEST_F, Test, Types, etc

#include <animation/geometry.h>
#include <animation/bounce/bounce.h>
#include <animation/box_calculation.h>
#include <animation/query/geometry_query.h>
#include <animation/stepper/linear.h>

#include <glm_ostream_operators.h>

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::Test;

namespace ab = animation::bounce;
namespace abc = animation::box_calculation;
namespace agd = animation::geometry::dimension;
namespace as = animation::stepper;

namespace
{
    template <typename T>
    decltype(auto) BQ (animation::Box <T> &&box)
    {
        return animation::MakeStaticBoxQuery <T> (std::move (box));
    }

    template <typename T>
    decltype(auto) BQ (animation::Box <T> const &box)
    {
        return animation::MakeStaticBoxQuery <T> (box);
    }

    TEST (BounceAnimation, AnimationIncompleteBeforeLengthTimesteps)
    {
        ab::BounceAnimation anim (0.7f,
                                  1.5f,
                                  1,
                                  BQ (animation::Box <animation::Point> (animation::Point (0, 0),
                                                                         animation::Point (100, 100))),
                                  as::Linear (200));

        EXPECT_TRUE (anim.Step (199));
    }

    TEST (BounceAnimation, AnimationCompleteAtLengthTimesteps)
    {
        ab::BounceAnimation anim (0.7f,
                                  1.5f,
                                  1,
                                  BQ (animation::Box <animation::Point> (animation::Point (0, 0),
                                                                         animation::Point (100, 100))),
                                  as::Linear (200));

        EXPECT_FALSE (anim.Step (200));
    }

    TEST (BounceAnimation, AnimationStopsAtTargetBox)
    {
        auto target = animation::Box <animation::Point> (animation::Point (100, 100),
                                                         animation::Point (200, 200));
        ab::BounceAnimation anim (0.7f,
                                  1.5f,
                                  1,
                                  BQ (target),
                                  as::Linear (200));

        anim.Step (200);

        /* Apply to a shape at (0, 0) which is translated to (100, 100) */
        glm::mat4 translation (glm::translate (glm::mat4 (1.0), glm::vec3 (100, 100, 0)));
        glm::vec4 tl (0, 0, 0, 1);
        glm::vec4 br (100, 100, 0, 1);

        /* Apply transformation matrix to vectors */
        glm::mat4 transformation (glm::make_mat4x4 (anim.Matrix ()));

        /* Should finish at target box */
        EXPECT_THAT (translation * transformation * tl,
                     Eq (glm::vec4 (agd::get <0> (target.topLeft ()),
                                    agd::get <1> (target.topLeft ()),
                                    0,
                                    1)));
        EXPECT_THAT (translation * transformation * br,
                     Eq (glm::vec4 (agd::get <0> (target.bottomRight ()),
                                    agd::get <1> (target.bottomRight ()),
                                    0,
                                    1)));
    }

    TEST (BounceAnimation, AnimationStartsAtTargetBoxScaledByInitialScale)
    {
        auto target = animation::Box <animation::Point> (animation::Point (100, 100),
                                                         animation::Point (200, 200));
        auto width = 100.0f;
        auto height = 100.0f;
        auto initialScale = 0.5f;
        ab::BounceAnimation anim (initialScale,
                                  1.5f,
                                  1,
                                  BQ (target),
                                  as::Linear (200));

        animation::Point boxCenter (abc::ComputeBoxCenter (target));

        animation::Box <animation::Point> scaledBox (
            animation::Point (agd::get <0> (boxCenter) - (width / 2.0f) * initialScale,
                              agd::get <1> (boxCenter) - (height / 2.0f) * initialScale),
            animation::Point (agd::get <0> (boxCenter) + (width / 2.0f) * initialScale,
                              agd::get <1> (boxCenter) + (height / 2.0f) * initialScale)
        );

        /* Apply to a shape at (0, 0) which is translated to (100, 100) */
        glm::mat4 translation (glm::translate (glm::mat4 (1.0), glm::vec3 (100, 100, 0)));
        glm::vec4 tl (0, 0, 0, 1);
        glm::vec4 br (100, 100, 0, 1);

        /* Apply transformation matrix to vectors */
        glm::mat4 transformation (glm::make_mat4x4 (anim.Matrix ()));

        /* Should finish at target box */
        EXPECT_THAT (translation * transformation * tl,
                     Eq (glm::vec4 (agd::get <0> (scaledBox.topLeft ()),
                                    agd::get <1> (scaledBox.topLeft ()),
                                    0,
                                    1)));
        EXPECT_THAT (translation * transformation * br,
                     Eq (glm::vec4 (agd::get <0> (scaledBox.bottomRight ()),
                                    agd::get <1> (scaledBox.bottomRight ()),
                                    0,
                                    1)));
    }

    TEST (BounceAnimation, AtHalfwayPointBounceIsAtHighestAttenuatedScale)
    {
        auto target = animation::Box <animation::Point> (animation::Point (100, 100),
                                                         animation::Point (200, 200));
        auto width = 100.0f;
        auto height = 100.0f;
        auto initialScale = 0.5f;
        auto maximumScale = 1.5f;
        auto progress = 0.5;
        auto scaleFloor = (1.0f - initialScale) * progress;

        /* At 0.5f progress, we are at the top of the sine wave, so
         * add that to the scale floor. */
        auto expectedScale = scaleFloor + (maximumScale - initialScale);
        ab::BounceAnimation anim (initialScale,
                                  maximumScale,
                                  1,
                                  BQ (target),
                                  as::Linear (200));
        anim.Step (100);

        animation::Point boxCenter (abc::ComputeBoxCenter (target));

        animation::Box <animation::Point> scaledBox (
            animation::Point (agd::get <0> (boxCenter) - (width / 2.0f) * expectedScale,
                              agd::get <1> (boxCenter) - (height / 2.0f) * expectedScale),
            animation::Point (agd::get <0> (boxCenter) + (width / 2.0f) * expectedScale,
                              agd::get <1> (boxCenter) + (height / 2.0f) * expectedScale)
        );

        /* Apply to a shape at (0, 0) which is translated to (100, 100) */
        glm::mat4 translation (glm::translate (glm::mat4 (1.0), glm::vec3 (100, 100, 0)));
        glm::vec4 tl (0, 0, 0, 1);
        glm::vec4 br (100, 100, 0, 1);

        /* Apply transformation matrix to vectors */
        glm::mat4 transformation (glm::make_mat4x4 (anim.Matrix ()));

        /* Should finish at target box */
        EXPECT_THAT (translation * transformation * tl,
                     Eq (glm::vec4 (agd::get <0> (scaledBox.topLeft ()),
                                    agd::get <1> (scaledBox.topLeft ()),
                                    0,
                                    1)));
        EXPECT_THAT (translation * transformation * br,
                     Eq (glm::vec4 (agd::get <0> (scaledBox.bottomRight ()),
                                    agd::get <1> (scaledBox.bottomRight ()),
                                    0,
                                    1)));
    }
}
