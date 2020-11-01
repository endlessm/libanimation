/*
 * animation-clutter/animation-clutter-wobbly-effect.c
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
#include <clutter/clutter.h>

#include <animation-glib/vector.h>
#include <animation-glib/wobbly/anchor.h>
#include <animation-glib/wobbly/model.h>

#include "animation-clutter-common.h"
#include "animation-clutter-common-private.h"
#include "animation-clutter-wobbly-effect.h"

struct _AnimationClutterWobblyEffect {
  ClutterDeformEffect parent_instance;
};

typedef struct _AnimationClutterWobblyEffectPrivate
{
  float                  slowdown_factor;
  double                 spring_constant;
  double                 friction;
  double                 movement_range;
  AnimationWobblyModel  *model;
  AnimationWobblyAnchor *anchor;
  gint64                 last_msecs;
  guint                  timeout_id;
  guint                  width_changed_signal;
  guint                  height_changed_signal;
  gboolean               ungrab_pending;
} AnimationClutterWobblyEffectPrivate;

enum
{
  PROP_0,

  PROP_SPRING_K,
  PROP_FRICTION,
  PROP_SLOWDOWN_FACTOR,
  PROP_OBJECT_MOVEMENT_RANGE,

  PROP_LAST
};

static GParamSpec *object_properties[PROP_LAST];

G_DEFINE_TYPE_WITH_PRIVATE (AnimationClutterWobblyEffect,
                            animation_clutter_wobbly_effect,
                            CLUTTER_TYPE_DEFORM_EFFECT)

static gboolean
animation_clutter_wobbly_effect_modify_paint_volume (ClutterEffect    *effect,
                                                     ClutterPaintVolume *volume)
{
  ClutterActorMeta *meta = CLUTTER_ACTOR_META (effect);
  ClutterActor *actor = clutter_actor_meta_get_actor (meta);
  AnimationClutterWobblyEffect *wobbly_effect = ANIMATION_CLUTTER_WOBBLY_EFFECT (effect);
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (wobbly_effect);

  /* We assume that the parent's modify_paint_volume method always returns
   * TRUE here. */
  CLUTTER_EFFECT_CLASS (animation_clutter_wobbly_effect_parent_class)->modify_paint_volume (effect, volume);

  if (priv->model && clutter_actor_meta_get_enabled (meta))
    {
      ClutterActorBox box;
      float actor_x, actor_y;

      animation_clutter_get_untransformed_paint_box_from_existing_volume (actor, volume, &box);
      clutter_actor_get_position (actor, &actor_x, &actor_y);

      AnimationVector offset = { box.x1 - actor_x, box.y1 - actor_y };
      AnimationVector extremes[4];

      animation_wobbly_model_query_extremes (priv->model,
                                   &extremes[0],
                                   &extremes[1],
                                   &extremes[2],
                                   &extremes[3]);

      float x1 = MIN (extremes[0].x, extremes[2].x);
      float y1 = MIN (extremes[0].y, extremes[1].y);
      float x2 = MAX (extremes[1].x, extremes[3].x);
      float y2 = MAX (extremes[2].y, extremes[3].y);

      ClutterActorBox const extremesBox =
        {
          floor (x1 + offset.x),
          floor (y1 + offset.y),
          ceil (x2 + offset.x),
          ceil (y2 + offset.x)
        };

      clutter_paint_volume_union_box (volume, &extremesBox);
    }

  return TRUE;
}

static void
animation_clutter_wobbly_effect_deform_vertex (ClutterDeformEffect *effect,
                                               gfloat               x G_GNUC_UNUSED,
                                               gfloat               y G_GNUC_UNUSED,
                                               CoglTextureVertex   *vertex)
{
  AnimationClutterWobblyEffect *wobbly_effect = ANIMATION_CLUTTER_WOBBLY_EFFECT (effect);
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (wobbly_effect);

  /* The reversal of ty and tx here is intentional */
  AnimationVector uv = { vertex->ty, vertex->tx };
  AnimationVector deformed;
  animation_wobbly_model_deform_texcoords (priv->model,
                                           uv,
                                           &deformed);

  vertex->x = deformed.x;
  vertex->y = deformed.y;
}

static void
remove_anchor_if_pending (AnimationClutterWobblyEffectPrivate *priv)
{
  if (priv->ungrab_pending)
    {
      g_clear_object (&priv->anchor);
      priv->ungrab_pending = FALSE;
    }
}

