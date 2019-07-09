/*
 * animation-glib/stepper/linear.cpp
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
 * GObject wrapper for linear stepper.
 */

#include <memory>

#include <animation/stepper/linear.h>
#include <animation/stepper/stepper.h>

#include <animation-glib/constructor-helpers.h>
#include <animation-glib/stepper/linear.h>
#include <animation-glib/stepper/stepper-holder.h>
#include <animation-glib/stepper/stepper-holder-internal.h>

namespace as = animation::stepper;

struct _AnimationLinearStepper
{
  GObject parent_instance;
};

G_DEFINE_TYPE (AnimationLinearStepper,
               animation_linear_stepper,
               ANIMATION_TYPE_STEPPER_HOLDER)

enum {
  PROP_0,
  PROP_LENGTH,
  NPROPS
};

static GParamSpec *animation_linear_stepper_properties[NPROPS];

static void
animation_linear_stepper_set_property (GObject      *object,
                                       guint         prop_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
  AnimationStepperHolder *holder = ANIMATION_STEPPER_HOLDER (object);

  switch (prop_id)
    {
    case PROP_LENGTH:
      std::static_pointer_cast <as::LinearStepper> (animation_stepper_holder_get_ptr (holder))->Length (g_value_get_uint (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
animation_linear_stepper_get_property (GObject    *object,
                                       guint       prop_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
  AnimationStepperHolder *holder = ANIMATION_STEPPER_HOLDER (object);

  switch (prop_id)
    {
    case PROP_LENGTH:
      g_value_set_uint (value,
                        std::static_pointer_cast <as::LinearStepper> (animation_stepper_holder_get_ptr (holder))->Length ());
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
animation_linear_stepper_init (AnimationLinearStepper *stepper)
{
  static const unsigned int DefaultAnimationLength = 300;
  AnimationStepperHolder *holder = ANIMATION_STEPPER_HOLDER (stepper);

  animation_stepper_holder_set_ptr (holder, std::make_shared <as::LinearStepper> (DefaultAnimationLength));
}

static void
animation_linear_stepper_class_init (AnimationLinearStepperClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = animation_linear_stepper_get_property;
  object_class->set_property = animation_linear_stepper_set_property;

  animation_linear_stepper_properties[PROP_LENGTH] =
    g_param_spec_uint ("length",
                       "Length",
                       "How long the animation lasts",
                       1,
                       5000,
                       300,
                       static_cast <GParamFlags> (G_PARAM_READWRITE));

  g_object_class_install_properties (object_class,
                                     NPROPS,
                                     animation_linear_stepper_properties);
}

/**
 * animation_linear_stepper_new:
 * @length: Length of the transition in milliseconds.
 *
 * Return a new #AnimationStepper which linearly increments progress
 * every time the step() method is called on it.
 *
 * Returns: (transfer full): An #AnimationLinearStepper
 *                           implementation of #AnimationStepper
 */
AnimationStepper *
animation_linear_stepper_new (unsigned int length)
{
  return ANIMATION_STEPPER (g_object_new (ANIMATION_TYPE_LINEAR_STEPPER,
                                          "length", length,
                                          NULL));
}
