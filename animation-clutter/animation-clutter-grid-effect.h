/*
 * animation-clutter/animation-clutter-grid-effect.h
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

#include <glib-object.h>
#include <animation-glib/grid/grid.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define ANIMATION_CLUTTER_TYPE_GRID_EFFECT (animation_clutter_grid_effect_get_type ())
G_DECLARE_FINAL_TYPE (AnimationClutterGridEffect, animation_clutter_grid_effect, ANIMATION_CLUTTER, GRID_EFFECT, ClutterDeformEffect)


/**
 * animation_clutter_grid_effect_new:
 * @grid_animation: An #AnimationGridAnimation to wrap.
 *
 * Creates a new #ClutterEffect which uses the underlying
 * AnimationGridAnimation to apply grid based effects to the actor.
 *
 * Returns: (transfer full): A new #ClutterEffect
 */
ClutterEffect * animation_clutter_grid_effect_new (AnimationGridAnimation *grid_animation);

G_END_DECLS
