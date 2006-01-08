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
      version="0.90.1",
      description="Bindings for TagLib",
      long_description="Binds Python to Scott Wheeler's TagLib. " +
      "Makes it easy to read ID3 tags from MP3 files, "+
      "Xiph Comments from Ogg Vorbis and FLAC files, "+
      "and APE tags from MusePack files.",
      author=u"Andreas Kloeckner",
      author_email="inform@tiker.net",
      classifiers=
      [ "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: BSD License",
        "Natural Language :: English",
        "Operating System :: OS Independent",
        "Operating System :: POSIX",
        "Operating System :: Unix",
        "Programming Language :: Python",
        "Topic :: Multimedia :: Sound/Audio",
        "Topic :: Multimedia :: Sound/Audio :: CD Audio :: CD Ripping",
        "Topic :: Multimedia :: Sound/Audio :: Editors",
        "Topic :: Software Development :: Libraries :: Python Modules",
        "Topic :: Utilities"],
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
