from distutils.core import setup, Extension

extension_mod = Extension("", ["_w2_module.cc", "algorithm-w2.c"])

setup(name = "w2", ext_modules=[extension_mod])
