/*
 * animation-clutter/animation-clutter-affine-effect.c
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

#include <animation-glib/transform/transform.h>
#include <animation-glib/vector.h>

#include <gio/gio.h>
#include <glib-object.h>
#include <clutter/clutter.h>
#include <cogl/cogl.h>
#include <math.h>

#include "animation-clutter-affine-effect.h"
#include "animation-clutter-common.h"

struct _AnimationClutterAffineEffect {
  ClutterEffect parent_instance;
};

typedef struct
{
  AnimationTransformAnimation *transform_animation;
  gint64                       last_usecs;
  guint                        timeout_id;
} AnimationClutterAffineEffectPrivate;

enum
{
  PROP_0,
  PROP_TRANSFORM_ANIMATION,
  PROP_LAST
};

static GParamSpec *object_properties[PROP_LAST];

G_DEFINE_TYPE_WITH_PRIVATE (AnimationClutterAffineEffect,
                            animation_clutter_affine_effect,
                            CLUTTER_TYPE_EFFECT)

static gboolean
animation_clutter_affine_effect_modify_paint_volume (ClutterEffect      *effect,
                                                     ClutterPaintVolume *volume)
{
  ClutterActorMeta *meta = CLUTTER_ACTOR_META (effect);
  ClutterActor *actor = clutter_actor_meta_get_actor (meta);
  AnimationClutterAffineEffect *affine_effect = ANIMATION_CLUTTER_AFFINE_EFFECT (effect);
  AnimationClutterAffineEffectPrivate *priv =
    animation_clutter_affine_effect_get_instance_private (affine_effect);

  /* We assume that the parent's get_paint_volume method always returns
   * TRUE here. */
  CLUTTER_EFFECT_CLASS (animation_clutter_affine_effect_parent_class)->modify_paint_volume (effect, volume);

  if (priv->transform_animation && clutter_actor_meta_get_enabled (meta))
    {
      AnimationVector   corners[4];
      AnimationVector4D extremes[4];
      AnimationVector   offset;

      animation_clutter_compute_corners_from_untransformed_paint_volume (actor,
                                                                        volume,
                                                                        corners,
                                                                        &offset);

      animation_transform_animation_extremes (priv->transform_animation,
                                              corners,
                                              extremes);

      animation_clutter_expand_paint_volume_with_extremes (volume, extremes, &offset);
    }

  return TRUE;
}

static gboolean
animation_clutter_affine_effect_new_frame (gpointer user_data)
{
  AnimationClutterAffineEffect *affine_effect = ANIMATION_CLUTTER_AFFINE_EFFECT (user_data);
  AnimationClutterAffineEffectPrivate *priv =
    animation_clutter_affine_effect_get_instance_private (affine_effect);
  ClutterActorMeta *meta = CLUTTER_ACTOR_META (affine_effect);
  ClutterActor *actor = clutter_actor_meta_get_actor (meta);
  gint64 usecs = g_get_monotonic_time ();

  static const unsigned int ms_to_us = 1000;

  g_assert (priv->transform_animation);

  /* Wraparound, priv->last_usecs -= G_MAXINT64.
   * We make priv->last_usecs negative so that subtracting it
   * from usecs results in the correct delta */
  if (G_UNLIKELY (priv->last_usecs > usecs))
    priv->last_usecs -= G_MAXINT64;

  gint64 msecs_delta = (usecs - priv->last_usecs) / ms_to_us;
  priv->last_usecs = usecs;

  /* If there was no time movement, then we can't really step or remove
   * models in a way that makes sense, so don't do it */
  if (msecs_delta == 0)
    return G_SOURCE_CONTINUE;

  if (!animation_transform_animation_step (priv->transform_animation, msecs_delta))
    {
      /* Reset the transform back to an identity matrix. This will
       * also cause the transform-set property to be unset. We
       * need to do this before the actor effect is disabled, since
       * disabling it may cause the actor to be destroyed
       * and the actor to be detached from the effect. */
      graphene_matrix_t matrix;
      graphene_matrix_init_identity (&matrix);
      clutter_actor_set_opacity (actor, 255);
      clutter_actor_set_transform (actor,
                                   (const graphene_matrix_t *) &matrix);

      /* Disable the effect */
      clutter_actor_meta_set_enabled (meta, FALSE);

      /* Finally, return false so that we don't keep animating */
      priv->timeout_id = 0;
      return G_SOURCE_REMOVE;
    }
  else
    {
      /* We need to immediately set the opacity of the actor
       * since if it is zero, then clutter_actor_paint will
       * never get called, causing us to never be able to
       * update the opacity of the actor */
      float scaled_opacity = animation_transform_animation_progress (priv->transform_animation) * 255.0;
      guint8 opacity = (guint8) (scaled_opacity);

      clutter_actor_set_opacity (actor, opacity);
      clutter_effect_queue_repaint (CLUTTER_EFFECT (affine_effect));
    }

  /* We always want to return true even if there was no time delta */
  return G_SOURCE_CONTINUE;
}

static void
animation_clutter_affine_effect_paint (ClutterEffect           *effect,
                                       ClutterPaintContext     *context,
                                       ClutterEffectPaintFlags  flags)
{
  ClutterActorMeta *meta = CLUTTER_ACTOR_META (effect);
  ClutterActor *actor = clutter_actor_meta_get_actor (meta);
  AnimationClutterAffineEffect *affine_effect = ANIMATION_CLUTTER_AFFINE_EFFECT (effect);
  AnimationClutterAffineEffectPrivate *priv =
    animation_clutter_affine_effect_get_instance_private (affine_effect);

  /* Apply the transform to the actor */
  graphene_matrix_t matrix;
  graphene_matrix_init_from_float (&matrix,
                                   animation_transform_animation_matrix (priv->transform_animation));

  clutter_actor_set_pivot_point (actor, 0, 0);
  clutter_actor_set_transform (actor, &matrix);
  clutter_actor_continue_paint (actor, context);
}

