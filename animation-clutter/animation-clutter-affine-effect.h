/*
 * animation-clutter/clutter-animation-affine-effect.h
 *
 * Copyright © 2013-2016 Endless Mobile, Inc.
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

#pragma once

#include <animation-glib/transform/transform.h>
#include <clutter/clutter.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ANIMATION_CLUTTER_TYPE_AFFINE_EFFECT (animation_clutter_affine_effect_get_type ())
G_DECLARE_FINAL_TYPE (AnimationClutterAffineEffect, animation_clutter_affine_effect, ANIMATION_CLUTTER, AFFINE_EFFECT, ClutterEffect)

/**
 * animation_clutter_affine_effect_new:
 * @transform_animation: An #AnimationTransformAnimation to wrap.
 *
 * Creates a new #ClutterEffect which uses the underlying
 * AnimationZoomAnimation to apply a linear transformation to the actor.
 *
 * Returns: (transfer full): A new #ClutterEffect
 */
ClutterEffect * animation_clutter_affine_effect_new (AnimationTransformAnimation *transform_animation);

G_END_DECLS
