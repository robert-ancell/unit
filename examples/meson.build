executable('http',
           'http.c',
           include_directories: ut_include,
           link_with: ut_lib)

executable('png2pnm',
           'png2pnm.c',
           include_directories: ut_include,
           link_with: ut_lib)

executable('tiff2pnm',
           'tiff2pnm.c',
           include_directories: ut_include,
           link_with: ut_lib)

executable('jpeg2pnm',
           'jpeg2pnm.c',
           include_directories: ut_include,
           link_with: ut_lib)

executable('gif2pnm',
           'gif2pnm.c',
           include_directories: ut_include,
           link_with: ut_lib)

executable('x11',
           'x11.c',
           include_directories: ut_include,
           link_with: ut_lib)

executable('wayland',
           'wayland.c',
           dependencies: [gbm_dep, egl_dep, gl_dep],
           include_directories: ut_include,
           link_with: ut_lib)
