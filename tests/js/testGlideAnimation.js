/*
 * /tests/js/testGlideAnimation.js
 *
 * Tests for the JavaScript Binding to the Glide Animation.
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

const { Animation } = imports.gi;
const { BoxWrapper, applyCallerTranslation, multiplyMatrixVector } = imports.common;

describe('Animation', function() {
  describe('GlideAnimation', function() {
    let animation;

    beforeEach(function() {
      animation = new Animation.GlideAnimation({
        initial_distance: 0.5,
        x_rotation_angle_degrees: 0.0,
        y_rotation_angle_degrees: 20.0,
        x_axis_location_unit: 0.5,
        y_axis_location_unit: 0.0,
        viewport: new BoxWrapper({
          box: new Animation.Box({
            top_left: new Animation.Vector({ x: 0, y: 0 }),
            bottom_right: new Animation.Vector({ x: 1000, y: 100 })
          })
        }),
        target: new BoxWrapper({
          box: new Animation.Box({
            top_left: new Animation.Vector({
              x: 0,
              y: 0,
            }),
            bottom_right: new Animation.Vector({
              x: 100,
              y: 100,
            }),
          }),
        }),
        stepper: new Animation.LinearStepper({
          length: 200,
        })
      });
    });

    it('has expected properties set on construction', function() {
      expect(animation.initial_distance).toEqual(0.5);
      expect(animation.x_rotation_angle_degrees).toEqual(0.0);
      expect(animation.y_rotation_angle_degrees).toEqual(20.0);
      expect(animation.x_axis_location_unit).toEqual(0.5);
      expect(animation.y_axis_location_unit).toEqual(0.0);
      expect(animation.stepper.length).toEqual(200);
    });

    it('animates to correct position after length timesteps', function() {
      expect(animation.step(200)).toBeFalsy();

      // Note that the animation is applied taking the translation the caller
      // would have already applied into account. Since the size does not change,
      // at the end of the animation we are in the same position as the caller
      // would be.
      let target = [100, 100, 0, 1]
      multiplyMatrixVector(animation.matrix(), [100, 100, 0, 1]).forEach(function(x, i) {
        expect(x).toBeCloseTo(target[i]);
      });
    });

    it('starts in the correct position', function() {
      expect(animation.step(200)).toBeFalsy();

      // Note that the animation is applied taking the translation the caller
      // would have already applied into account. Since the size does not change,
      // at the end of the animation we are in the same position as the caller
      // would be.
      expect(multiplyMatrixVector(animation.matrix(), [100, 100, 0, 1])).not.toEqual([100, 100, 0, 1]);
    });
  });
});
