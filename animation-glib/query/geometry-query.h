/*
 * animation-glib/query/geometry-query.h
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
 * GObject Interface for the BoxQuery <animation::Point> class.
 */
#pragma once

#include <glib-object.h>

#include <animation-glib/box.h>
#include <animation-glib/vector.h>

G_BEGIN_DECLS

#define ANIMATION_TYPE_BOX_QUERY animation_box_query_get_type ()
G_DECLARE_DERIVABLE_TYPE (AnimationBoxQuery, animation_box_query, ANIMATION, BOX_QUERY, GObject)

struct _AnimationBoxQueryClass {
  GObjectClass parent_class;
};

void animation_box_query_update (AnimationBoxQuery  *query,
                                 const AnimationBox *box);

AnimationBox * animation_box_query_geometry (AnimationBoxQuery *query);

AnimationBoxQuery * animation_box_query_new (void);

G_END_DECLS
