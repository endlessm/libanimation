/*
 * animation-glib/glide/glide.cpp
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
 * GObject implementation for a "glide" animation.
 */

#include <animation/glide/glide.h>

#include <animation-glib/box.h>
#include <animation-glib/constructor-helpers.h>
#include <animation-glib/glide/glide.h>
#include <animation-glib/query/geometry-query.h>
#include <animation-glib/stepper/linear.h>
#include <animation-glib/stepper/stepper.h>
#include <animation-glib/stepper/stepper-internal.h>
#include <animation-glib/query/geometry-query.h>
#include <animation-glib/query/geometry-query-private.h>
#include <animation-glib/vector.h>

namespace agd = animation::geometry::dimension;
namespace ag = animation::glide;
namespace agl = animation::glib;
namespace at = animation::transform;
namespace asg = animation::stepper::glib;

struct _AnimationGlideAnimation
{
  AnimationTransformAnimation parent_instance;
};

typedef struct _AnimationGlideAnimationPrivate
{
} AnimationGlideAnimationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AnimationGlideAnimation,
                            animation_glide_animation,
                            ANIMATION_TYPE_TRANSFORM_ANIMATION)

enum {
  PROP_0,
  PROP_INITIAL_DISTANCE,
  PROP_X_ROTATION_ANGLE_DEGREES,
  PROP_Y_ROTATION_ANGLE_DEGREES,
  PROP_X_AXIS_LOCATION_UNIT,
  PROP_Y_AXIS_LOCATION_UNIT,
  PROP_VIEWPORT,
  PROP_TARGET,
  PROP_STEPPER,
  NPROPS
};

static GParamSpec *animation_glide_animation_props [NPROPS] = { NULL, };

void
animation_glide_animation_set_initial_distance (AnimationGlideAnimation *animation,
                                                double                   initial_distance)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->InitialDistance (initial_distance);
}

double
animation_glide_animation_get_initial_distance (AnimationGlideAnimation *animation)
{
  return LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->InitialDistance ();
}

void
animation_glide_animation_set_x_rotation_angle_degrees (AnimationGlideAnimation *animation,
                                                        double                   x_rotation_angle_degrees)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->XRotationAngleDegrees (x_rotation_angle_degrees);
}

double
animation_glide_animation_get_x_rotation_angle_degrees (AnimationGlideAnimation *animation)
{
  return LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->XRotationAngleDegrees ();
}

void
animation_glide_animation_set_y_rotation_angle_degrees (AnimationGlideAnimation *animation,
                                                        double                   y_rotation_angle_degrees)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->YRotationAngleDegrees (y_rotation_angle_degrees);
}

double
animation_glide_animation_get_y_rotation_angle_degrees (AnimationGlideAnimation *animation)
{
  return LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->YRotationAngleDegrees ();
}

void
animation_glide_animation_set_x_axis_location_unit (AnimationGlideAnimation *animation,
                                                    double                   x_axis_location_unit)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->XAxisLocationUnit (x_axis_location_unit);
}

double
animation_glide_animation_get_x_axis_location_unit (AnimationGlideAnimation *animation)
{
  return LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->XAxisLocationUnit ();
}

void
animation_glide_animation_set_y_axis_location_unit (AnimationGlideAnimation *animation,
                                                    double                   y_axis_location_unit)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->YAxisLocationUnit (y_axis_location_unit);
}

double
animation_glide_animation_get_y_axis_location_unit (AnimationGlideAnimation *animation)
{
  return LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->YAxisLocationUnit ();
}

void
animation_glide_animation_set_target (AnimationGlideAnimation *animation,
                                      AnimationBoxQuery       *target)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->Target (std::make_shared <agl::BoxQueryWrapper> (target));
}

/**
 * animation_glide_animation_get_target:
 * @animation: An #AnimationBoxQuery
 *
 * Returns: (transfer none): Get the #AnimationBoxQuery target for this #AnimationGlideAnimation
 */
AnimationBoxQuery *
animation_glide_animation_get_target (AnimationGlideAnimation *animation)
{
  return std::static_pointer_cast <agl::BoxQueryWrapper> (LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->Target ())->BoxQuery ();
}

void
animation_glide_animation_set_viewport (AnimationGlideAnimation *animation,
                                        AnimationBoxQuery       *viewport)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->Viewport (std::make_shared <agl::BoxQueryWrapper> (viewport));
}

/**
 * animation_glide_animation_get_viewport:
 * @animation: An #AnimationBoxQuery
 *
 * Returns: (transfer none): Get the #AnimationBoxQuery for the viewport for this #AnimationGlideAnimation
 */
