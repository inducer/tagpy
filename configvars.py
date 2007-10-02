vars = [
    ("BOOST_INC_DIR", None,
        "The include directory for all of Boost C++"),
    ("BOOST_LIB_DIR", None,
        "The library directory for all of Boost C++"),
    ("BOOST_PYTHON_LIB_NAMES", "boost_python-gcc42-mt",
        "The name of the Boost Python library binary (without lib and .so)"),
    # -------------------------------------------------------------------------
    ("TAGLIB_INC_DIR", None,
        "Include directory for TagLib"),
    ("TAGLIB_LIB_DIR", None,
        "Library directory for TagLib"),
    ("TAGLIB_LIB_NAMES", "tag",
        "Library names for TagLib, comma-separated"),
    # -------------------------------------------------------------------------
    ("CXXFLAGS", None,
        "Any extra C++ compiler options to include"),
    ]
subst_files = ["siteconf.py", "Makefile"]
