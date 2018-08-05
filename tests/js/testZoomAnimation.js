/*
 * /tests/js/testZoomAnimation.js
 *
 * Tests for the JavaScript Binding to the Zoom Animation.
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
  describe('ZoomAnimation', function() {
    let animation;

    beforeEach(function() {
      animation = new Animation.ZoomAnimation({
        source: new BoxWrapper({
          box: new Animation.Box({
            top_left: new Animation.Vector({
              x: 100,
              y: 100,
            }),
            bottom_right: new Animation.Vector({
              x: 200,
              y: 200,
            }),
          }),
        }),
        target: new BoxWrapper({
          box: new Animation.Box({
            top_left: new Animation.Vector({
              x: 200,
              y: 200,
            }),
            bottom_right: new Animation.Vector({
              x: 300,
              y: 300,
            }),
          }),
        }),
        stepper: new Animation.LinearStepper({
          length: 10,
        })
      });
    });

    it('has expected properties set on construction', function() {
      expect(animation.stepper.length).toEqual(10);
    });

    it('animates to correct position after length timesteps', function() {
      expect(animation.step(10)).toBeFalsy();

      // Note that the animation is applied taking the translation the caller
      // would have already applied into account. Since the size does not change,
      // at the end of the animation we are in the same position as the caller
      // would be.
      expect(multiplyMatrixVector(animation.matrix(), [200, 200, 0, 1])).toEqual([200, 200, 0, 1]);
    });

    describe('when scaling', function() {
      beforeEach(function() {
        animation = new Animation.ZoomAnimation({
          source: new BoxWrapper({
            box: new Animation.Box({
              top_left: new Animation.Vector({
                x: 100,
                y: 100,
              }),
              bottom_right: new Animation.Vector({
                x: 150,
                y: 150,
              }),
            }),
          }),
          target: new BoxWrapper({
            box: new Animation.Box({
              top_left: new Animation.Vector({
                x: 200,
                y: 200,
              }),
              bottom_right: new Animation.Vector({
                x: 300,
                y: 300,
              }),
            }),
          }),
          stepper: new Animation.LinearStepper({
            length: 10,
          })
        });
      });

      it('animates and scales to correct position after length timesteps', function() {
        expect(animation.step(10)).toBeFalsy();

        // Take into account caller translation and size (we translated to (200, 200), (250, 250)),
        // meaning that and the end of the animation, nothing changes.
        expect(multiplyMatrixVector(animation.matrix(), [250, 250, 0, 1])).toEqual([250, 250, 0, 1]);
      });

      it('is scaled down at the beginning of the animation', function() {
        // We need to scale down at the beginning of the animation, given that the
        // caller size is already 100x100
        expect(applyCallerTranslation(multiplyMatrixVector(animation.matrix(), [0, 0, 0, 1]), 200, 200)).toEqual([100, 100, 0, 1]);
        expect(applyCallerTranslation(multiplyMatrixVector(animation.matrix(), [100, 100, 0, 1]), 200, 200)).toEqual([150, 150, 0, 1]);
      });
    });

    it('starts in the correct position', function() {
      // Take into account caller translation, we need to apply a negative offset
      expect(multiplyMatrixVector(animation.matrix(), [200, 200, 0, 1])).toEqual([100, 100, 0, 1]);
    });
  });
});
