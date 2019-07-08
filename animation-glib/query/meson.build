# /animation-glib/query/meson.build
#
# Build the libanimation library (box query GLib binding component).
#
# See /LICENCE.md for Copyright information.

geometry_query_introspectable_sources = files([
  'geometry-query.cpp'
])

geometry_query_headers = files([
  'geometry-query.h'
])

geometry_query_private_headers = files([
  'geometry-query-private.h'
])

geometry_query_private_sources = files([
  'geometry-query-internal.cpp'
])

animation_glib_introspectable_sources += geometry_query_introspectable_sources
animation_glib_headers += geometry_query_headers
animation_glib_private_headers += geometry_query_private_headers
animation_glib_private_sources += geometry_query_private_sources

install_headers(magiclamp_headers, subdir: join_paths(animation_glib_headers_subdir, 'query'))