/* It turns out that clutter doesn't contain any mechanism whatsoever
 * to do timeline-less animations. We're just using a timeout here
 * to keep performing animations on the actor */
static gboolean
animation_clutter_wobbly_effect_new_frame (gpointer user_data)
{
  AnimationClutterWobblyEffect *wobbly_effect = ANIMATION_CLUTTER_WOBBLY_EFFECT (user_data);
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (wobbly_effect);
  gint64 msecs = g_get_monotonic_time ();

  static const unsigned int ms_to_us = 1000;

  g_assert (priv->model);

  /* Wraparound, priv->last_msecs -= G_MAXINT64.
   * We make priv->last_msecs negative so that subtracting it
   * from msecs results in the correct delta */
  if (G_UNLIKELY (priv->last_msecs > msecs))
    priv->last_msecs -= G_MAXINT64;

  gint64 msecs_delta = (msecs - priv->last_msecs ) / ms_to_us;
  priv->last_msecs = msecs;

  /* If there was no time movement, then we can't really step or remove
   * models in a way that makes sense, so don't do it */
  if (msecs_delta)
    {
      if (animation_wobbly_model_step (priv->model, msecs_delta / priv->slowdown_factor))
        {
          clutter_actor_meta_set_enabled (CLUTTER_ACTOR_META (wobbly_effect), TRUE);
          clutter_deform_effect_invalidate (CLUTTER_DEFORM_EFFECT (wobbly_effect));
        }
      else
        {
          remove_anchor_if_pending (priv);

          /* Also disable the effect */
          clutter_actor_meta_set_enabled (CLUTTER_ACTOR_META (wobbly_effect), FALSE);

          /* Finally, return false so that we don't keep animating */
          priv->timeout_id = -1;
          return FALSE;
        }
    }

  /* We always want to return true even if there was no time delta */
  return TRUE;
}

static void
animation_clutter_wobbly_effect_ensure_timeline (AnimationClutterWobblyEffect *wobbly_effect)
{
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (wobbly_effect);

  if (priv->timeout_id == -1)
    {
      static const unsigned int frame_length_ms = 16; // 60 / 1000;

      priv->last_msecs = g_get_monotonic_time ();
      priv->timeout_id = g_timeout_add (frame_length_ms, animation_clutter_wobbly_effect_new_frame, wobbly_effect);
    }
}

void
animation_clutter_wobbly_effect_grab (AnimationClutterWobblyEffect *effect,
                                      double                        x,
                                      double                        y)
{
  ClutterActor *actor = clutter_actor_meta_get_actor (CLUTTER_ACTOR_META (effect));
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (effect);

  g_assert (!priv->anchor || priv->ungrab_pending);

  /* Either ungrab here or at the end of the animation */
  remove_anchor_if_pending (priv);

  if (priv->model)
    {
      /* Make sure to update the model geometry and move
       * to the right position, it may have changed
       * in the meantime */
      ClutterActorBox box;
      float actor_paint_box_width, actor_paint_box_height;
      animation_clutter_get_best_known_paint_extents_box (actor, &box);

      /* See animation-clutter-common.c for why we need to do this */
      animation_clutter_actor_box_enlarge_for_effects (&box);

      actor_paint_box_width = box.x2 - box.x1;
      actor_paint_box_height = box.y2 - box.y1;

      AnimationVector position = { 0, 0 };
      AnimationVector size = { actor_paint_box_width, actor_paint_box_height };

      animation_wobbly_model_resize (priv->model, size);
      animation_wobbly_model_move_to (priv->model, position);

      animation_clutter_wobbly_effect_ensure_timeline (effect);

      float actor_x, actor_y;
      clutter_actor_get_position (actor, &actor_x, &actor_y);

      AnimationVector anchor_position = { x - actor_x, y - actor_y };

      priv->anchor = animation_wobbly_model_grab_anchor (priv->model, anchor_position);
    }
}

void
animation_clutter_wobbly_effect_ungrab (AnimationClutterWobblyEffect *effect)
{
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (effect);

  g_assert (priv->anchor && !priv->ungrab_pending);

  /* Don't immediately ungrab. We can be a little bit more
   * clever here and make the ungrab pending on the completion
   * of the animation */
  if (priv->timeout_id != -1)
    priv->ungrab_pending = TRUE;
  else
    g_clear_object (&priv->anchor);
}

