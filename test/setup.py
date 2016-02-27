from distutils.core import setup, Extension

setup(
    ext_modules=[
        Extension("_dimmer", ["dimmer_module.c"],
                  extra_compile_args=['-DNO_AVR_TEST'],
        )],
    include_dirs=['..'],
)
