/*
 * animation-glib/magiclamp/magiclamp.cpp
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
 * GObject implementation for a "magiclamp" animation.
 */

#include <animation/magiclamp/magiclamp.h>

#include <animation/geometry.h>

#include <animation-glib/box.h>
#include <animation-glib/constructor-helpers.h>
#include <animation-glib/grid/grid.h>
#include <animation-glib/magiclamp/magiclamp.h>
#include <animation-glib/stepper/linear.h>
#include <animation-glib/stepper/stepper.h>
#include <animation-glib/stepper/stepper-internal.h>
#include <animation-glib/query/geometry-query.h>
#include <animation-glib/query/geometry-query-private.h>
#include <animation-glib/vector.h>

namespace ag = animation::grid;
namespace agd = animation::geometry::dimension;
namespace age = animation::geometry;
namespace agl = animation::glib;
namespace aml = animation::magiclamp;
namespace asg = animation::stepper::glib;

struct _AnimationMagicLampAnimation
{
  AnimationGridAnimation parent_instance;
};

typedef struct _AnimationMagicLampAnimationPrivate
{
} AnimationMagicLampAnimationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AnimationMagicLampAnimation,
                            animation_magiclamp_animation,
                            ANIMATION_TYPE_GRID_ANIMATION)

enum {
  PROP_0,
  PROP_SOURCE,
  PROP_TARGET,
  PROP_RESOLUTION,
  PROP_BEND_FACTOR,
  PROP_OFFSET_FACTOR,
  PROP_STRETCH_FACTOR,
  PROP_DEFORM_SPEED_FACTOR,
  PROP_STEPPER,
  NPROPS
};

static GParamSpec *animation_magiclamp_animation_props [NPROPS] = { NULL, };

void
animation_magiclamp_animation_set_bend_factor (AnimationMagicLampAnimation *animation,
                                               double                       bend_factor)
{
  LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->BendFactor (bend_factor);
}

double
animation_magiclamp_animation_get_bend_factor (AnimationMagicLampAnimation *animation)
{
  return LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->BendFactor ();
}

void
animation_magiclamp_animation_set_offset_factor (AnimationMagicLampAnimation *animation,
                                                 double                       offset_factor)
{
  LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->OffsetFactor (offset_factor);
}

double
animation_magiclamp_animation_get_offset_factor (AnimationMagicLampAnimation *animation)
{
  return LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->OffsetFactor ();
}

void
animation_magiclamp_animation_set_stretch_factor (AnimationMagicLampAnimation *animation,
                                                  double                       stretch_factor)
{
  LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->StretchFactor (stretch_factor);
}

double
animation_magiclamp_animation_get_stretch_factor (AnimationMagicLampAnimation *animation)
{
  return LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->StretchFactor ();
}

void
animation_magiclamp_animation_set_deform_speed_factor (AnimationMagicLampAnimation *animation,
                                                       double                       deform_speed_factor)
{
  LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->DeformSpeedFactor (deform_speed_factor);
}

double
animation_magiclamp_animation_get_deform_speed_factor (AnimationMagicLampAnimation *animation)
{
  return LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->DeformSpeedFactor ();
}

void
animation_magiclamp_animation_set_resolution (AnimationMagicLampAnimation *animation,
                                              AnimationVector             *vector)
{
  auto resolution = age::PointModel <size_t> (vector->x, vector->y);
  LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->GridResolution (resolution);
}

/**
 * animation_magiclamp_animation_get_resolution:
 * @animation: An #AnimationMagicLampAnimation
 * @vector: (out): The #AnimationVector to write the resolution into.
 */
void
animation_magiclamp_animation_get_resolution (AnimationMagicLampAnimation *animation,
                                              AnimationVector             *vector)
{
  auto resolution = LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->GridResolution ();

  vector->x = static_cast <size_t> (resolution.x);
  vector->y = static_cast <size_t> (resolution.y);
}

void
animation_magiclamp_animation_set_source (AnimationMagicLampAnimation *animation,
                                          AnimationBoxQuery           *source)
{
  LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->Source (std::make_shared <agl::BoxQueryWrapper> (source));
}

/**
 * animation_magiclamp_animation_get_source:
 * @animation: A #AnimationMagicLampAnimation
 *
 * Returns: (transfer none): Get the #AnimationBoxQuery source for this #AnimationMagicLampAnimation
 */
AnimationBoxQuery *
animation_magiclamp_animation_get_source (AnimationMagicLampAnimation *animation)
{
  return std::static_pointer_cast <agl::BoxQueryWrapper> (LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->Source ())->BoxQuery ();
}

void
animation_magiclamp_animation_set_target (AnimationMagicLampAnimation *animation,
                                          AnimationBoxQuery           *target)
{
  LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->Target (std::make_shared <agl::BoxQueryWrapper> (target));
}

/**
 * animation_magiclamp_animation_get_target:
 * @animation: A #AnimationMagicLampAnimation
 *
 * Returns: (transfer none): Get the #AnimationBoxQuery target for this #AnimationMagicLampAnimation
 */
