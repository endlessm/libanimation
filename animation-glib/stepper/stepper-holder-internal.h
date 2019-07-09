/*
 * animation-glib/stepper/stepper-holder-internal.h
 *
 * Copyright 2018 Endless Mobile, Inc.
 *
 * libanimation is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * libanimation is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with eos-companion-app-service.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Internally public methods for GObject stepper holder implementation.
 */
#pragma once

#include <memory>

#include <animation/stepper/stepper.h>
#include <animation-glib/stepper/stepper-holder.h>


animation::stepper::Stepper::Shared animation_stepper_holder_get_ptr (AnimationStepperHolder *holder);

void animation_stepper_holder_set_ptr (AnimationStepperHolder                    *holder,
                                       animation::stepper::Stepper::Shared const &ptr);
