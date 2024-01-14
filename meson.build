project('katzenwl', 'c',
  version: '0.1.0',
  license: 'The 3-Clause BSD License',
    default_options: [
    'c_std=c11',
    'warning_level=1',
    'werror=true'
  ],
)

wayland_deps = dependency('wayland-server')
xcb_deps = dependency('xcb')


includes = include_directories('includes')

src_files = [ 'src/backends/backend.c',
  'src/backends/xcb/xcb.c',
  'src/logger/log.c',
  'src/allocator/allocator.c',
  'src/allocator/shm/shm.c'
  ]

deps = [ wayland_deps, xcb_deps ]

kwllib = shared_library(meson.project_name(), src_files, dependencies: deps, include_directories: includes, soversion: '0', version: meson.project_version())

executable('server-example', 'examples/server.c',
  link_with: kwllib, include_directories: includes,
  dependencies: wayland_deps)
