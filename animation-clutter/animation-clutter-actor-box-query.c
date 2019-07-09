/*
 * animation-clutter/animation-clutter-actor-box-query.cpp
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
 * AnimationClutterActorBoxQuery subclass for ClutterActor
 */

#include <animation-clutter/animation-clutter-actor-box-query.h>

struct _AnimationClutterActorBoxQuery
{
  GObject parent_instance;
};

typedef struct _AnimationClutterActorBoxQueryPrivate
{
  ClutterActor *actor;
} AnimationClutterActorBoxQueryPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AnimationClutterActorBoxQuery,
                            animation_clutter_actor_box_query,
                            ANIMATION_TYPE_BOX_QUERY)


enum {
  PROP_0,
  PROP_ACTOR,
  NPROPS
};

static GParamSpec *animation_clutter_actor_box_query_props[NPROPS] = { NULL, };

static void
update_box_for_actor_geometry (AnimationClutterActorBoxQuery *box_query,
                               ClutterActor                  *actor)
{
  float x, y, width, height;

  clutter_actor_get_position (actor, &x, &y);
  clutter_actor_get_size (actor, &width, &height);

  const AnimationBox box = {
    .top_left = { x, y },
    .bottom_right = { x + width, y + height }
  };

  animation_box_query_update (ANIMATION_BOX_QUERY (box_query), &box);
}

static void
actor_geometry_changed (GObject    *object,
                        GParamSpec *pspec,
                        gpointer    user_data)
{
  ClutterActor *actor = CLUTTER_ACTOR (object);
  AnimationClutterActorBoxQuery *box_query = ANIMATION_CLUTTER_ACTOR_BOX_QUERY (user_data);

  update_box_for_actor_geometry (box_query, actor);
}

static void
disconnect_signals_and_unref (gpointer data)
{
  ClutterActor *actor = data;

  g_signal_handlers_disconnect_by_data (actor, actor);
  g_object_unref (actor);
}

static ClutterActor *
connect_signals_and_update (ClutterActor                  *actor,
                            AnimationClutterActorBoxQuery *box_query)
{
  g_signal_connect_object (actor, "notify::x", G_CALLBACK (actor_geometry_changed), box_query, G_CONNECT_AFTER);
  g_signal_connect_object (actor, "notify::y", G_CALLBACK (actor_geometry_changed), box_query, G_CONNECT_AFTER);
  g_signal_connect_object (actor, "notify::width", G_CALLBACK (actor_geometry_changed), box_query, G_CONNECT_AFTER);
  g_signal_connect_object (actor, "notify::height", G_CALLBACK (actor_geometry_changed), box_query, G_CONNECT_AFTER);

  update_box_for_actor_geometry (box_query, actor);

  return actor;
}

static void
animation_clutter_actor_box_query_set_property (GObject      *object,
                                                guint         prop_id,
                                                const GValue *value,
                                                GParamSpec   *pspec)
{
  AnimationClutterActorBoxQuery *box_query = ANIMATION_CLUTTER_ACTOR_BOX_QUERY (object);
  AnimationClutterActorBoxQueryPrivate *priv = animation_clutter_actor_box_query_get_instance_private (box_query);

  switch (prop_id)
    {
    case PROP_ACTOR:
      g_clear_pointer (&priv->actor, (GDestroyNotify) disconnect_signals_and_unref);
      priv->actor = connect_signals_and_update (CLUTTER_ACTOR (g_value_dup_object (value)), box_query);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
animation_clutter_actor_box_query_get_property (GObject    *object,
                                            guint       prop_id,
                                            GValue     *value,
                                            GParamSpec *pspec)
{
  AnimationClutterActorBoxQuery *box_query = ANIMATION_CLUTTER_ACTOR_BOX_QUERY (object);
  AnimationClutterActorBoxQueryPrivate *priv = animation_clutter_actor_box_query_get_instance_private (box_query);

  switch (prop_id)
    {
    case PROP_ACTOR:
      g_value_set_object (value, priv->actor);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
animation_clutter_actor_box_query_dispose (GObject *object)
{
  AnimationClutterActorBoxQuery *box_query = ANIMATION_CLUTTER_ACTOR_BOX_QUERY (object);
  AnimationClutterActorBoxQueryPrivate *priv = animation_clutter_actor_box_query_get_instance_private (box_query);

  g_clear_object (&priv->actor);

  G_OBJECT_CLASS (animation_clutter_actor_box_query_parent_class)->dispose (object);
}

static void
animation_clutter_actor_box_query_class_init (AnimationClutterActorBoxQueryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = animation_clutter_actor_box_query_get_property;
  object_class->set_property = animation_clutter_actor_box_query_set_property;
  object_class->dispose = animation_clutter_actor_box_query_dispose;

  animation_clutter_actor_box_query_props[PROP_ACTOR] =
    g_param_spec_object ("actor",
                         "Clutter Actor",
                         "The ClutterActor to observe",
                         CLUTTER_TYPE_ACTOR,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class,
                                     NPROPS,
                                     animation_clutter_actor_box_query_props);
}

static void
animation_clutter_actor_box_query_init (AnimationClutterActorBoxQuery *box_query)
{
}

AnimationBoxQuery *
animation_clutter_actor_box_query_new_for_actor (ClutterActor *actor)
{
  return ANIMATION_BOX_QUERY (g_object_new (ANIMATION_CLUTTER_TYPE_ACTOR_BOX_QUERY,
                                            "actor", actor,
                                            NULL));
}
