project(kicad-doc)

if(WIN32)
    cmake_minimum_required(VERSION 2.6.1 FATAL_ERROR)
else(WIN32)
    cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)
endif(WIN32)

# Locations for install targets.
if(UNIX)
    if(APPLE)
    else(APPLE)
        # Like all variables, CMAKE_INSTALL_PREFIX can be over-ridden on the command line.
        set(CMAKE_INSTALL_PREFIX /usr/local
            CACHE PATH "")

        # Everything without leading / is relative to CMAKE_INSTALL_PREFIX.
        set(KICAD_DOCS share/doc/kicad
            CACHE PATH "Location of KiCad documentation files.")
        set(KICAD_HELP ${KICAD_DOCS}/help
            CACHE PATH "Location of KiCad help files.")
        set(KICAD_INTERNAT share/kicad/internat
            CACHE PATH "Location of KiCad i18n files.")
    endif(APPLE)
endif(UNIX)

if(WIN32)
    # Like all variables, CMAKE_INSTALL_PREFIX can be over-ridden on the command line.
    set(CMAKE_INSTALL_PREFIX c:/kicad
        CACHE PATH "")

    # Everything without leading / is relative to CMAKE_INSTALL_PREFIX.
    set(KICAD_DOCS doc
        CACHE PATH "Location of KiCad documentation files.")
    set(KICAD_HELP ${KICAD_DOCS}/help
        CACHE PATH "Location of KiCad help files.")
    set(KICAD_INTERNAT share/internat
        CACHE PATH "Location of KiCad i18n files.")
endif(WIN32)


mark_as_advanced(KICAD_DOCS
                 KICAD_HELP
                 KICAD_INTERNAT
                 )

# CMake will look in these dirs for nested 'CMakeLists.txt' files.
# Resources.
add_subdirectory(doc/tutorials)
add_subdirectory(doc/help)
add_subdirectory(internat)
