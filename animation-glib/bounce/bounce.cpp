/*
 * animation-glib/bounce/bounce.cpp
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
 * GObject implementation for a "bounce" animation.
 */

#include <animation/bounce/bounce.h>

#include <animation-glib/box.h>
#include <animation-glib/bounce/bounce.h>
#include <animation-glib/constructor-helpers.h>
#include <animation-glib/query/geometry-query.h>
#include <animation-glib/query/geometry-query-private.h>
#include <animation-glib/stepper/stepper.h>
#include <animation-glib/stepper/linear.h>
#include <animation-glib/stepper/stepper-internal.h>
#include <animation-glib/vector.h>

namespace agd = animation::geometry::dimension;
namespace agl = animation::glib;
namespace ab = animation::bounce;
namespace ag = animation::glib;
namespace as = animation::stepper;
namespace asg = animation::stepper::glib;
namespace at = animation::transform;

struct _AnimationBounceAnimation
{
  AnimationTransformAnimation parent_instance;
};

typedef struct _AnimationBounceAnimationPrivate
{
} AnimationBounceAnimationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AnimationBounceAnimation,
                            animation_bounce_animation,
                            ANIMATION_TYPE_TRANSFORM_ANIMATION)

enum {
  PROP_0,
  PROP_INITIAL_SCALE,
  PROP_MAXIMUM_SCALE,
  PROP_N_BOUNCE,
  PROP_TARGET,
  PROP_STEPPER,
  NPROPS
};

static GParamSpec *animation_bounce_animation_props [NPROPS] = { NULL, };

double
animation_bounce_animation_get_initial_scale (AnimationBounceAnimation *animation)
{
  return LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->InitialScale ();
}

void
animation_bounce_animation_set_initial_scale (AnimationBounceAnimation *animation,
                                              double                    initial_scale)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->InitialScale (initial_scale);
}

double
animation_bounce_animation_get_maximum_scale (AnimationBounceAnimation *animation)
{
  return LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->MaximumScale ();
}

void
animation_bounce_animation_set_maximum_scale (AnimationBounceAnimation *animation,
                                              double                    maximum_scale)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->MaximumScale (maximum_scale);
}

unsigned int
animation_bounce_animation_get_n_bounce (AnimationBounceAnimation *animation)
{
  return LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->NBounce ();
}

void
animation_bounce_animation_set_n_bounce (AnimationBounceAnimation *animation,
                                         unsigned int              n_bounce)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->NBounce (n_bounce);
}

void
animation_bounce_animation_set_target (AnimationBounceAnimation *animation,
                                       AnimationBoxQuery        *target)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->Target (std::make_shared <ag::BoxQueryWrapper> (target));
}

/**
 * animation_bounce_animation_get_target:
 * @animation: An #AnimationBoxQuery
 *
 * Returns: (transfer none): Get the #AnimationBoxQuery target for this #AnimationBounceAnimation
 */
AnimationBoxQuery *
animation_bounce_animation_get_target (AnimationBounceAnimation *animation)
{
  return std::static_pointer_cast <ag::BoxQueryWrapper> (LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->Target ())->BoxQuery ();
}

void
animation_bounce_animation_set_stepper (AnimationBounceAnimation *animation,
                                        AnimationStepper         *stepper)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->Stepper (std::make_shared <asg::StepperWrapper> (stepper));
}

/**
 * animation_bounce_animation_get_stepper:
 * @animation: An #AnimationBounceAnimation
 *
 * Returns: (transfer none): Get the #AnimationStepper for this #AnimationBounceAnimation
 */
AnimationStepper *
animation_bounce_animation_get_stepper (AnimationBounceAnimation *animation)
{
  auto const &stepper (LookupTypedInterfaceProp <at::TransformAnimation, ab::BounceAnimation> (G_OBJECT (animation))->Stepper ());

  return std::static_pointer_cast <asg::StepperWrapper> (stepper)->BaseStepper ();
}

