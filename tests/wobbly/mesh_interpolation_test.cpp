/*
 * tests/wobbly/mesh_interpolation_test.cpp
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
 * Tests for the "wobbly" spring model.
 */
#include <array>                        // for array
#include <functional>                   // for function, __bind, __base, etc
#include <memory>                       // for unique_ptr
#include <sstream>                      // for basic_stringbuf<>::int_type, etc

#include <math.h>                       // for pow, ceil

#include <stddef.h>                     // for size_t

#include <gmock/gmock-matchers.h>       // for Matcher, ElementsAreArray, etc
#include <gmock/gmock.h>                // IWYU pragma: keep
#include <gtest/gtest-param-test.h>     // for Values, Combine, ValuesIn, etc
#include <gtest/gtest.h>                // for AssertHelper, TEST_F, Test, etc

#include <animation/wobbly/wobbly.h>    // for Point, PointView
#include <animation/wobbly/wobbly_internal.h>            // for BezierMesh, Height, Width, etc

#include <mathematical_model_matcher.h>  // for Eq, EqDispatchHelper, etc
#include <ostream_point_operator.h>     // for operator<<

using ::testing::ElementsAreArray;
using ::testing::Matcher;
using ::testing::Not;
using ::testing::Test;
using ::testing::ValuesIn;
using ::testing::WithParamInterface;

using ::animation::matchers::Eq;
using ::animation::matchers::SatisfiesModel;
using ::animation::matchers::WithSamples;
using ::animation::matchers::WithTolerance;

using ::animation::models::Linear;

namespace
{
    namespace agd = animation::geometry::dimension;

    constexpr double TextureWidth = 50.0f;
    constexpr double TextureHeight = 100.0f;
    animation::Point const TextureCenter = animation::Point (TextureWidth / 2,
                                                             TextureHeight / 2);

    double TileWidth (double width)
    {
        return width / (wobbly::config::Width - 1);
    }

    double TileHeight (double height)
    {
        return height / (wobbly::config::Height - 1);
    }

    void
    InitializePositionsWithDimensions (wobbly::MeshArray &positions,
                                       double            width,
                                       double            height)
    {
        double const meshWidth = wobbly::config::Width;
        double const meshHeight = wobbly::config::Height;
        double const tileWidth = TileWidth (width);
        double const tileHeight = TileHeight (height);

        for (unsigned int i = 0; i < meshHeight; ++i)
        {
            for (unsigned int j = 0; j < meshWidth; ++j)
            {
                animation::PointView <double> pv (positions,
                                                  i * meshWidth + j);
                agd::assign (pv, animation::Point (tileWidth * j,
                                                   tileHeight * i));
            }
        }
    }

    class BezierMesh :
        public Test
    {
        public:

            wobbly::BezierMesh mesh;

        protected:

            typedef animation::Point Point;

            typedef std::function <void (animation::PointView <double> &,
                                         size_t,
                                         size_t)> MeshTransform;
            typedef std::function <double (Point const &)> ResultFactory;

            void SetUp () override;
            void ApplyTransformation (MeshTransform const &);

            std::function <double (int)>
            UnitDeformationFunction (ResultFactory const      &resultFactory,
                                     wobbly::BezierMesh const &mesh,
                                     unsigned int             nSamples);
    };

    void
    BezierMesh::SetUp ()
    {
        InitializePositionsWithDimensions (mesh.PointArray (),
                                           TextureWidth,
                                           TextureHeight);
    }

    void
    BezierMesh::ApplyTransformation (MeshTransform const &transform)
    {
        for (size_t i = 0; i < wobbly::config::Height; ++i)
        {
            for (size_t j = 0; j < wobbly::config::Width; ++j)
            {
                auto pv (mesh.PointForIndex (j, i));
                transform (pv, j, i);
            }
        }
    }

    std::function <double (int)>
    BezierMesh::UnitDeformationFunction (ResultFactory const      &result,
                                         wobbly::BezierMesh const &mesh,
                                         unsigned int             nSamples)
    {
        auto deformation =
            [nSamples, &mesh, result](int lookupValue) -> double {
                /* Divide by the total number of samples (2^10) in order to get
                 * the unit value. */

                double const unitLookupValue =
                    lookupValue / static_cast <double> (pow (2, nSamples));
                auto const lookup (animation::Point (unitLookupValue,
                                                     unitLookupValue));
                auto p (mesh.DeformUnitCoordsToMeshSpace (lookup));

                return result (p);
            };

        return deformation;
    }

