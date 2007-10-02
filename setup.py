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
          version="0.92",
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