static void
animation_clutter_affine_effect_ensure_timeline (AnimationClutterAffineEffect *affine_effect)
{
  AnimationClutterAffineEffectPrivate *priv =
    animation_clutter_affine_effect_get_instance_private (affine_effect);

  if (priv->timeout_id == 0)
    {
      static const unsigned int frame_length_ms = 16; /* 1000 / 60; */

      priv->last_usecs = g_get_monotonic_time ();
      priv->timeout_id = g_timeout_add (frame_length_ms, animation_clutter_affine_effect_new_frame, affine_effect);

      /* We need to show the actor and set the initial transform now to prevent flicker */
      graphene_matrix_t matrix;
      graphene_matrix_init_from_float (&matrix,
                                       animation_transform_animation_matrix (priv->transform_animation));

      float scaled_opacity = animation_transform_animation_progress (priv->transform_animation) * 255.0;
      guint8 opacity = (guint8) (scaled_opacity);

      ClutterActor *actor = clutter_actor_meta_get_actor (CLUTTER_ACTOR_META (affine_effect));

      clutter_actor_set_opacity (actor, opacity);
      clutter_actor_set_pivot_point (actor, 0, 0);
      clutter_actor_set_transform (actor, &matrix);
      clutter_actor_show (actor);
    }
}

static void
animation_clutter_affine_effect_notify (GObject    *object,
                                        GParamSpec *pspec)
{
  ClutterActorMeta *actor_meta = CLUTTER_ACTOR_META (object);
  AnimationClutterAffineEffect *affine_effect = ANIMATION_CLUTTER_AFFINE_EFFECT (object);
  AnimationClutterAffineEffectPrivate *priv =
    animation_clutter_affine_effect_get_instance_private (affine_effect);

  if (g_strcmp0 (pspec->name, "enabled") == 0)
    {
      if (clutter_actor_meta_get_enabled (actor_meta))
        animation_clutter_affine_effect_ensure_timeline (affine_effect);
      else
        g_clear_handle_id (&priv->timeout_id, g_source_remove);
    }

  G_OBJECT_CLASS (animation_clutter_affine_effect_parent_class)->notify (object, pspec);
}

static void
animation_clutter_affine_effect_set_property (GObject      *object,
                                              guint        prop_id,
                                              const GValue *value,
                                              GParamSpec   *pspec)
{
  AnimationClutterAffineEffect *affine_effect = ANIMATION_CLUTTER_AFFINE_EFFECT (object);
  AnimationClutterAffineEffectPrivate *priv =
    animation_clutter_affine_effect_get_instance_private (affine_effect);

  switch (prop_id)
    {
    case PROP_TRANSFORM_ANIMATION:
      g_set_object (&priv->transform_animation, g_value_dup_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
animation_clutter_affine_effect_dispose (GObject *object)
{
  AnimationClutterAffineEffect *affine_effect = ANIMATION_CLUTTER_AFFINE_EFFECT (object);
  AnimationClutterAffineEffectPrivate *priv =
    animation_clutter_affine_effect_get_instance_private (affine_effect);

  g_clear_object (&priv->transform_animation);

  G_OBJECT_CLASS (animation_clutter_affine_effect_parent_class)->dispose (object);
}

static void
animation_clutter_affine_effect_finalize (GObject *object)
{
  AnimationClutterAffineEffect *affine_effect = ANIMATION_CLUTTER_AFFINE_EFFECT (object);
  AnimationClutterAffineEffectPrivate *priv =
    animation_clutter_affine_effect_get_instance_private (affine_effect);

  g_clear_handle_id (&priv->timeout_id, g_source_remove);

  G_OBJECT_CLASS (animation_clutter_affine_effect_parent_class)->finalize (object);
}

static void
animation_clutter_affine_effect_init (AnimationClutterAffineEffect *effect)
{
  AnimationClutterAffineEffectPrivate *priv =
    animation_clutter_affine_effect_get_instance_private (effect);

  priv->timeout_id = 0;
}

static void
animation_clutter_affine_effect_class_init (AnimationClutterAffineEffectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  ClutterEffectClass *effect_class = CLUTTER_EFFECT_CLASS (klass);

  object_class->notify = animation_clutter_affine_effect_notify;
  object_class->set_property = animation_clutter_affine_effect_set_property;
  object_class->dispose = animation_clutter_affine_effect_dispose;
  object_class->finalize = animation_clutter_affine_effect_finalize;
  effect_class->modify_paint_volume = animation_clutter_affine_effect_modify_paint_volume;
  effect_class->paint = animation_clutter_affine_effect_paint;

  object_properties[PROP_TRANSFORM_ANIMATION] =
    g_param_spec_object ("transform-animation",
                         "Transform Animation",
                         "The underlying transform animation",
                         ANIMATION_TYPE_TRANSFORM_ANIMATION,
                         G_PARAM_WRITABLE);

  g_object_class_install_properties (object_class, PROP_LAST, object_properties);
}

ClutterEffect *
animation_clutter_affine_effect_new (AnimationTransformAnimation *transform_animation)
{
  return g_object_new (ANIMATION_CLUTTER_TYPE_AFFINE_EFFECT,
                       "transform-animation", transform_animation,
                       NULL);
}
