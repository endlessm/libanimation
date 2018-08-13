/*
 * animation-glib/constructor-helpers.cpp
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
 * Helpers for constructing C++ objects directly from GObject properties
 * in a GObject constructor.
 */

#pragma once

#include <glib.h>
#include <glib-object.h>

#include <array>
#include <type_traits>
#include <utility>

#include <animation/geometry.h>
#include <animation/query/geometry_query.h>
#include <animation/stepper/linear.h>
#include <animation/stepper/stepper.h>
#include <animation-glib/box.h>
#include <animation-glib/query/geometry-query.h>
#include <animation-glib/query/geometry-query-private.h>
#include <animation-glib/stepper/stepper.h>
#include <animation-glib/stepper/stepper-internal.h>

G_BEGIN_DECLS

inline GValue *
lookup_gvalue (GHashTable *ht, const char *key)
{
  return reinterpret_cast <GValue *> (g_hash_table_lookup (ht, key));
}

typedef void (*AnimationConstructorHelpersInitializeValueFunc) (GValue   *value,
                                                                gpointer  user_data);

void replace_construct_param (GObjectConstructParam                          *construct_params,
                              unsigned int                                    n_construct_params,
                              const char                                     *prop_name,
                              AnimationConstructorHelpersInitializeValueFunc  initialize_func,
                              gpointer                                        initialize_func_data);

void replace_named_pointer_prop_in_construct_params (GObjectConstructParam *construct_params,
                                                     unsigned int           n_construct_params,
                                                     const char            *prop_name,
                                                     gpointer               ptr);

void replace_interface_prop_in_construct_params (GObjectConstructParam *construct_params,
                                                 unsigned int           n_construct_params,
                                                 gpointer               interface);

typedef gpointer (*AnimationConstructorHelpersGValueGetPointerFunc) (const GValue *value);
typedef void (*AnimationConstructorHelpersGValueSetPointerFunc) (GValue *value, gpointer);
typedef gpointer (*AnimationConstructorHelpersConstructDefaultValueFunc) (void);

void replace_named_pointer_prop_in_construct_params_if_null (GObjectConstructParam                                *construct_params,
                                                             unsigned int                                          n_construct_params,
                                                             const char                                           *prop_name,
                                                             AnimationConstructorHelpersGValueGetPointerFunc       get_func,
                                                             AnimationConstructorHelpersGValueSetPointerFunc       set_func,
                                                             AnimationConstructorHelpersConstructDefaultValueFunc  construct_func);

G_END_DECLS

#ifdef __cplusplus
struct ReplacePropSpec
{
    ReplacePropSpec (const char                                           *prop_name,
                     AnimationConstructorHelpersGValueGetPointerFunc       get_func,
                     AnimationConstructorHelpersGValueSetPointerFunc       set_func,
                     AnimationConstructorHelpersConstructDefaultValueFunc  construct_func) :
        prop_name (prop_name),
        get_func (get_func),
        set_func (set_func),
        construct_func (construct_func)
    {
    }

    const char                                           *prop_name;
    AnimationConstructorHelpersGValueGetPointerFunc       get_func;
    AnimationConstructorHelpersGValueSetPointerFunc       set_func;
    AnimationConstructorHelpersConstructDefaultValueFunc  construct_func;
};

void replace_named_pointer_props_in_construct_params_if_null (GObjectConstructParam                   *construct_params,
                                                              unsigned int                             n_construct_params,
                                                              std::initializer_list <ReplacePropSpec>  replace_specs);

template <typename Marshaller>
typename std::result_of <Marshaller(GValue *)>::type ForwardFromValueHT (GHashTable  *ht,
                                                                         Marshaller &&m,
                                                                         const char  *name)
{
  return m (lookup_gvalue (ht, name));
}

template <class Interface>
struct InterfaceConstructor
{
    template <typename... ArgTypes>
    static Interface * construct (ArgTypes&&... args)
    {
        return new Interface (args...);
    }
};

template <class InterfaceType, class DerivedType>
DerivedType * LookupTypedInterfaceProp (GObject *object)
{
  InterfaceType *iface = nullptr;
  g_object_get (object, "interface", (gpointer) &iface, NULL);

  return static_cast <DerivedType *> (iface);
}
#endif
