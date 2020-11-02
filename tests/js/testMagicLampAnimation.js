/*
 * /tests/js/testMagicLampAnimation.js
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
const { BoxWrapper } = imports.common;

describe('Animation', function() {
  describe('MagicLampAnimation', function() {
    let animation;
    let bendFactor = 10;
    let offsetFactor = 0.5;
    let stretchFactor = 0.45;
    let deformationSpeedFactor = 2.3;
    let resolution = new Animation.Vector({ x: 4, y: 4 });
    let source = new BoxWrapper({
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
    });
    let target = new BoxWrapper({
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
    });

    beforeEach(function() {
      animation = new Animation.MagicLampAnimation({
        source: source,
        target: target,
        resolution: resolution,
        bend_factor: bendFactor,
        offset_factor: offsetFactor,
        stretch_factor: stretchFactor,
        deform_speed_factor: deformationSpeedFactor,
        stepper: new Animation.LinearStepper({
          length: 200,
        }),
      });
    });

    it('has expected properties set on construction', function() {
      expect(animation.resolution.x).toEqual(4);
      expect(animation.resolution.y).toEqual(4);
      expect(animation.bend_factor).toBeCloseTo(bendFactor);
      expect(animation.stretch_factor).toBeCloseTo(stretchFactor);
      expect(animation.deform_speed_factor).toBeCloseTo(deformationSpeedFactor);
      expect(animation.stepper.length).toEqual(200);
    });

    it('animates to correct position after length timesteps', function() {
      expect(animation.step(200)).toBeFalsy();

      let corners = [
        new Animation.Vector({ x: 0, y: 0 }),
        new Animation.Vector({ x: 100, y: 0 }),
        new Animation.Vector({ x: 0, y: 100 }),
        new Animation.Vector({ x: 100, y: 100 }),
      ];
      let extremes = animation.extremes (corners);

      expect(extremes[0].x).toBeCloseTo(0);
      expect(extremes[0].y).toBeCloseTo(0);

      expect(extremes[1].x).toBeCloseTo(100);
      expect(extremes[1].y).toBeCloseTo(0);

      expect(extremes[2].x).toBeCloseTo(0);
      expect(extremes[2].y).toBeCloseTo(100);

      expect(extremes[3].x).toBeCloseTo(100);
      expect(extremes[3].y).toBeCloseTo(100);
    });
  });
});
