#!/usr/bin/env python
# -*- coding: latin-1 -*-

from distutils.core import setup,Extension

# BEGIN USER CUSTOMIZABLE SECTION --------------------
TAGLIB_INCLUDE_DIRS = ["/usr/include/taglib"]
TAGLIB_LIBRARY_DIRS = []
TAGLIB_LIBRARIES = ["tag"]

BOOST_INCLUDE_DIRS = []
BOOST_LIBRARY_DIRS = []
BOOST_LIBRARIES = ["boost_python"]
# END USER CUSTOMIZABLE SECTION ----------------------

INCLUDE_DIRS = TAGLIB_INCLUDE_DIRS + BOOST_INCLUDE_DIRS
LIBRARY_DIRS = TAGLIB_LIBRARY_DIRS + BOOST_LIBRARY_DIRS
LIBRARIES = TAGLIB_LIBRARIES + BOOST_LIBRARIES
EXTRA_COMPILE_ARGS = []

setup(name="tagpy",
      version="0.90",
      description="TagLib bindings",
      author=u"Andreas Kloeckner",
      author_email="inform@tiker.net",
      license = "BSD-Style",
      url="http://news.tiker.net/software/tagpy",
      packages=["tagpy"],
      ext_modules=[ Extension("_tagpy", 
                              ["src/basics.cpp", 
                               "src/id3.cpp",
                               "src/rest.cpp"],
                              include_dirs = INCLUDE_DIRS,
                              library_dirs = LIBRARY_DIRS,
                              libraries = LIBRARIES,
                              extra_compile_args = EXTRA_COMPILE_ARGS,
                              ),
                    ]
      )
