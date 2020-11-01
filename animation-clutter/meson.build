# animation-clutter/meson.build
#
# Copyright (C) 2017, 2018 Endless Mobile, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Meson build definitions for clutter backend to libanimation.

api_version = '0'

animation_clutter_introspectable_sources = files([])
animation_clutter_private_headers = files([])
animation_clutter_private_sources = files([])
animation_clutter_headers = files([])
animation_clutter_headers_subdir = 'animation-clutter'
animation_clutter_sources = files([])

animation_clutter_toplevel_private_headers = files([
  'animation-clutter-common-private.h'
])
animation_clutter_toplevel_headers = files([
  'animation-clutter-affine-effect.h',
  'animation-clutter-actor-box-query.h',
  'animation-clutter-common.h',
  'animation-clutter-grid-effect.h',
  'animation-clutter-wobbly-effect.h'
])

animation_clutter_toplevel_private_sources = files([])
animation_clutter_toplevel_introspectable_sources = files([
    'animation-clutter-affine-effect.c',
    'animation-clutter-actor-box-query.c',
    'animation-clutter-common.c',
    'animation-clutter-grid-effect.c',
    'animation-clutter-wobbly-effect.c'
])

animation_clutter_introspectable_sources += animation_clutter_toplevel_introspectable_sources
animation_clutter_private_sources += animation_clutter_toplevel_private_sources
animation_clutter_headers += animation_clutter_toplevel_headers
animation_clutter_private_headers += animation_clutter_toplevel_private_headers

install_headers(animation_clutter_toplevel_headers, subdir: animation_clutter_headers_subdir)

animation_clutter_sources = animation_clutter_introspectable_sources + animation_clutter_private_sources

glib = dependency('glib-2.0')
gobject = dependency('gobject-2.0')
clutter = dependency('mutter-clutter-7')
mutter = dependency('libmutter-7')

mutter_typelibdir = mutter.get_pkgconfig_variable('typelibdir')
install_rpath = mutter_typelibdir

animation_clutter_lib = shared_library(
  'animation-clutter',
  animation_clutter_sources,
  soversion: api_version,
  install: true,
  include_directories: [ animation_inc ],
  dependencies: [ clutter, glib, gobject, mutter, animation_dep, animation_glib_dep ],
  build_rpath: mutter_typelibdir,
  install_rpath: mutter_typelibdir
)

animation_clutter_dep = declare_dependency(
  link_with: animation_clutter_lib,
  include_directories: [ animation_inc ],
)

introspection_sources = [ animation_clutter_introspectable_sources, animation_clutter_headers ]

gnome = import('gnome')
animation_clutter_gir = gnome.generate_gir(
  animation_clutter_lib,
  extra_args: ['--warn-all', '--warn-error'],
  identifier_prefix: 'AnimationClutter',
  include_directories: animation_inc,
  includes: [animation_glib_gir, 'Clutter-7', 'GLib-2.0', 'GObject-2.0'],
  install: true,
  namespace: 'AnimationClutter',
  nsversion: api_version,
  sources: introspection_sources,
  symbol_prefix: 'animation_clutter'
)[0]

pkg = import('pkgconfig')
pkg.generate(
  description: 'Library to provide 2D surface animations (Clutter Implementation)',
  name: 'libanimation-clutter',
  filebase: 'libanimation-clutter-' + api_version,
  version: meson.project_version(),
  libraries: animation_clutter_lib,
  install_dir: join_paths(get_option('libdir'), 'pkgconfig')
)