AnimationBoxQuery *
animation_magiclamp_animation_get_target (AnimationMagicLampAnimation *animation)
{
  return std::static_pointer_cast <agl::BoxQueryWrapper> (LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->Target ())->BoxQuery ();
}

void
animation_magiclamp_animation_set_stepper (AnimationMagicLampAnimation *animation,
                                           AnimationStepper            *stepper)
{
  LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->Stepper (std::make_shared <asg::StepperWrapper> (stepper));
}

/**
 * animation_magiclamp_animation_get_stepper:
 * @animation: An #AnimationMagicLampAnimation
 *
 * Returns: (transfer full): Get the stepper for this #AnimationMagicLampAnimation
 */
AnimationStepper *
animation_magiclamp_animation_get_stepper (AnimationMagicLampAnimation *animation)
{
  auto const &stepper (LookupTypedInterfaceProp <ag::GridAnimation, aml::MagicLampAnimation> (G_OBJECT (animation))->Stepper ());

  return std::static_pointer_cast <asg::StepperWrapper> (stepper)->BaseStepper ();
}

static void
animation_magiclamp_animation_set_property (GObject      *object,
                                            guint         prop_id,
                                            const GValue *value,
                                            GParamSpec   *pspec)
{
  AnimationMagicLampAnimation *magiclamp_animation = ANIMATION_MAGIC_LAMP_ANIMATION (object);

  switch (prop_id)
    {
    case PROP_SOURCE:
      animation_magiclamp_animation_set_source (magiclamp_animation,
                                                ANIMATION_BOX_QUERY (g_value_get_object (value)));
      break;
    case PROP_TARGET:
      animation_magiclamp_animation_set_target (magiclamp_animation,
                                                ANIMATION_BOX_QUERY (g_value_get_object (value)));
      break;
    case PROP_RESOLUTION:
      animation_magiclamp_animation_set_resolution (magiclamp_animation,
                                                    reinterpret_cast <AnimationVector *> (g_value_get_boxed (value)));
      break;
    case PROP_BEND_FACTOR:
      animation_magiclamp_animation_set_bend_factor (magiclamp_animation, g_value_get_double (value));
      break;
    case PROP_OFFSET_FACTOR:
      animation_magiclamp_animation_set_offset_factor (magiclamp_animation, g_value_get_double (value));
      break;
    case PROP_STRETCH_FACTOR:
      animation_magiclamp_animation_set_stretch_factor (magiclamp_animation, g_value_get_double (value));
      break;
    case PROP_DEFORM_SPEED_FACTOR:
      animation_magiclamp_animation_set_deform_speed_factor (magiclamp_animation, g_value_get_double (value));
      break;
    case PROP_STEPPER:
      animation_magiclamp_animation_set_stepper (magiclamp_animation,
                                                 ANIMATION_STEPPER (g_value_get_object (value)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
animation_magiclamp_animation_get_property (GObject    *object,
                                            guint       prop_id,
                                            GValue     *value,
                                            GParamSpec *pspec)
{
  AnimationMagicLampAnimation *magiclamp_animation = ANIMATION_MAGIC_LAMP_ANIMATION (object);

  switch (prop_id)
    {
    case PROP_SOURCE:
      g_value_set_object (value, animation_magiclamp_animation_get_source (magiclamp_animation));
      break;
    case PROP_TARGET:
      g_value_set_object (value, animation_magiclamp_animation_get_target (magiclamp_animation));
      break;
    case PROP_RESOLUTION:
      {
        AnimationVector vector;
        animation_magiclamp_animation_get_resolution (magiclamp_animation, &vector);
        g_value_set_boxed (value, reinterpret_cast <gconstpointer> (&vector));
      }
      break;
    case PROP_BEND_FACTOR:
      g_value_set_double (value, animation_magiclamp_animation_get_bend_factor (magiclamp_animation));
      break;
    case PROP_STRETCH_FACTOR:
      g_value_set_double (value, animation_magiclamp_animation_get_stretch_factor (magiclamp_animation));
      break;
    case PROP_OFFSET_FACTOR:
      g_value_set_double (value, animation_magiclamp_animation_get_offset_factor (magiclamp_animation));
      break;
    case PROP_DEFORM_SPEED_FACTOR:
      g_value_set_double (value, animation_magiclamp_animation_get_deform_speed_factor (magiclamp_animation));
      break;
    case PROP_STEPPER:
      g_value_set_object (value, animation_magiclamp_animation_get_stepper (magiclamp_animation));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static GObject *
animation_magiclamp_animation_constructor (GType                  type,
                                           unsigned int           n_construct_params,
                                           GObjectConstructParam *construct_params)
{
  auto *interface = InterfaceConstructor <aml::MagicLampAnimation>::construct ();
  auto *grid_interface = static_cast <ag::GridAnimation *> (interface);

  /* We need to also set defaults for certain properties in order
   * to ensure that they are bindable later on, in case they are
   * not set by the caller. */
  replace_named_pointer_props_in_construct_params_if_null (
    construct_params,
    n_construct_params,
    {
      ReplacePropSpec ("stepper", g_value_get_object, g_value_take_object, []() -> gpointer {
        return animation_linear_stepper_new (1);
      }),
      ReplacePropSpec ("source", g_value_get_object, g_value_take_object, []() -> gpointer {
        return animation_box_query_new ();
      }),
      ReplacePropSpec ("target", g_value_get_object, g_value_take_object, []() -> gpointer {
        return animation_box_query_new ();
      })
    }
  );

  replace_interface_prop_in_construct_params (construct_params,
                                              n_construct_params,
                                              grid_interface);

  return G_OBJECT_CLASS (animation_magiclamp_animation_parent_class)->constructor (type,
                                                                                   n_construct_params,
                                                                                   construct_params);
}

static void
animation_magiclamp_animation_constructed (GObject *object)
{
  /* Take a size-zero step, which ensures that we update the
   * internal state of the animation with all the properties we
   * just set */
  animation_grid_animation_step (ANIMATION_GRID_ANIMATION (object), 0);
}

static void
animation_magiclamp_animation_init (AnimationMagicLampAnimation *model)
{
}


static void
animation_magiclamp_animation_class_init (AnimationMagicLampAnimationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructor = animation_magiclamp_animation_constructor;
  object_class->constructed = animation_magiclamp_animation_constructed;
  object_class->get_property = animation_magiclamp_animation_get_property;
  object_class->set_property = animation_magiclamp_animation_set_property;

  animation_magiclamp_animation_props[PROP_SOURCE] =
    g_param_spec_object ("source",
                         "Source Box",
                         "BoxQuery for box that we are animating from",
                         ANIMATION_TYPE_BOX_QUERY,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_magiclamp_animation_props[PROP_TARGET] =
    g_param_spec_object ("target",
                         "Target Box",
                         "BoxQuery for box that we are animating to",
                         ANIMATION_TYPE_BOX_QUERY,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_magiclamp_animation_props[PROP_RESOLUTION] =
    g_param_spec_boxed ("resolution",
                        "Resolution",
                        "Grid Resolution",
                        ANIMATION_TYPE_VECTOR,
                        static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_magiclamp_animation_props[PROP_BEND_FACTOR] =
    g_param_spec_double ("bend-factor",
                         "Bend Factor",
                         "How much the window should bend",
                         1.0,
                         20.0,
                         10.0,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_magiclamp_animation_props[PROP_OFFSET_FACTOR] =
    g_param_spec_double ("offset-factor",
                         "Offset Factor",
                         "How big the curves of the animation should be",
                         0.1,
                         1.0,
                         0.5,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_magiclamp_animation_props[PROP_STRETCH_FACTOR] =
    g_param_spec_double ("stretch-factor",
                         "Stretch Factor",
                         "How much the window should stretch when animating",
                         0.2,
                         1.0,
                         0.45,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_magiclamp_animation_props[PROP_DEFORM_SPEED_FACTOR] =
    g_param_spec_double ("deform-speed-factor",
                         "Deform Speed Factor",
                         "How quickly the deformation phase should happen",
                         1.0,
                         4.0,
                         2.3,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_magiclamp_animation_props[PROP_STEPPER] =
    g_param_spec_object ("stepper",
                         "Stepper",
                         "Stepper to use to progress the animation",
                         ANIMATION_TYPE_STEPPER,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_properties (object_class,
                                     NPROPS,
                                     animation_magiclamp_animation_props);
}

/**
 * animation_magiclamp_new:
 * @source_box: The #AnimationBoxQuery that we are animating from.
 * @target_box: The #AnimationBoxQuery that we are animating to.
 * @resolution: The #AnimationVector representing the grid resolution
 * @bend_factor: How much the window should bend
 * @offset_factor: How big the curves of the animation should be
 * @deform_speed_factor: How quickly the deformation should complete.
 * @stepper: An #AnimationStepper used for progressing the animation.
 *
 * Returns: (transfer full): A new #AnimationMagicLampAnimation.
 */
AnimationMagicLampAnimation *
animation_magiclamp_new (const AnimationBoxQuery *source_box,
                         const AnimationBoxQuery *target_box,
                         const AnimationVector   *resolution,
                         double                   bend_factor,
                         double                   offset_factor,
                         double                   stretch_factor,
                         double                   deform_speed_factor,
                         AnimationStepper        *stepper)
{
  return ANIMATION_MAGIC_LAMP_ANIMATION (g_object_new (ANIMATION_TYPE_MAGIC_LAMP_ANIMATION,
                                                       "source", source_box,
                                                       "target", target_box,
                                                       "resolution", resolution,
                                                       "bend-factor", bend_factor,
                                                       "offset-factor", offset_factor,
                                                       "stretch-factor", stretch_factor,
                                                       "deform-speed-factor", deform_speed_factor,
                                                       "stepper", stepper,
                                                       NULL));
}
