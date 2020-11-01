/*
 * animation-clutter/animation-clutter-grid-effect.c
 *
 * Copyright © 2013-2018 Endless Mobile, Inc.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * licence or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Sam Spilsbury <sam@endlessm.com>
 */

#include <math.h>

#include <glib-object.h>
#include <gio/gio.h>
#include <cogl/cogl.h>
#include <clutter/clutter.h>

#include <animation-glib/vector.h>
#include <animation-glib/grid/grid.h>

#include "animation-clutter-grid-effect.h"
#include "animation-clutter-common.h"

struct _AnimationClutterGridEffect {
  ClutterDeformEffect parent_instance;
};

typedef struct
{
  AnimationGridAnimation *grid_animation;
  gint64                  last_usecs;
  guint                   timeout_id;
} AnimationClutterGridEffectPrivate;

enum
{
  PROP_0,
  PROP_GRID_ANIMATION,
  PROP_LAST
};

static GParamSpec *object_properties[PROP_LAST];

G_DEFINE_TYPE_WITH_PRIVATE (AnimationClutterGridEffect,
                            animation_clutter_grid_effect,
                            CLUTTER_TYPE_DEFORM_EFFECT)

static gboolean
animation_clutter_grid_effect_modify_paint_volume (ClutterEffect    *effect,
                                                   ClutterPaintVolume *volume)
{
  ClutterActorMeta *meta = CLUTTER_ACTOR_META (effect);
  ClutterActor *actor = clutter_actor_meta_get_actor (meta);
  AnimationClutterGridEffect *grid_effect = ANIMATION_CLUTTER_GRID_EFFECT (effect);
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (grid_effect);

  /* We assume that the parent's modify_paint_volume method always returns
   * TRUE here. */
  CLUTTER_EFFECT_CLASS (animation_clutter_grid_effect_parent_class)->modify_paint_volume (effect, volume);

  if (priv->grid_animation && clutter_actor_meta_get_enabled (meta))
    {
      ClutterActorBox box;
      float actor_x, actor_y;

      animation_clutter_get_untransformed_paint_box_from_existing_volume (actor, volume, &box);
      clutter_actor_get_position (actor, &actor_x, &actor_y);

      AnimationVector corners[4] = {
        { box.x1, box.y1 },
        { box.x2, box.y1 },
        { box.x1, box.y2 },
        { box.x2, box.y2 }
      };
      AnimationVector4D extremes[4];

      animation_grid_animation_extremes (priv->grid_animation,
                                         corners,
                                         extremes);

      float x1 = MIN (extremes[0].x, extremes[2].x);
      float y1 = MIN (extremes[0].y, extremes[1].y);
      float z1 = MIN (MIN (extremes[0].z, extremes[1].z),
                      MIN (extremes[2].z, extremes[3].z));
      float x2 = MAX (extremes[1].x, extremes[3].x);
      float y2 = MAX (extremes[2].y, extremes[3].y);
      float z2 = MAX (MAX (extremes[0].z, extremes[1].z),
                      MAX (extremes[2].z, extremes[3].z));

      g_autoptr(ClutterPaintVolume) extremes_volume =
        clutter_paint_volume_copy (volume);

      ClutterVertex const origin = { x1 - actor_x, y1 - actor_y, z1 };
      clutter_paint_volume_set_origin (extremes_volume, &origin);
      clutter_paint_volume_set_width (extremes_volume, MAX (x2 - x1, 1.0));
      clutter_paint_volume_set_height (extremes_volume, MAX (y2 - y1, 1.0));
      clutter_paint_volume_set_depth (extremes_volume, z2 - z1);

      clutter_paint_volume_union (volume, extremes_volume);
    }

  return TRUE;
}

static void
animation_clutter_grid_effect_deform_vertex (ClutterDeformEffect *effect,
                                             gfloat               width G_GNUC_UNUSED,
                                             gfloat               height G_GNUC_UNUSED,
                                             CoglTextureVertex   *vertex)
{
  ClutterActor *actor = clutter_actor_meta_get_actor (CLUTTER_ACTOR_META (effect));
  AnimationClutterGridEffect *grid_effect = ANIMATION_CLUTTER_GRID_EFFECT (effect);
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (grid_effect);
  AnimationVector uv = { vertex->tx, vertex->ty };
  AnimationVector deformed;
  float x, y;

  animation_grid_animation_deform_uv_to_model_space (priv->grid_animation,
                                                     &uv,
                                                     &deformed);

  clutter_actor_get_position (actor, &x, &y);

  vertex->x = deformed.x - x;
  vertex->y = deformed.y - y;
}


static gboolean
animation_clutter_grid_effect_new_frame (gpointer user_data)
{
  AnimationClutterGridEffect *grid_effect = ANIMATION_CLUTTER_GRID_EFFECT (user_data);
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (grid_effect);
  ClutterActorMeta *meta = CLUTTER_ACTOR_META (grid_effect);
  gint64 msecs = g_get_monotonic_time ();

  static const unsigned int ms_to_us = 1000;

  g_assert (priv->grid_animation);

  /* Wraparound, priv->last_usecs -= G_MAXINT64.
   * We make priv->last_usecs negative so that subtracting it
   * from msecs results in the correct delta */
  if (G_UNLIKELY (priv->last_usecs > msecs))
    priv->last_usecs -= G_MAXINT64;

  gint64 msecs_delta = (msecs - priv->last_usecs) / ms_to_us;
  priv->last_usecs = msecs;

  /* If there was no time movement, then we can't really step or remove
   * models in a way that makes sense, so don't do it */
  if (msecs_delta == 0)
    return G_SOURCE_CONTINUE;

  if (!animation_grid_animation_step (priv->grid_animation, msecs_delta))
    {
      /* Disable the effect */
      clutter_actor_meta_set_enabled (meta, FALSE);

      /* Finally, return false so that we don't keep animating */
      priv->timeout_id = 0;
      return G_SOURCE_REMOVE;
    }
  else
    {
      clutter_actor_meta_set_enabled (meta, TRUE);
      clutter_deform_effect_invalidate (CLUTTER_DEFORM_EFFECT (grid_effect));
    }

  /* We always want to return true even if there was no time delta */
  return G_SOURCE_CONTINUE;
}

