/*
 * animation-glib/query/geometry-query-internal.cpp
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
 * Wrapper for the AnimationBoxQuery object into a C++ object satisfying
 * the animation::BoxQuery <animation::Point> interface, implementation.
 */

#include <animation/geometry.h>
#include <animation/query/geometry_query.h>
#include <animation-glib/query/geometry-query-private.h>

namespace ag = animation::glib;

namespace animation
{
    namespace glib
    {
        struct BoxQueryWrapper::Private
        {
            Private (AnimationBoxQuery *);
            ~Private ();
        
            AnimationBoxQuery *query;
        };
    }
}

ag::BoxQueryWrapper::Private::Private (AnimationBoxQuery *query) :
    query (ANIMATION_BOX_QUERY (g_object_ref (query)))
{
}

ag::BoxQueryWrapper::Private::~Private ()
{
    g_clear_object (&query);
}

animation::Box <animation::Point> const &
ag::BoxQueryWrapper::Geometry () const
{
    return animation_box_query_get_storage (priv->query).Geometry ();
}

AnimationBoxQuery *
ag::BoxQueryWrapper::BoxQuery () const
{
    return priv->query;
}

ag::BoxQueryWrapper::BoxQueryWrapper (AnimationBoxQuery *query) :
    priv (std::make_unique <ag::BoxQueryWrapper::Private> (query))
{
}

ag::BoxQueryWrapper::~BoxQueryWrapper ()
{
}

