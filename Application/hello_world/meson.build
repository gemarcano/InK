project('hello_world_ink', 'c',
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
  '-Wl,--gc-sections', '-fno-exceptions', '-Wl,-Map=output.map',
]

InK = dependency('InK')

sources = files(['appinit.c', 'thread1.c'])
exe = executable('hello_world_ink',
  sources,
  dependencies: InK,
  c_args: c_args,
  link_args: link_args + ['-T' + meson.project_source_root() / 'lnk_apollo3.ld'])

objcopy = find_program('objcopy')

bin = custom_target('hello_world_ink' + '.bin',
  input : exe,
  output : meson.project_name() + '.bin',
  command : [objcopy, '-O', 'binary',
    '@INPUT@', '@OUTPUT@',
  ],
  build_by_default: true
)

svl_tools = find_program('svl-tools')

run_target('flash',
  command : [svl_tools, 'flash', '-p', get_option('tty'), '-f',
    meson.project_name() + '.bin', '-b', '921600', '-v'],
  depends : bin,
)
