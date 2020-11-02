/*
 * animation-glib/stepper/stepper-holder.cpp
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
 * GObject base class for steppers. Copies the stepper
 * passed to it in its construct parameters.
 */

#include <animation/stepper/stepper.h>
#include <animation/stepper/linear.h>

#include <animation-glib/stepper/stepper.h>
#include <animation-glib/stepper/stepper-holder.h>

namespace as = animation::stepper;

struct _AnimationStepperHolder
{
  GObject parent_instance;
};

typedef struct _AnimationStepperHolderPrivate
{
  as::Stepper::Shared stepper;
} AnimationStepperHolderPrivate;

static void animation_stepper_iface_init (AnimationStepperInterface *stepper_iface);

G_DEFINE_TYPE_WITH_CODE (AnimationStepperHolder,
                         animation_stepper_holder,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ANIMATION_TYPE_STEPPER, animation_stepper_iface_init)
                         G_ADD_PRIVATE (AnimationStepperHolder))

static float
animation_stepper_holder_step (AnimationStepper *stepper,
                               unsigned int      ms)
{
  AnimationStepperHolderPrivate *priv =
    reinterpret_cast <AnimationStepperHolderPrivate *> (animation_stepper_holder_get_instance_private (ANIMATION_STEPPER_HOLDER (stepper)));

  return (*priv->stepper) (ms);
}

as::Stepper::Shared
animation_stepper_holder_get_ptr (AnimationStepperHolder *holder)
{
  AnimationStepperHolderPrivate *priv =
    reinterpret_cast <AnimationStepperHolderPrivate *> (animation_stepper_holder_get_instance_private (holder));

  return priv->stepper;
}

void
animation_stepper_holder_set_ptr (AnimationStepperHolder       *holder,
                                  as::Stepper::Shared    const &ptr)
{
  AnimationStepperHolderPrivate *priv =
    reinterpret_cast <AnimationStepperHolderPrivate *> (animation_stepper_holder_get_instance_private (holder));

  priv->stepper = ptr;
}

static void
animation_stepper_holder_finalize (GObject *object)
{
  AnimationStepperHolder *holder = ANIMATION_STEPPER_HOLDER (object);
  AnimationStepperHolderPrivate *priv =
    reinterpret_cast <AnimationStepperHolderPrivate *> (animation_stepper_holder_get_instance_private (holder));

  using Ptr = as::Stepper::Shared;
  priv->stepper.~Ptr ();

  G_OBJECT_CLASS (animation_stepper_holder_parent_class)->finalize (object);
}

static void
animation_stepper_holder_init (AnimationStepperHolder *holder)
{
  AnimationStepperHolderPrivate *priv =
    reinterpret_cast <AnimationStepperHolderPrivate *> (animation_stepper_holder_get_instance_private (holder));

  new (&priv->stepper) std::shared_ptr <as::Stepper> ();
}

static void
animation_stepper_iface_init (AnimationStepperInterface *interface)
{
  interface->step = animation_stepper_holder_step;
}

static void
animation_stepper_holder_class_init (AnimationStepperHolderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = animation_stepper_holder_finalize;
}
