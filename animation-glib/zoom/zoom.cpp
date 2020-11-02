/*
 * animation-glib/zoom/zoom.cpp
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
 * GObject interface for a zoom animation.
 */

#include <animation/zoom/zoom.h>

#include <animation-glib/box.h>
#include <animation-glib/constructor-helpers.h>
#include <animation-glib/stepper/linear.h>
#include <animation-glib/stepper/stepper-internal.h>
#include <animation-glib/vector.h>
#include <animation-glib/zoom/zoom.h>

namespace asg = animation::stepper::glib;
namespace agl = animation::glib;
namespace at = animation::transform;
namespace az = animation::zoom;

struct _AnimationZoomAnimation
{
  AnimationTransformAnimation parent_instance;
};

typedef struct _AnimationZoomAnimationPrivate
{
} AnimationZoomAnimationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (AnimationZoomAnimation,
                            animation_zoom_animation,
                            ANIMATION_TYPE_TRANSFORM_ANIMATION)

enum {
  PROP_0,
  PROP_SOURCE,
  PROP_TARGET,
  PROP_STEPPER,
  NPROPS
};

static GParamSpec *animation_zoom_animation_props [NPROPS] = { NULL, };

void
animation_zoom_animation_set_source (AnimationZoomAnimation *animation,
                                     AnimationBoxQuery      *source)
{
  LookupTypedInterfaceProp <at::TransformAnimation, az::ZoomAnimation> (G_OBJECT (animation))->Source (std::make_shared <agl::BoxQueryWrapper> (source));
}

/**
 * animation_zoom_animation_get_source:
 * @animation: An #AnimationBoxQuery
 *
 * Returns: (transfer none): Get the #AnimationBoxQuery source for this #AnimationZoomAnimation
 */
AnimationBoxQuery *
animation_zoom_animation_get_source (AnimationZoomAnimation *animation)
{
  return std::static_pointer_cast <agl::BoxQueryWrapper> (LookupTypedInterfaceProp <at::TransformAnimation, az::ZoomAnimation> (G_OBJECT (animation))->Source ())->BoxQuery ();
}

void
animation_zoom_animation_set_target (AnimationZoomAnimation *animation,
                                     AnimationBoxQuery      *target)
{
  LookupTypedInterfaceProp <at::TransformAnimation, az::ZoomAnimation> (G_OBJECT (animation))->Target (std::make_shared <agl::BoxQueryWrapper> (target));
}

/**
 * animation_zoom_animation_get_target:
 * @animation: An #AnimationBoxQuery
 *
 * Returns: (transfer none): Get the #AnimationBoxQuery target for this #AnimationZoomAnimation
 */
AnimationBoxQuery *
animation_zoom_animation_get_target (AnimationZoomAnimation *animation)
{
  return std::static_pointer_cast <agl::BoxQueryWrapper> (LookupTypedInterfaceProp <at::TransformAnimation, az::ZoomAnimation> (G_OBJECT (animation))->Target ())->BoxQuery ();
}


void
animation_zoom_animation_set_stepper (AnimationZoomAnimation *animation,
                                      AnimationStepper       *stepper)
{
  LookupTypedInterfaceProp <at::TransformAnimation, az::ZoomAnimation> (G_OBJECT (animation))->Stepper (std::make_shared <asg::StepperWrapper> (stepper));
}

/**
 * animation_zoom_animation_get_stepper:
 * @animation: An #AnimationZoomAnimation
 *
 * Returns: (transfer full): Get the stepper for this #AnimationZoomAnimation
 */
AnimationStepper *
animation_zoom_animation_get_stepper (AnimationZoomAnimation *animation)
{
  auto const &stepper (LookupTypedInterfaceProp <at::TransformAnimation, az::ZoomAnimation> (G_OBJECT (animation))->Stepper ());

  return std::static_pointer_cast <asg::StepperWrapper> (stepper)->BaseStepper ();
}

