from distutils.core import setup, Extension

extension_mod = Extension("", ["w2/_w2_module.cc", "w2/algorithm-w2.c"])

setup(name = "w2", ext_modules=[extension_mod], packages=['w2'])
