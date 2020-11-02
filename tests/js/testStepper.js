/*
 * /tests/js/testStepper.js
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
 * Tests for the JavaScript Binding to the Stepper.
 */

const { Animation } = imports.gi;
const { BoxWrapper, applyCallerTranslation, multiplyMatrixVector } = imports.common;

describe('Animation', function() {
  describe('LinearStepper', function() {
    let stepper;

    beforeEach(function() {
      stepper = new Animation.LinearStepper({
        length: 200
      });
    });

    it('has properties that were set on construction', function() {
      expect(stepper.length).toEqual(200);
    });

    it('stops at 1.0 on the last step', function() {
      expect(stepper.step(200)).toEqual(1.0);
    });

    it('behaves linearly', function() {
      expect(stepper.step(100)).toEqual(0.5);
    });
  });
});