static void
animation_zoom_animation_set_property (GObject      *object,
                                       guint         prop_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
  AnimationZoomAnimation *zoom_animation = ANIMATION_ZOOM_ANIMATION (object);

  switch (prop_id)
    {
    case PROP_SOURCE:
      animation_zoom_animation_set_source (zoom_animation,
                                           ANIMATION_BOX_QUERY (g_value_get_object (value)));
      break;
    case PROP_TARGET:
      animation_zoom_animation_set_target (zoom_animation,
                                           ANIMATION_BOX_QUERY (g_value_get_object (value)));
      break;
    case PROP_STEPPER:
      animation_zoom_animation_set_stepper (zoom_animation,
                                            ANIMATION_STEPPER (g_value_get_object (value)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
animation_zoom_animation_get_property (GObject    *object,
                                       guint       prop_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
  AnimationZoomAnimation *zoom_animation = ANIMATION_ZOOM_ANIMATION (object);

  switch (prop_id)
    {
    case PROP_SOURCE:
      g_value_set_object (value,
                          animation_zoom_animation_get_source (zoom_animation));
      break;
    case PROP_TARGET:
      g_value_set_object (value,
                          animation_zoom_animation_get_target (zoom_animation));
      break;
    case PROP_STEPPER:
      g_value_set_object (value, animation_zoom_animation_get_stepper (zoom_animation));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static GObject *
animation_zoom_animation_constructor (GType                  type,
                                      unsigned int           n_construct_params,
                                      GObjectConstructParam *construct_params)
{
  auto *interface = InterfaceConstructor <az::ZoomAnimation>::construct ();
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
      ReplacePropSpec ("source", g_value_get_object, g_value_set_object, []() -> gpointer {
        return animation_box_query_new ();
      }),
      ReplacePropSpec ("target", g_value_get_object, g_value_set_object, []() -> gpointer {
        return animation_box_query_new ();
      })
    }
  );

  replace_interface_prop_in_construct_params (construct_params,
                                              n_construct_params,
                                              transform_interface);

  return G_OBJECT_CLASS (animation_zoom_animation_parent_class)->constructor (type,
                                                                              n_construct_params,
                                                                              construct_params);
}

static void
animation_zoom_animation_constructed (GObject *object)
{
  /* Take a size-zero step, which ensures that we update the
   * internal state of the animation with all the properties we
   * just set */
  animation_transform_animation_step (ANIMATION_TRANSFORM_ANIMATION (object), 0);
}

static void
animation_zoom_animation_init (AnimationZoomAnimation *model)
{
}

static void
animation_zoom_animation_class_init (AnimationZoomAnimationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructor = animation_zoom_animation_constructor;
  object_class->constructed = animation_zoom_animation_constructed;
  object_class->get_property = animation_zoom_animation_get_property;
  object_class->set_property = animation_zoom_animation_set_property;

  animation_zoom_animation_props[PROP_SOURCE] =
    g_param_spec_object ("source",
                         "Source Box Query",
                         "BoxQuery that we are animating from",
                         ANIMATION_TYPE_BOX_QUERY,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  animation_zoom_animation_props[PROP_TARGET] =
    g_param_spec_object ("target",
                         "Target Box Query",
                         "BoxQuery that we are animating to",
                         ANIMATION_TYPE_BOX_QUERY,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  animation_zoom_animation_props[PROP_STEPPER] =
    g_param_spec_object ("stepper",
                         "Stepper",
                         "Stepper to use to progress the animation",
                         ANIMATION_TYPE_STEPPER,
                         static_cast <GParamFlags> (G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_properties (object_class,
                                     NPROPS,
                                     animation_zoom_animation_props);
}

/**
 * animation_zoom_new:
 * @source: The #AnimationBoxQuery that we are animating from.
 * @target: The #AnimationBoxQuery that we are animating to (current location).
 * @stepper: The stepper to use.
 *
 * Returns: (transfer full): A new #AnimationZoomAnimation.
 */
AnimationZoomAnimation *
animation_zoom_new (const AnimationBoxQuery *source,
                    const AnimationBoxQuery *target,
                    AnimationStepper        *stepper)
{
  return ANIMATION_ZOOM_ANIMATION (g_object_new (ANIMATION_TYPE_ZOOM_ANIMATION,
                                                 "source", source,
                                                 "target", target,
                                                 "stepper", stepper,
                                                 NULL));
}
