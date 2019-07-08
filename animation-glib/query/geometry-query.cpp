/*
 * animation-glib/query/geometry-query.cpp
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
 * GObject interface for the BoxQuery <animation::Point> class.
 */

#include <memory>

#include <animation/query/geometry_query.h>
#include <animation-glib/box.h>
#include <animation-glib/query/geometry-query.h>
#include <animation-glib/query/geometry-query-private.h>
#include <animation-glib/vector.h>

namespace agd = animation::geometry::dimension;

typedef struct _AnimationBoxQueryPrivate
{
  animation::BoxQueryStorage <animation::Point> query;
} AnimationBoxQueryPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AnimationBoxQuery,
                            animation_box_query,
                            G_TYPE_OBJECT)

/**
 * animation_box_query_update:
 * @query: An #AnimationBoxQuery
 * @box: An #AnimationBox
 *
 * Update the internal geometry in this geometry query.
 */
void
animation_box_query_update (AnimationBoxQuery  *query,
                            const AnimationBox *box)
{
  AnimationBoxQueryPrivate *priv =
    reinterpret_cast <AnimationBoxQueryPrivate *> (animation_box_query_get_instance_private (query));

  priv->query.Update (animation::Box <animation::Point> (animation::Point (box->top_left.x,
                                                                           box->top_left.y),
                                                         animation::Point (box->bottom_right.x,
                                                                           box->bottom_right.y)));
}

/**
 * animation_box_query_get_storage:
 * @query: An #AnimationBoxQuery
 *
 * Get the underlying storage for the #AnimationBoxQuery. This is
 * an internal method that is used by animation::glib::BoxQueryWrapper
 * to directly get the geometry referred to in the storage.
 *
 * Return: An animation::BoxQueryStorage <animation::Point> used by this #AnimationBoxQuery.
 */ 
animation::BoxQueryStorage <animation::Point> const &
animation_box_query_get_storage (AnimationBoxQuery *query)
{
  AnimationBoxQueryPrivate *priv =
    reinterpret_cast <AnimationBoxQueryPrivate *> (animation_box_query_get_instance_private (query));

  return priv->query;
}

/**
 * animation_box_query_get_geometry:
 * @query: An #AnimationBoxQuery
 * @out_box: (out caller-allocates): An #AnimationBox to write the return value to.
 *
 * Get the currently stored geometry as an AnimationBox.
 */
void
animation_box_query_geometry (AnimationBoxQuery *query,
                              AnimationBox      *out_box)
{
  AnimationBoxQueryPrivate *priv =
    reinterpret_cast <AnimationBoxQueryPrivate *> (animation_box_query_get_instance_private (query));

  g_return_if_fail (out_box != NULL);

  auto box = priv->query.Geometry ();

  out_box->top_left.x = agd::get <0> (box.topLeft ());
  out_box->top_left.y = agd::get <1> (box.topLeft ());
  out_box->bottom_right.x = agd::get <0> (box.bottomRight ());
  out_box->bottom_right.y = agd::get <1> (box.bottomRight ());
}

static void
animation_box_query_finalize (GObject *object)
{
  AnimationBoxQuery *query = ANIMATION_BOX_QUERY (object);
  AnimationBoxQueryPrivate *priv =
    reinterpret_cast <AnimationBoxQueryPrivate *> (animation_box_query_get_instance_private (query));

  /* Explicitly call destructor, which will release the pointer
   * and return the memory to an uninitialized state */
  priv->query.~BoxQueryStorage();

  G_OBJECT_CLASS (animation_box_query_parent_class)->finalize (object);
}

static void
animation_box_query_init (AnimationBoxQuery *query)
{
  AnimationBoxQueryPrivate *priv =
    reinterpret_cast <AnimationBoxQueryPrivate *> (animation_box_query_get_instance_private (query));

  new (&priv->query) animation::BoxQueryStorage <animation::Point> (
    animation::Box <animation::Point> (
      animation::Point (0, 0),
      animation::Point (1, 1)
    )
  );
}

static void
animation_box_query_class_init (AnimationBoxQueryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = animation_box_query_finalize;
}

/**
 * animation_box_query_new:
 *
 * Create a new AnimationBoxQuery * with default-initialized geometry.
 *
 * You probably don't want to directly use this, and instead use a derived class
 * class constructor.
 *
 * Returns: A new #AnimationBoxQuery
 */
AnimationBoxQuery *
animation_box_query_new (void)
{
  return ANIMATION_BOX_QUERY (g_object_new (ANIMATION_TYPE_BOX_QUERY, NULL));
}
