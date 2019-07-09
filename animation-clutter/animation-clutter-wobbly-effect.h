/*
 * animation-clutter/animation-clutter-wobbly-effect.h
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
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define ANIMATION_CLUTTER_TYPE_WOBBLY animation_clutter_wobbly_effect_get_type ()
G_DECLARE_FINAL_TYPE (AnimationClutterWobblyEffect, animation_clutter_wobbly_effect, ANIMATION_CLUTTER, WOBBLY_EFFECT, ClutterDeformEffect)

/**
 * animation_clutter_wobbly_effect_grab:
 * @effect: An #AnimationClutterWobblyEffect
 * @x: The x-coordinate on the mesh to grab, specified relative to the
 * upper-left corner of the mesh
 * @y: The y-coordinate on the mesh to grab, specified relative to the
 * upper-left corner of the mesh.
 *
 * Grabs the anchor specified by @x and @y on the mesh. While
 * the mesh is in this state, this point will move immediately,
 * causing spring forces to be applied to other points on the mesh
 *
 * It is a precondition violation to call this function when the mesh is
 * already grabbed.
 *
 */
void animation_clutter_wobbly_effect_grab (AnimationClutterWobblyEffect *effect,
                                           double         x,
                                           double         y);

/**
 * animation_clutter_wobbly_effect_ungrab:
 * @effect: An #AnimationClutterWobblyEffect
 * Removes the current grab. When the actor is moved, the mesh will
 * move uniformly.
 *
 * It is a precondition violation to call this function when the mesh is
 * not grabbed.
 */
void animation_clutter_wobbly_effect_ungrab (AnimationClutterWobblyEffect *effect);

/**
 * animation_clutter_wobbly_effect_move_by:
 * @effect: An #AnimationClutterWobblyEffect
 * @dx: A delta-x coordinate to move the mesh by
 * @dy: A delta-y coordinate to move the mesh by
 *
 * Moves the mesh by @dx and @dy
 *
 * If the mesh is grabbed, then spring forces will be applied causing
 * some points on the mesh to move more slowly than others. The nature
 * of the moment will depend on the window's maximization state.
 *
 */
void animation_clutter_wobbly_effect_move_by (AnimationClutterWobblyEffect *effect,
                                              double         dx,
                                              double         dy);

/**
 * animation_clutter_wobbly_effect_new:
 *
 * Creates a new #ClutterEffect which makes the window "wobble"
 * on a spring mesh for the actor
 *
 * Returns: (transfer full): A new #ClutterEffect
 */
ClutterEffect * animation_clutter_wobbly_effect_new (void);

G_END_DECLS
