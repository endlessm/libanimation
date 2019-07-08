/*
 * animation-glib/query/geometry-query-private.h
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
 * Wrapper for the AnimationBoxQuery object into a C++ object satisfying
 * the animation::BoxQuery <animation::Point> interface.
 */
#pragma once

#include <glib-object.h>

#include <memory>

#include <animation/geometry.h>
#include <animation/query/geometry_query.h>
#include <animation-glib/query/geometry-query.h>

#ifdef __cplusplus
namespace animation
{
    namespace glib
    {
        class BoxQueryWrapper :
            public BoxQuery <animation::Point>
        {
            public:

                BoxQueryWrapper (AnimationBoxQuery *);
                ~BoxQueryWrapper ();

                geometry::Box <animation::Point> const & Geometry () const override;

                AnimationBoxQuery * BoxQuery () const;

            private:

                class Private;
                std::unique_ptr <Private> priv;
        };
    }
}
#endif

G_BEGIN_DECLS

animation::BoxQueryStorage <animation::Point> const &
animation_box_query_get_storage (AnimationBoxQuery *query);

G_END_DECLS