    TEST_F (BezierMesh, LinearHorizontalDeformationForUniformScale)
    {
        using namespace std::placeholders;

        ApplyTransformation (std::bind ([](animation::PointView <double> &pv) {
            agd::scale (pv, 2);
        }, _1));

        unsigned int const nSamples = 10;
        std::function <double (int)> horizontalDeformation =
            UnitDeformationFunction ([](animation::Point const &p) -> double {
                                        return agd::get <0> (p);
                                     },
                                     mesh,
                                     nSamples);

        EXPECT_THAT (horizontalDeformation,
                     SatisfiesModel (Linear <double> (),
                                     WithSamples (nSamples),
                                     WithTolerance (10e-5)));
    }

    TEST_F (BezierMesh, LinearVerticalDeformationForUniformScale)
    {
        using namespace std::placeholders;

        ApplyTransformation (std::bind ([](animation::PointView <double> &pv) {
            agd::scale (pv, 2);
        }, _1));

        unsigned int const nSamples = 10;
        std::function <double (int)> verticalDeformation =
            UnitDeformationFunction ([](animation::Point const &p) -> double {
                                        return agd::get <1> (p);
                                     },
                                     mesh,
                                     nSamples);

        EXPECT_THAT (verticalDeformation,
                     SatisfiesModel (Linear <double> (),
                                     WithSamples (nSamples),
                                     WithTolerance (10e-5)));
    }

    TEST_F (BezierMesh, Performance)
    {
       double x = 0;
       for (int i = 0; i < 100000000; ++i) {
         auto const p = mesh.DeformUnitCoordsToMeshSpace (animation::Point(i / 100000000.0, i / 100000.0));
         x += agd::get <0> (p);
       }

       EXPECT_EQ (x, 1.0);
    }

    /* Its somewhat difficult to test the non-linear case, considering
     * that the bezier patch evaluation is actually an interpolation
     * between four different parabolic functions. For now just check
     * if the deformation was non-linear */
    TEST_F (BezierMesh, NonLinearHorizontalDeformationForNonUniformScale)
    {
        using namespace std::placeholders;

        typedef animation::PointView <double> DoublePointView;

        ApplyTransformation ([](DoublePointView &pv, size_t x, size_t y) {
             agd::pointwise_scale (pv, animation::Point (x, y));
        });

        unsigned int const nSamples = 10;
        std::function <double (int)> horizontalDeformation =
            UnitDeformationFunction ([](animation::Point const &p) -> double {
                                         return agd::get <0> (p);
                                     },
                                     mesh,
                                     nSamples);

        EXPECT_THAT (horizontalDeformation,
                     Not (SatisfiesModel (Linear <double> (),
                                          WithSamples (nSamples),
                                          WithTolerance (10e-5))));
    }

    TEST_F (BezierMesh, ExtremesAreTextureEdges)
    {
        std::array <animation::Point, 4> const extremes = mesh.Extremes ();
        Matcher <animation::Point const &> const textureEdges[] =
        {
            Eq (animation::Point (0, 0)),
            Eq (animation::Point (TextureWidth, 0)),
            Eq (animation::Point (0, TextureHeight)),
            Eq (animation::Point (TextureWidth, TextureHeight))
        };

        EXPECT_THAT (extremes, ElementsAreArray (textureEdges));
    }

    template <typename Point>
    void PointCeiling (Point &p)
    {
        agd::for_each_coordinate (p, [](auto const &coord) -> decltype(auto) {
            return std::ceil (coord);
        });
    }

    class BezierMeshPoints :
        public BezierMesh,
        public WithParamInterface <animation::Point>
    {
    };

    animation::Point TexturePrediction (animation::Point const &unit)
    {
        animation::Point textureRelative (agd::get <1> (unit),
                                          agd::get <0> (unit));
        agd::pointwise_scale (textureRelative,
                              animation::Point (TextureWidth,
                                                TextureHeight));
        PointCeiling (textureRelative);
        return textureRelative;
    }

    animation::Point MeshInterpolation (wobbly::BezierMesh const &mesh,
                                        animation::Point const &unit)
    {
        animation::Point meshRelative (mesh.DeformUnitCoordsToMeshSpace (unit));
        PointCeiling (meshRelative);
        return meshRelative;
    }

    TEST_P (BezierMeshPoints, LinearDefinitionForNoTransformation)
    {
        auto point (GetParam ());
        auto prediction (TexturePrediction (point));
        auto interpolated (MeshInterpolation (mesh, point));

        EXPECT_THAT (interpolated, Eq (prediction));
    }

    animation::Point const unitMeshExtremes[] =
    {
        animation::Point (0.0, 0.0),
        animation::Point (0.0, 1.0),
        animation::Point (1.0, 0.0),
        animation::Point (1.0, 1.0)
    };

    INSTANTIATE_TEST_CASE_P (UnitExtremes,
                             BezierMeshPoints,
                             ValuesIn (unitMeshExtremes));
}
