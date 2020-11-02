/*
 * /tests/js/testBounceAnimation.js
 *
 * Copyright (C) 2019 Sam Spilsbury <smspillaz@gmail.com>.
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
 *
 * Tests for the JavaScript Binding to the Bounce Animation.
 */

const { Animation } = imports.gi;
const { BoxWrapper, applyCallerTranslation, multiplyMatrixVector } = imports.common;

describe('Animation', function() {
  describe('BounceAnimation', function() {
    let animation;

    beforeEach(function() {
      animation = new Animation.BounceAnimation({
        initial_scale: 0.7,
        maximum_scale: 1.5,
        n_bounce: 1,
        target: new BoxWrapper({
          box: new Animation.Box({
            top_left: new Animation.Vector({
              x: 0,
              y: 0,
            }),
            bottom_right: new Animation.Vector({
              x: 100,
              y: 100,
            })
          }),
        }),
        stepper: new Animation.LinearStepper({
          length: 200
        }),
      });
    });

    it('has properties that were set on construction', function() {
      expect(animation.initial_scale).toBeCloseTo(0.7);
      expect(animation.maximum_scale).toBeCloseTo(1.5);
      expect(animation.n_bounce).toEqual(1);
      expect(animation.stepper.length).toEqual(200);
    });

    it('stops at the target box', function() {
      animation.step(200);

      expect(multiplyMatrixVector(animation.matrix(), [0, 0, 0, 1])).toEqual([0, 0, 0, 1]);
    });

    it('starts with the target scale', function() {
      expect(multiplyMatrixVector(animation.matrix(), [0, 0, 0, 1])).toEqual([15, 15, 0, 1]);
    });
  });
});
