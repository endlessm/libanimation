/*
 * /tests/js/common.js
 *
 * Helper functions for javascript tests for animations.
 *
 * Copyright (C) 2019 Sam Spilsbury.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

const { Animation, GLib, GObject } = imports.gi;

var BoxWrapper = GObject.registerClass({
    Properties: {
        'box': GObject.ParamSpec.boxed('box',
                                       'AnimationBox',
                                       'AnimationBox to animate to (static)',
                                       GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
                                       Animation.Box)
    }
}, class BoxWrapper extends Animation.BoxQuery {
    _init(props) {
        super._init(props);
        this.update(this.box);
    }
});

function multiplyMatrixVector(mat, vec) {
  return [
    mat[0] * vec[0] + mat[4] * vec[1] + mat[8] * vec[2] + mat[12] * vec[3],
    mat[1] * vec[0] + mat[5] * vec[1] + mat[9] * vec[2] + mat[13] * vec[3],
    mat[2] * vec[0] + mat[6] * vec[1] + mat[10] * vec[2] + mat[14] * vec[3],
    mat[3] * vec[0] + mat[7] * vec[1] + mat[11] * vec[2] + mat[15] * vec[3]
  ]
}

function applyCallerTranslation(vector, x, y) {
  return [vector[0] + x, vector[1] + y, 0, 1];
}

