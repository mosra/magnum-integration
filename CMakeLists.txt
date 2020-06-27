#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018 Konstantinos Chatzilygeroudis <costashatz@gmail.com>
#   Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
#   Copyright © 2019 bowling-allie <allie.smith.epic@gmail.com>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

cmake_minimum_required(VERSION 3.4)

project(MagnumIntegration CXX)

# Use folders for nice tree in Visual Studio and XCode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})
find_package(Magnum REQUIRED)

# Parts of the library
option(WITH_BULLET "Build BulletIntegration library" OFF)
option(WITH_DART "Build DartIntegration library" OFF)
option(WITH_EIGEN "Build EigenIntegration library" OFF)
option(WITH_GLM "Build GlmIntegration library" OFF)
option(WITH_IMGUI "Build ImGuiIntegration library" OFF)
option(WITH_OVR "Build OvrIntegration library" OFF)

# Library-specific options. This one uses the `SHELL:` option from CMake 3.12
# and target_link_options() from 3.13 in order to pass `-s USE_BULLET=1` to
# both the compiler and linker without it being split into two parts and passed
# through an unique filter.
if(CORRADE_TARGET_EMSCRIPTEN AND NOT CMAKE_VERSION VERSION_LESS 3.13)
    option(USE_EMSCRIPTEN_PORTS_BULLET "Use Bullet from emscripten-ports instead of looking for it elsewhere (warning: likely an outdated version)" OFF)
    if(USE_EMSCRIPTEN_PORTS_BULLET)
        set(MAGNUM_USE_EMSCRIPTEN_PORTS_BULLET 1)
    endif()
else()
    if(USE_EMSCRIPTEN_PORTS_BULLET)
        message(FATAL_ERROR "The USE_EMSCRIPTEN_PORTS_BULLET option is only for Emscripten builds and CMake 3.13+")
    endif()
endif()

include(CMakeDependentOption)
option(BUILD_STATIC "Build static libraries (default are shared)" OFF)
# Disable PIC on Emscripten by default (but still allow it to be enabled
# explicitly if one so desires). Currently causes linker errors related to
# __memory_base etc.: https://github.com/emscripten-core/emscripten/issues/8761
if(CORRADE_TARGET_EMSCRIPTEN)
    set(ON_EXCEPT_EMSCRIPTEN OFF)
else()
    set(ON_EXCEPT_EMSCRIPTEN ON)
endif()
cmake_dependent_option(BUILD_STATIC_PIC "Build static libraries with position-independent code" ${ON_EXCEPT_EMSCRIPTEN} "BUILD_STATIC" OFF)
option(BUILD_TESTS "Build unit tests" OFF)
cmake_dependent_option(BUILD_GL_TESTS "Build unit tests for OpenGL code" OFF "BUILD_TESTS" OFF)

if(BUILD_TESTS)
    find_package(Corrade REQUIRED TestSuite)
    if(CORRADE_TARGET_IOS)
        set(CORRADE_TESTSUITE_BUNDLE_IDENTIFIER_PREFIX "cz.mosra.magnum-integration")
    endif()
    enable_testing()
endif()

if(BUILD_GL_TESTS)
    find_package(Magnum REQUIRED OpenGLTester)
endif()

# Dynamic linking is meaningless on Emscripten and too inconvenient on Android
if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(BUILD_STATIC ON)
endif()

if(NOT BUILD_STATIC)
    set(SHARED_OR_STATIC SHARED)
else()
    set(SHARED_OR_STATIC STATIC)
endif()

set(MAGNUMINTEGRATION_CMAKE_MODULE_INSTALL_DIR share/cmake/MagnumIntegration)

# Library version. MAGNUMINTEGRATION_VERSION_YEAR/MONTH is used in
# src/Magnum/CMakeLists.txt to generate the versionIntegration.h header.
set(MAGNUMINTEGRATION_LIBRARY_VERSION 2.4)
set(MAGNUMINTEGRATION_LIBRARY_SOVERSION 2)
set(MAGNUMINTEGRATION_VERSION_YEAR 2020)
set(MAGNUMINTEGRATION_VERSION_MONTH 6)

# A single output location. After a decade of saying NO THIS IS A NON-SOLUTION
# TO A NON-PROBLEM I reconsidered my views and enabled this, because:
#
# - On Windows (which don't have RPATH), this makes test execution finally
#   possible without having to install all the stuff first (including the
#   test-only libs, which is ugh).
# - With CMake subprojects, this makes it finally possible to use dynamic
#   plugins directly from the build dir (again without installing anything) ---
#   all plugins are put into the same place, so PluginManager has a single
#   place to look into; and thanks to the dynamic libraries being there as
#   well, this location can be automagically detected as relative to
#   Directory::libraryLocation().
# - Thanks to the $<CONFIG> being part of the output path, you are always sure
#   you never accidentally mix up debug/release libraries when switching
#   CMAKE_BUILD_TYPE in an existing build dir.
#
# The runtime location is set to CMAKE_BINARY_DIR and not PROJECT_BINARY_DIR
# because have one runtime location per CMake subproject would not solve much
# either. If the user already provides CMAKE_RUNTIME_OUTPUT_DIRECTORY (even
# empty), it's respected and nothing is being done.
#
# Explicitly using a generator expression to ensure plugins are added to e.g.
# <CONFIG>/lib/magnum/importers/ instead of lib/magnum/importers/<CONFIG>. Also
# adding this to cache, making superprojects pick that up implicitly as well,
# without forcing them to explicitly mirror this setting.
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY AND NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY AND NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib CACHE PATH "" FORCE)
endif()

add_subdirectory(modules)
add_subdirectory(src)

# Build snippets as part of testing
if(BUILD_TESTS)
    add_subdirectory(doc/snippets)
endif()