AnimationBoxQuery *
animation_glide_animation_get_viewport (AnimationGlideAnimation *animation)
{
  return std::static_pointer_cast <agl::BoxQueryWrapper> (LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->Viewport ())->BoxQuery ();
}

void
animation_glide_animation_set_stepper (AnimationGlideAnimation *animation,
                                       AnimationStepper        *stepper)
{
  LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->Stepper (std::make_shared <asg::StepperWrapper> (stepper));
}

/**
 * animation_glide_animation_get_stepper:
 * @animation: An #AnimationGlideAnimation
 *
 * Returns: (transfer full): Get the stepper for this #AnimationGlideAnimation
 */
AnimationStepper *
animation_glide_animation_get_stepper (AnimationGlideAnimation *animation)
{
  auto const &stepper (LookupTypedInterfaceProp <at::TransformAnimation, ag::GlideAnimation> (G_OBJECT (animation))->Stepper ());

  return std::static_pointer_cast <asg::StepperWrapper> (stepper)->BaseStepper ();
}

static void
animation_glide_animation_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  AnimationGlideAnimation *glide_animation = ANIMATION_GLIDE_ANIMATION (object);

  switch (prop_id)
    {
    case PROP_INITIAL_DISTANCE:
      animation_glide_animation_set_initial_distance (glide_animation, g_value_get_double (value));
      break;
    case PROP_X_ROTATION_ANGLE_DEGREES:
      animation_glide_animation_set_x_rotation_angle_degrees (glide_animation, g_value_get_double (value));
      break;
    case PROP_Y_ROTATION_ANGLE_DEGREES:
      animation_glide_animation_set_y_rotation_angle_degrees (glide_animation, g_value_get_double (value));
      break;
    case PROP_X_AXIS_LOCATION_UNIT:
      animation_glide_animation_set_x_axis_location_unit (glide_animation, g_value_get_double (value));
      break;
    case PROP_Y_AXIS_LOCATION_UNIT:
      animation_glide_animation_set_y_axis_location_unit (glide_animation, g_value_get_double (value));
      break;
    case PROP_VIEWPORT:
      animation_glide_animation_set_viewport (glide_animation,
                                              ANIMATION_BOX_QUERY (g_value_get_object (value)));
      break;
    case PROP_TARGET:
      animation_glide_animation_set_target (glide_animation,
                                            ANIMATION_BOX_QUERY (g_value_get_object (value)));
      break;
    case PROP_STEPPER:
      animation_glide_animation_set_stepper (glide_animation,
                                             ANIMATION_STEPPER (g_value_get_object (value)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
animation_glide_animation_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  AnimationGlideAnimation *glide_animation = ANIMATION_GLIDE_ANIMATION (object);

  switch (prop_id)
    {
    case PROP_INITIAL_DISTANCE:
      g_value_set_double (value, animation_glide_animation_get_initial_distance (glide_animation));
      break;
    case PROP_X_ROTATION_ANGLE_DEGREES:
      g_value_set_double (value, animation_glide_animation_get_x_rotation_angle_degrees (glide_animation));
      break;
    case PROP_Y_ROTATION_ANGLE_DEGREES:
      g_value_set_double (value, animation_glide_animation_get_y_rotation_angle_degrees (glide_animation));
      break;
    case PROP_X_AXIS_LOCATION_UNIT:
      g_value_set_double (value, animation_glide_animation_get_x_axis_location_unit (glide_animation));
      break;
    case PROP_Y_AXIS_LOCATION_UNIT:
      g_value_set_double (value, animation_glide_animation_get_y_axis_location_unit (glide_animation));
      break;
    case PROP_VIEWPORT:
      g_value_set_object (value, animation_glide_animation_get_viewport (glide_animation));
      break;
    case PROP_TARGET:
      g_value_set_object (value, animation_glide_animation_get_target (glide_animation));
      break;
    case PROP_STEPPER:
      g_value_set_object (value, animation_glide_animation_get_stepper (glide_animation));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static GObject *
animation_glide_animation_constructor (GType                  type,
                                       unsigned int           n_construct_params,
                                       GObjectConstructParam *construct_params)
{
  auto *interface = InterfaceConstructor <ag::GlideAnimation>::construct ();
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
      }),
      ReplacePropSpec ("viewport", g_value_get_object, g_value_set_object, []() -> gpointer {
        return animation_box_query_new ();
      })
    }
  );

  replace_interface_prop_in_construct_params (construct_params,
                                              n_construct_params,
                                              transform_interface);

  return G_OBJECT_CLASS (animation_glide_animation_parent_class)->constructor (type,
                                                                               n_construct_params,
                                                                               construct_params);
}

