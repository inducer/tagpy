#!/usr/bin/env python
# -*- coding: latin-1 -*-

def get_config_schema():
    from aksetup_helper import ConfigSchema, Option, \
            IncludeDir, LibraryDir, Libraries, \
            Switch, StringListOption

    return ConfigSchema([
        IncludeDir("BOOST", []),
        LibraryDir("BOOST", []),
        Libraries("BOOST_PYTHON", ["boost_python-gcc42-mt"]),

        IncludeDir("TAGLIB", []),
        LibraryDir("TAGLIB", []),
        Libraries("TAGLIB", ["tag"]),

        StringListOption("CXXFLAGS", [], 
            help="Any extra C++ compiler options to include"),
        ])




def main():
    import glob
    from aksetup_helper import hack_distutils, get_config, setup, Extension

    hack_distutils()
    conf = get_config()

    INCLUDE_DIRS = conf["TAGLIB_INC_DIR"] + conf["BOOST_INC_DIR"]
    LIBRARY_DIRS = conf["TAGLIB_LIB_DIR"] + conf["BOOST_LIB_DIR"]
    LIBRARIES = conf["TAGLIB_LIBNAME"] + conf["BOOST_PYTHON_LIBNAME"]

    setup(name="tagpy",
          version="0.94.2",
          description="Python Bindings for TagLib",
          long_description="""
          TagPy is a set of Python bindings for Scott Wheeler's 
          `TagLib <http://developer.kde.org/~wheeler/taglib.html>`_. 
          It builds upon `Boost.Python <http://www.boost.org/libs/python/doc/>`_, 
          a wrapper generation library which is part of the renowned Boost 
          set of C++ libraries.

          Just like TagLib, TagPy can:

          * read and write ID3 tags of version 1 and 2, with many supported frame types
            for version 2 (in MPEG Layer 2 and MPEG Layer 3, FLAC and MPC),
          * access Xiph Comments in Ogg Vorbis Files and Ogg Flac Files,
          * access APE tags in Musepack and MP3 files.

          All these features have their own specific interfaces, but 
          TagLib's generic tag reading and writing mechanism is also 
          supported. It comes with a bunch of examples.
          """,
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

          url="http://mathema.tician.de/software/tagpy",
          packages=["tagpy", "tagpy.ogg"],
          package_dir={"tagpy": "src/python"},
          ext_modules=[ Extension("_tagpy", 
                                  ["src/wrapper/basics.cpp", 
                                   "src/wrapper/id3.cpp",
                                   "src/wrapper/rest.cpp"],
                                  include_dirs=INCLUDE_DIRS,
                                  library_dirs=LIBRARY_DIRS,
                                  libraries=LIBRARIES,
                                  extra_compile_args=conf["CXXFLAGS"],
                                  ),
                        ]
          )




if __name__ == '__main__':
    main()