void
animation_clutter_wobbly_effect_move_by (AnimationClutterWobblyEffect *effect,
                                         double                        dx,
                                         double                        dy)
{
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (effect);

  if (priv->anchor)
    {
      AnimationVector delta = { dx, dy };

      animation_clutter_wobbly_effect_ensure_timeline (effect);
      animation_wobbly_anchor_move_by (priv->anchor, delta);

      AnimationVector reverse_delta = delta;
      reverse_delta.x *= -1;
      reverse_delta.y *= -1;

      /* Now move the entire model back - this ensures that
       * we stay in sync with the actor's relative position */
      animation_wobbly_model_move_by (priv->model, reverse_delta);
    }
}

static void
animation_clutter_wobbly_effect_size_changed (GObject    *object,
                                              GParamSpec *spec G_GNUC_UNUSED,
                                              gpointer    user_data)
{
  ClutterActor    *actor = CLUTTER_ACTOR (object);
  AnimationClutterWobblyEffect *effect = ANIMATION_CLUTTER_WOBBLY_EFFECT (user_data);
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (effect);

  /* We don't ensure a timeline here because we only want to redistribute
   * non-anchor points if we're already grabbed, which the wobbly effect will
   * do internally anyways */
  if (priv->model)
    {
      ClutterActorBox box;
      float actor_paint_box_width, actor_paint_box_height;
      animation_clutter_get_best_known_paint_extents_box (actor, &box);

      /* See animation-clutter-common.c for why we need to do this */
      animation_clutter_actor_box_enlarge_for_effects (&box);

      actor_paint_box_width = box.x2 - box.x1;
      actor_paint_box_height = box.y2 - box.y1;

      /* If we have any pending anchors, we should release them now -
       * the model move and resize code explicitly does not move
       * anchors around (because that'd put them out of sync with
       * the cursor) */
      remove_anchor_if_pending (priv);

      AnimationVector actor_size = { actor_paint_box_width, actor_paint_box_height };
      AnimationVector actor_position = { 0.0, 0.0 };

      g_message ("Actor paint box size %f %f %f %f\n", box.x1, box.y1, actor_paint_box_width, actor_paint_box_height);
      animation_wobbly_model_resize (priv->model, actor_size);
      animation_wobbly_model_move_to (priv->model, actor_position);
    }
}

static void
animation_clutter_wobbly_effect_set_actor (ClutterActorMeta *actor_meta,
                                           ClutterActor     *actor)
{
  ClutterActor *prev_actor = clutter_actor_meta_get_actor (actor_meta);

  CLUTTER_ACTOR_META_CLASS (animation_clutter_wobbly_effect_parent_class)->set_actor (actor_meta, actor);

  AnimationClutterWobblyEffect *wobbly_effect = ANIMATION_CLUTTER_WOBBLY_EFFECT (actor_meta);
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (wobbly_effect);

  g_clear_object (&priv->anchor);
  g_clear_object (&priv->model);

  priv->ungrab_pending = FALSE;

  if (priv->timeout_id != -1)
    {
      g_source_remove (priv->timeout_id);
      priv->timeout_id = -1;
    }

  if (prev_actor)
    {
      g_signal_handler_disconnect (prev_actor, priv->width_changed_signal);
      priv->width_changed_signal = 0;

      g_signal_handler_disconnect (prev_actor, priv->height_changed_signal);
      priv->height_changed_signal = 0;
    }

  if (actor)
    {
      ClutterActorBox box;
      float actor_paint_box_width, actor_paint_box_height;
      animation_clutter_get_best_known_paint_extents_box (actor, &box);

      actor_paint_box_width = box.x2 - box.x1;
      actor_paint_box_height = box.y2 - box.y1;

      AnimationVector actor_position = { 0, 0 };
      AnimationVector actor_size = { actor_paint_box_width, actor_paint_box_height };

      priv->model = animation_wobbly_model_new (actor_position,
                                                actor_size,
                                                priv->spring_constant,
                                                priv->friction,
                                                priv->movement_range);

      priv->width_changed_signal =
        g_signal_connect_object (actor,
                                 "notify::width",
                                 G_CALLBACK (animation_clutter_wobbly_effect_size_changed),
                                 wobbly_effect,
                                 G_CONNECT_AFTER);
      priv->height_changed_signal =
        g_signal_connect_object (actor,
                                 "notify::height",
                                 G_CALLBACK (animation_clutter_wobbly_effect_size_changed),
                                 wobbly_effect,
                                 G_CONNECT_AFTER);
    }

  /* Whatever the actor, ensure that the effect is disabled at this point */
  clutter_actor_meta_set_enabled (actor_meta, FALSE);
}

