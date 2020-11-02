/*
 * animation-clutter/animation-clutter-common.h
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

#include <animation-glib/vector.h>
#include <animation-glib/vector4d.h>
#include <clutter/clutter.h>
#include <glib.h>

G_BEGIN_DECLS

void animation_clutter_get_untransformed_paint_box_from_existing_volume (ClutterActor             *actor,
                                                                         const ClutterPaintVolume *volume,
                                                                         ClutterActorBox          *box);

void animation_clutter_get_best_known_paint_extents_box (ClutterActor    *actor,
                                                         ClutterActorBox *box);

void animation_clutter_compute_corners_from_untransformed_paint_volume (ClutterActor       *actor,
                                                                        ClutterPaintVolume *volume,
                                                                        AnimationVector    *out_corners,
                                                                        AnimationVector    *out_offset);

void animation_clutter_expand_paint_volume_with_extremes (ClutterPaintVolume *volume,
                                                          AnimationVector4D  *extremes,
                                                          AnimationVector    *offset);

G_END_DECLS