static void
animation_bounce_animation_set_property (GObject      *object,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
  switch (prop_id)
    {
    case PROP_INITIAL_SCALE:
      animation_bounce_animation_set_initial_scale (ANIMATION_BOUNCE_ANIMATION (object),
                                                    g_value_get_double (value));
      break;
    case PROP_MAXIMUM_SCALE:
      animation_bounce_animation_set_maximum_scale (ANIMATION_BOUNCE_ANIMATION (object),
                                                    g_value_get_double (value));
      break;
    case PROP_N_BOUNCE:
      animation_bounce_animation_set_n_bounce (ANIMATION_BOUNCE_ANIMATION (object),
                                               g_value_get_uint (value));
      break;
    case PROP_TARGET:
      animation_bounce_animation_set_target (ANIMATION_BOUNCE_ANIMATION (object),
                                             ANIMATION_BOX_QUERY (g_value_get_object (value)));
      break;
    case PROP_STEPPER:
      animation_bounce_animation_set_stepper (ANIMATION_BOUNCE_ANIMATION (object),
                                              ANIMATION_STEPPER (g_value_get_object (value)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
animation_bounce_animation_get_property (GObject    *object,
                                         guint       prop_id,
                                         GValue     *value,
                                         GParamSpec *pspec)
{
  AnimationBounceAnimation *bounce_animation = ANIMATION_BOUNCE_ANIMATION (object);

  switch (prop_id)
    {
    case PROP_INITIAL_SCALE:
      g_value_set_double (value, animation_bounce_animation_get_initial_scale (bounce_animation));
      break;
    case PROP_MAXIMUM_SCALE:
      g_value_set_double (value, animation_bounce_animation_get_maximum_scale (bounce_animation));
      break;
    case PROP_N_BOUNCE:
      g_value_set_uint (value, animation_bounce_animation_get_n_bounce (bounce_animation));
      break;
    case PROP_TARGET:
      g_value_set_object (value, animation_bounce_animation_get_target (bounce_animation));
      break;
    case PROP_STEPPER:
      g_value_set_object (value, animation_bounce_animation_get_stepper (bounce_animation));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static GObject *
animation_bounce_animation_constructor (GType                  type,
                                        unsigned int           n_construct_params,
                                        GObjectConstructParam *construct_params)
{
  auto *interface = InterfaceConstructor <ab::BounceAnimation>::construct ();
  auto *transform_interface = static_cast <at::TransformAnimation *> (interface);

  /* We need to also set defaults for certain properties in order
   * to ensure that they are bindable later on, in case they are
   * not set by the caller. */
  replace_named_pointer_props_in_construct_params_if_null (
    construct_params,
    n_construct_params,
    {
      ReplacePropSpec ("stepper", g_value_get_object, g_value_set_object, []() -> gpointer {
        return animation_linear_stepper_new (1);
      }),
      ReplacePropSpec ("target", g_value_get_object, g_value_set_object, []() -> gpointer {
        return animation_box_query_new ();
      })
    }
  );

  replace_interface_prop_in_construct_params (construct_params,
                                              n_construct_params,
                                              transform_interface);

  return G_OBJECT_CLASS (animation_bounce_animation_parent_class)->constructor (type,
                                                                                n_construct_params,
                                                                                construct_params);
}

static void
animation_bounce_animation_constructed (GObject *object)
{
  /* Take a size-zero step, which ensures that we update the
   * internal state of the animation with all the properties we
   * just set */
  animation_transform_animation_step (ANIMATION_TRANSFORM_ANIMATION (object), 0);
}

static void
animation_bounce_animation_init (AnimationBounceAnimation *model)
{
}


static void
animation_bounce_animation_class_init (AnimationBounceAnimationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructor = animation_bounce_animation_constructor;
  object_class->constructed = animation_bounce_animation_constructed;
  object_class->get_property = animation_bounce_animation_get_property;
  object_class->set_property = animation_bounce_animation_set_property;

  animation_bounce_animation_props[PROP_INITIAL_SCALE] =
    g_param_spec_double ("initial-scale",
                         "Initial Scale",
                         "The initial scale of the animation",
                         0.1,
                         1.0,
                         0.7,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_bounce_animation_props[PROP_MAXIMUM_SCALE] =
    g_param_spec_double ("maximum-scale",
                         "Maximum Scale",
                         "The maximum scale of the animation",
                         1.0,
                         3.0,
                         1.2,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_bounce_animation_props[PROP_N_BOUNCE] =
    g_param_spec_uint ("n-bounce",
                       "Number of Bounces",
                       "The number of bounces in the animation",
                       1,
                       10,
                       1,
                       static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_bounce_animation_props[PROP_TARGET] =
    g_param_spec_object ("target",
                         "Target Box",
                         "Box that we are animating to",
                         ANIMATION_TYPE_BOX_QUERY,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_bounce_animation_props[PROP_STEPPER] =
    g_param_spec_object ("stepper",
                         "Stepper",
                         "Stepper to use to progress the animation",
                         ANIMATION_TYPE_STEPPER,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_properties (object_class,
                                     NPROPS,
                                     animation_bounce_animation_props);
}

/**
 * animation_bounce_new:
 * @initial_scale: Scale factor that the surface will initially have.
 * @maximum_scale: Scale factor that the surface will have at maximum.
 * @n_bounce: Number of bounces.
 * @target: The #AnimationBoxQuery that we are animating to.
 * @stepper: The #AnimationStepper of the animation.
 *
 * Returns: (transfer full): A new #AnimationBounceAnimation.
 */
AnimationBounceAnimation *
animation_bounce_new (double                    initial_scale,
                      double                    maximum_scale,
                      unsigned int             n_bounce,
                      const AnimationBoxQuery *target,
                      AnimationStepper        *stepper)
{
  return ANIMATION_BOUNCE_ANIMATION (g_object_new (ANIMATION_TYPE_BOUNCE_ANIMATION,
                                                   "initial-scale", initial_scale,
                                                   "maximum-scale", maximum_scale,
                                                   "n-bounce", n_bounce,
                                                   "target", target,
                                                   "stepper", stepper,
                                                   NULL));
}
