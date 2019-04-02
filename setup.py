from distutils.core import setup, Extension

extension_mod = Extension("_w2", ["_w2.c", "w2/w2.c"])

setup(name = "w2", ext_modules=[extension_mod], packages=['w2'])
