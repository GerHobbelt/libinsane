project('libinsane', 'c',
    version: '1.0.10',
    license: 'LGPLv3+',
    meson_version: '>=0.37.1')

if host_machine.system() == 'windows'
    message('Target OS: Microsoft Windows (Mingw)')
    add_project_arguments(
        '-DOS_WINDOWS',
        '-DCOMPILER_GCC',
        language: 'c'
    )
elif host_machine.system() == 'darwin'
    message('Target OS: Apple macOS')
    add_project_arguments(
        '-DOS_MACOS',
        '-DCOMPILER_GCC',
        language: 'c'
    )
else
    message('Target OS: Unix-like')
    add_project_arguments(
        '-DOS_LINUX',
        '-DCOMPILER_GCC',
        language: 'c'
    )
endif

subdir('include')
subdir('src')
subdir('bmp')

subdir('doc')
subdir('examples')
subdir('tests')
