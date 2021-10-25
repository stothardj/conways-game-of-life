project('gol', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

gtkmm_dep = dependency('gtkmm-3.0')

exe = executable('gol', 'gol.cpp',
  install : true, dependencies: gtkmm_dep)

test('basic', exe)
