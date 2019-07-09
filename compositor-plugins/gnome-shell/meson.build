# /compositor-plugins/gnome-shell/meson.build
#
# Install the GNOME-Shell extension.
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

datadir = get_option('datadir')

extension_name = 'animation@endlessm.com'

extension_files = files([
  'extension.js',
  'metadata.json',
  'stylesheet.css'
])

schemas_files = files([
  'org.gnome.shell.extensions.animation.gschema.xml'
])

install_data(
  schemas_files,
  install_dir: join_paths(datadir, 'glib-2.0', 'schemas')
)

install_data(
  extension_files,
  install_dir: join_paths(datadir, 'gnome-shell', 'extensions', extension_name)
)