static void
animation_clutter_grid_effect_ensure_timeline (AnimationClutterGridEffect *grid_effect)
{
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (grid_effect);

  if (priv->timeout_id == 0)
    {
      static const unsigned int frame_length_ms = 16; /* 1000 / 60 */;

      priv->last_usecs = g_get_monotonic_time ();
      priv->timeout_id = g_timeout_add (frame_length_ms, animation_clutter_grid_effect_new_frame, grid_effect);

      ClutterActor *actor = clutter_actor_meta_get_actor (CLUTTER_ACTOR_META (grid_effect));

      /* Make sure to show the actor so that we can see the animation
       * if the animation was on open or unminimize */
      clutter_actor_show (actor);
    }
}

static void
animation_clutter_grid_effect_notify (GObject    *object,
                                      GParamSpec *pspec)
{
  ClutterActorMeta *actor_meta = CLUTTER_ACTOR_META (object);
  AnimationClutterGridEffect *grid_effect = ANIMATION_CLUTTER_GRID_EFFECT (object);
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (grid_effect);

  if (g_strcmp0 (pspec->name, "enabled") == 0)
    {
      if (clutter_actor_meta_get_enabled (actor_meta))
        animation_clutter_grid_effect_ensure_timeline (grid_effect);
      else
        g_clear_handle_id (&priv->timeout_id, g_source_remove);
    }

  G_OBJECT_CLASS (animation_clutter_grid_effect_parent_class)->notify (object, pspec);
}

static void
animation_clutter_grid_effect_set_actor (ClutterActorMeta *actor_meta,
                                         ClutterActor     *actor)
{
  CLUTTER_ACTOR_META_CLASS (animation_clutter_grid_effect_parent_class)->set_actor (actor_meta, actor);

  AnimationClutterGridEffect *grid_effect = ANIMATION_CLUTTER_GRID_EFFECT (actor_meta);
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (grid_effect);

  /* Disable the effect before we do anything else - this ensures that
   * for instance, we get the right paint box because we don't have applied
   * effects that haven't had their paint boxes computed. */
  clutter_actor_meta_set_enabled (actor_meta, FALSE);

  g_clear_handle_id (&priv->timeout_id, g_source_remove);
}

static void
animation_clutter_grid_effect_set_property (GObject      *object,
                                            guint        prop_id,
                                            const GValue *value,
                                            GParamSpec   *pspec)
{
  AnimationClutterGridEffect *grid_effect = ANIMATION_CLUTTER_GRID_EFFECT (object);
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (grid_effect);

  switch (prop_id)
    {
    case PROP_GRID_ANIMATION:
      g_set_object (&priv->grid_animation, g_value_dup_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
animation_clutter_grid_effect_dispose (GObject *object)
{
  AnimationClutterGridEffect *grid_effect = ANIMATION_CLUTTER_GRID_EFFECT (object);
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (grid_effect);

  g_clear_object (&priv->grid_animation);

  G_OBJECT_CLASS (animation_clutter_grid_effect_parent_class)->dispose (object);
}

static void
animation_clutter_grid_effect_finalize (GObject *object)
{
  AnimationClutterGridEffect *grid_effect = ANIMATION_CLUTTER_GRID_EFFECT (object);
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (grid_effect);

  g_clear_handle_id (&priv->timeout_id, g_source_remove);

  G_OBJECT_CLASS (animation_clutter_grid_effect_parent_class)->finalize (object);
}

static void
animation_clutter_grid_effect_init (AnimationClutterGridEffect *effect)
{
  AnimationClutterGridEffectPrivate *priv =
    animation_clutter_grid_effect_get_instance_private (effect);

  priv->timeout_id = 0;
}

static void
animation_clutter_grid_effect_class_init (AnimationClutterGridEffectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  ClutterActorMetaClass *meta_class = CLUTTER_ACTOR_META_CLASS (klass);
  ClutterEffectClass *effect_class = CLUTTER_EFFECT_CLASS (klass);
  ClutterDeformEffectClass *deform_class = CLUTTER_DEFORM_EFFECT_CLASS (klass);

  object_class->notify = animation_clutter_grid_effect_notify;
  object_class->set_property = animation_clutter_grid_effect_set_property;
  object_class->dispose = animation_clutter_grid_effect_dispose;
  object_class->finalize = animation_clutter_grid_effect_finalize;
  meta_class->set_actor = animation_clutter_grid_effect_set_actor;
  effect_class->modify_paint_volume = animation_clutter_grid_effect_modify_paint_volume;
  deform_class->deform_vertex = animation_clutter_grid_effect_deform_vertex;

  object_properties[PROP_GRID_ANIMATION] =
    g_param_spec_object ("grid-animation",
                         "Grid Animation",
                         "The underlying grid animation",
                         ANIMATION_TYPE_GRID_ANIMATION,
                         G_PARAM_WRITABLE);

  g_object_class_install_properties (object_class, PROP_LAST, object_properties);
}

ClutterEffect *
animation_clutter_grid_effect_new (AnimationGridAnimation *grid_animation)
{
  return g_object_new (ANIMATION_CLUTTER_TYPE_GRID_EFFECT,
                       "grid-animation", grid_animation,
                       NULL);
}