static void
animation_glide_animation_constructed (GObject *object)
{
  /* Take a size-zero step, which ensures that we update the
   * internal state of the animation with all the properties we
   * just set */
  animation_transform_animation_step (ANIMATION_TRANSFORM_ANIMATION (object), 0);
}

static void
animation_glide_animation_init (AnimationGlideAnimation *model)
{
}


static void
animation_glide_animation_class_init (AnimationGlideAnimationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructor = animation_glide_animation_constructor;
  object_class->constructed = animation_glide_animation_constructed;
  object_class->get_property = animation_glide_animation_get_property;
  object_class->set_property = animation_glide_animation_set_property;

  animation_glide_animation_props[PROP_INITIAL_DISTANCE] =
    g_param_spec_double ("initial-distance",
                         "Initial Distance",
                         "The initial distance away from the camera",
                         -1.0,
                         1.0,
                         -0.3,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_glide_animation_props[PROP_X_ROTATION_ANGLE_DEGREES] =
    g_param_spec_double ("x-rotation-angle-degrees",
                         "X Rotation Angle Degrees",
                         "Number of degrees on the X axis to rotate",
                         -360.0,
                         360.0,
                         0.0,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_glide_animation_props[PROP_Y_ROTATION_ANGLE_DEGREES] =
    g_param_spec_double ("y-rotation-angle-degrees",
                         "Y Rotation Angle Degrees",
                         "Number of degrees on the Y axis to rotate",
                         -360.0,
                         360.0,
                         0.0,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_glide_animation_props[PROP_X_AXIS_LOCATION_UNIT] =
    g_param_spec_double ("x-axis-location-unit",
                         "X Axis Location Unit",
                         "Unit-coordinates of where the X axis is on the surface",
                         0.0,
                         1.0,
                         0.2,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_glide_animation_props[PROP_Y_AXIS_LOCATION_UNIT] =
    g_param_spec_double ("y-axis-location-unit",
                         "Y Axis Location Unit",
                         "Unit-coordinates of where the Y axis is on the surface",
                         0.0,
                         1.0,
                         0.5,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_glide_animation_props[PROP_VIEWPORT] =
    g_param_spec_object ("viewport",
                         "Viewport BoxQuery",
                         "BoxQuery for the viewport dimensions that the surface is in",
                         ANIMATION_TYPE_BOX_QUERY,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_glide_animation_props[PROP_TARGET] =
    g_param_spec_object ("target",
                         "Target BoxQuery",
                         "BoxQuery for box that we are animating to",
                         ANIMATION_TYPE_BOX_QUERY,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_glide_animation_props[PROP_STEPPER] =
    g_param_spec_object ("stepper",
                         "Stepper",
                         "Stepper to use to progress the animation",
                         ANIMATION_TYPE_STEPPER,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_properties (object_class,
                                     NPROPS,
                                     animation_glide_animation_props);
}

/**
 * animation_glide_new:
 * @initial_distance: Initial distance frm the camera.
 * @x_rotation_angle_degrees: Degrees of rotation towards the X axis.
 * @y_rotation_angle_degrees: Degrees of rotation towards the Y axis.
 * @y_axis_location_unit: Unit-coordinates of where the X axis is on the surface.
 * @x_axis_location_unit: Unit-coordinates of where the Y axis is on the surface.
 * @screen_width: Width of the screen, in pixels.
 * @target_box: The #AnimationBoxQuery that we are animating to.
 * @length: The length of the animation.
 *
 * Returns: (transfer full): A new #AnimationGlideAnimation.
 */
AnimationGlideAnimation *
animation_glide_new (double                   initial_distance,
                     double                   x_rotation_angle_degrees,
                     double                   y_rotation_angle_degrees,
                     double                   x_axis_location_unit,
                     double                   y_axis_location_unit,
                     unsigned int             screen_width,
                     const AnimationBoxQuery *target_box,
                     unsigned int             length)
{
  return ANIMATION_GLIDE_ANIMATION (g_object_new (ANIMATION_TYPE_GLIDE_ANIMATION,
                                                  "initial-distance", initial_distance,
                                                  "x-rotation-angle-degrees", x_rotation_angle_degrees,
                                                  "y-rotation-angle-degrees", y_rotation_angle_degrees,
                                                  "x-axis-location-unit", x_axis_location_unit,
                                                  "y-axis-location-unit", y_axis_location_unit,
                                                  "screen-width", screen_width,
                                                  "target", target_box,
                                                  "length", length,
                                                  NULL));
}
