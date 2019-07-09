/*
 * animation-clutter/animation-clutter-actor-box-query.h
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
 * Geometry query subclass for ClutterActor
 */
#pragma once

#include <glib-object.h>

#include <clutter/clutter.h>

#include <animation-glib/box.h>
#include <animation-glib/query/geometry-query.h>

G_BEGIN_DECLS

#define ANIMATION_CLUTTER_TYPE_ACTOR_BOX_QUERY animation_clutter_actor_box_query_get_type ()
G_DECLARE_FINAL_TYPE (AnimationClutterActorBoxQuery, animation_clutter_actor_box_query, ANIMATION_CLUTTER, ACTOR_BOX_QUERY, GObject)

AnimationBoxQuery * animation_clutter_actor_box_query_new_for_actor (ClutterActor *actor);

G_END_DECLS
