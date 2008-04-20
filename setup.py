#!/usr/bin/env python
# -*- coding: latin-1 -*-

from distutils.core import setup,Extension


def main():
    from distutils.core import setup, Extension
    from distutils import sysconfig

    def non_matching_config():
        print "*** The version of your configuration template does not match"
        print "*** the version of the setup script. Please re-run configure."
        sys.exit(1)

    try:
        conf = {}
        execfile("siteconf.py", conf)
    except:
        print "*** Please run configure first."
        sys.exit(1)

    if "TAGPY_CONF_TEMPLATE_VERSION" not in conf:
        non_matching_config()

    if conf["TAGPY_CONF_TEMPLATE_VERSION"] != 1:
        non_matching_config()

    INCLUDE_DIRS = conf["TAGLIB_INCLUDE_DIRS"] + conf["BOOST_INCLUDE_DIRS"]
    LIBRARY_DIRS = conf["TAGLIB_LIBRARY_DIRS"] + conf["BOOST_LIBRARY_DIRS"]
    LIBRARIES = conf["TAGLIB_LIBRARIES"] + conf["BOOST_LIBRARIES"]
    EXTRA_COMPILE_ARGS = []

    setup(name="tagpy",
          version="0.94",
          description="Python Bindings for TagLib",
          long_description="
          TagPy is a set of Python bindings for Scott Wheeler's 
          `TagLib <http://developer.kde.org/~wheeler/taglib.html>`_. 
          It builds upon `Boost.Python <http://www.boost.org/libs/python/doc/>_, 
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
          ",
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
          packages=["tagpy", "tagpy.ogg"],
          package_dir={"tagpy": "src/python"},
          ext_modules=[ Extension("_tagpy", 
                                  ["src/wrapper/basics.cpp", 
                                   "src/wrapper/id3.cpp",
                                   "src/wrapper/rest.cpp"],
                                  include_dirs = INCLUDE_DIRS,
                                  library_dirs = LIBRARY_DIRS,
                                  libraries = LIBRARIES,
                                  extra_compile_args = EXTRA_COMPILE_ARGS,
                                  ),
                        ]
          )




if __name__ == '__main__':
    # hack distutils.sysconfig to eliminate debug flags
    # stolen from mpi4py
    import sys
    if not sys.platform.lower().startswith("win"):
        from distutils import sysconfig

        cvars = sysconfig.get_config_vars()
        cflags = cvars.get('OPT')
        if cflags:
            cflags = cflags.split()
            for bad_prefix in ('-g', '-O', '-Wstrict-prototypes'):
                for i, flag in enumerate(cflags):
                    if flag.startswith(bad_prefix):
                        cflags.pop(i)
                        break
                if flag in cflags:
                    cflags.remove(flag)
            cflags.append("-O3")
            cvars['OPT'] = str.join(' ', cflags)
            cvars["CFLAGS"] = cvars["BASECFLAGS"] + " " + cvars["OPT"]
    # and now call main
    main()