static void
animation_clutter_wobbly_effect_set_property (GObject      *object,
                                              guint         prop_id,
                                              const GValue *value,
                                              GParamSpec   *pspec)
{
  AnimationClutterWobblyEffect *wobbly_effect = ANIMATION_CLUTTER_WOBBLY_EFFECT (object);
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (wobbly_effect);

  switch (prop_id)
    {
    case PROP_SPRING_K:
      priv->spring_constant = g_value_get_double (value);

      if (priv->model != NULL)
        animation_wobbly_model_set_spring_k (priv->model, priv->spring_constant);
      break;
    case PROP_FRICTION:
      priv->friction = g_value_get_double (value);

      if (priv->model != NULL)
        animation_wobbly_model_set_friction (priv->model, priv->friction);
      break;
    case PROP_SLOWDOWN_FACTOR:
      priv->slowdown_factor = g_value_get_double (value);
      break;
    case PROP_OBJECT_MOVEMENT_RANGE:
      priv->movement_range = g_value_get_double (value);

      if (priv->model != NULL)
        animation_wobbly_model_set_maximum_range (priv->model, priv->movement_range);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
animation_clutter_wobbly_effect_finalize (GObject *object)
{
  AnimationClutterWobblyEffect *wobbly_effect = ANIMATION_CLUTTER_WOBBLY_EFFECT (object);
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (wobbly_effect);

  g_clear_object (&priv->model);

  if (priv->timeout_id != -1)
    {
      g_source_remove (priv->timeout_id);
      priv->timeout_id = -1;
    }

  G_OBJECT_CLASS (animation_clutter_wobbly_effect_parent_class)->finalize (object);
}

static void
animation_clutter_wobbly_effect_init (AnimationClutterWobblyEffect *effect)
{
  AnimationClutterWobblyEffectPrivate *priv =
    animation_clutter_wobbly_effect_get_instance_private (effect);

  priv->timeout_id = -1;
}

static void
animation_clutter_wobbly_effect_class_init (AnimationClutterWobblyEffectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  ClutterActorMetaClass *meta_class = CLUTTER_ACTOR_META_CLASS (klass);
  ClutterEffectClass *effect_class = CLUTTER_EFFECT_CLASS (klass);
  ClutterDeformEffectClass *deform_class = CLUTTER_DEFORM_EFFECT_CLASS (klass);

  object_class->set_property = animation_clutter_wobbly_effect_set_property;
  object_class->finalize = animation_clutter_wobbly_effect_finalize;
  meta_class->set_actor = animation_clutter_wobbly_effect_set_actor;
  effect_class->modify_paint_volume = animation_clutter_wobbly_effect_modify_paint_volume;
  deform_class->deform_vertex = animation_clutter_wobbly_effect_deform_vertex;

  object_properties[PROP_SPRING_K] =
    g_param_spec_double ("spring-k",
                         "Spring Constant",
                         "How springy the model is",
                         2.0, 10.0, 8.0,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);

  object_properties[PROP_FRICTION] =
    g_param_spec_double ("friction",
                         "Friction Constant",
                         "How much friction force should be applied to moving objects",
                         2.0, 10.0, 3.0,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);

  object_properties[PROP_SLOWDOWN_FACTOR] =
    g_param_spec_double ("slowdown-factor",
                         "Slowdown Factor",
                         "How much to slow the model's timesteps down",
                         1.0, 5.0, 1.0,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);

  object_properties[PROP_OBJECT_MOVEMENT_RANGE] =
    g_param_spec_double ("object-movement-range",
                         "Object Movement Range",
                         "How much objects are allowed to move around",
                         10.0, 500.0, 100.0,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT);

  g_object_class_install_properties (object_class, PROP_LAST, object_properties);
}

ClutterEffect *
animation_clutter_wobbly_effect_new (void)
{
  return g_object_new (ANIMATION_CLUTTER_TYPE_WOBBLY, NULL);
}
