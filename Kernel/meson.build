project('InK', 'c',
  default_options: [
    'b_lto=true',
    'b_staticpic=false',
    'c_std=c23',
    'warning_level=3',
  ],
  license: ['LGPL-3.0'],
  version: '0.1.0')

c_args = [
  '-ffunction-sections',
]

link_args = [
  '-Wl,--gc-sections', '-fno-exceptions',
]


# Find libm...
cc = meson.get_compiler('c', native: false)
m_dep = cc.find_library('m', required : false)

# library
lib_sources = files([
  './main.c',
  './isr/isrmanager.c',
  './isr/persistentqueue.c',
  './mcu/apollo3/apollo3.c',
  './scheduler/priority.c',
  './scheduler/scheduler.c',
  './scheduler/thread.c',
  './timer/timer.c',
  './timer/persistent_timer_commit/persistent_timer.c',
])

apollo_sources = files([
  './mcu/apollo3/apollo3.c',
  './timer/mcu_specifics/apollo3_clk.c',
])

msp_sources = files([
  './mcu/msp430/dma.c',
  './mcu/msp430/msp430fr5969.c',
  './timer/mcu_specifics/msp430_clk.c',
  './timer/mcu_specifics/msp430_timer.c',
])

ambiq_lib = dependency('ambiq_rba_atp')
asimple_lib = dependency('asimple_rba_atp')

includes = include_directories([
  'mcu/',
  'isr/',
  'scheduler/',
  'timer/'
])

lib = library(meson.project_name(),
  lib_sources + apollo_sources,
  dependencies: [m_dep, ambiq_lib, asimple_lib],
  include_directories: includes,
  c_args: c_args,
  link_args: link_args,
  install: true,
)

headers = files([
  'channel/channel.h',
  'ink.h',
  'isr/isr.h',
  'isr/isrmanager.h',
  'isr/persistentqueue.h',
  'mcu/mcu.h',
  'mcu/msp430/fram.h',
  'mcu/msp430/msp430fr5969.h',
  'mcu/nv.h',
  'scheduler/priority.h',
  'scheduler/scheduler.h',
  'scheduler/task.h',
  'scheduler/thread.h',
  'timer/mcu_specifics/clk.h',
  'timer/persistent_timer_commit/persistent_timer.h',
  'timer/timer.h',
])

install_headers(headers, preserve_path: true, subdir: 'InK')

#install_subdir('include', install_dir: '')

# Create a pkgconfig file
pkg = import('pkgconfig')
pkg.generate(lib, subdirs: ['InK'])

system = 'none'
cpu_family = 'arm'
cpu = 'cortex-m4'
endian = 'little'

# Only create the executable if the target is something resembling the RedBoard
#if (host_machine.system() == 'none' and host_machine.cpu_family() == 'arm' and
#    host_machine.cpu() == 'cortex-m4')
#  # Executable
#  sources = files([
#    'src/main.c',
#  ])
#
#  ambiq_lib = dependency('ambiq_rba_atp')
#  asimple_lib = dependency('asimple_rba_atp')
#
#
#  exe = executable(meson.project_name(),
#    sources,
#    link_with: lib,
#    dependencies: [ambiq_lib, m_dep, asimple_lib],
#    include_directories: includes,
#    c_args: c_args,
#    link_args: link_args + ['-T' + meson.source_root() / 'linker.ld']
#  )
#
#  # Custom flash target, to make it easier to program the RedBoard, requires
#  # objcopy
#  objcopy = find_program('objcopy')
#
#  bin = custom_target(
#    input : exe,
#    output : exe.name().split('.')[0] + '.bin',
#    command : [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@', ],
#    build_by_default: true
#  )
#
#  run_target('flash',
#    command : ['python3', meson.source_root() / 'svl.py',
#      get_option('tty'), '-f',  bin, '-b', '921600', '-v'],
#    depends : bin,
#  )
#endif
