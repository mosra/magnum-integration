#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014
#             Vladimír Vondruš <mosra@centrum.cz>
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

cmake_minimum_required(VERSION 2.8.9)
project(MagnumIntegration)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

find_package(Magnum REQUIRED)

option(WITH_FIND_MODULE "Install FindMagnumIntegration.cmake module into CMake's module dir (might require admin privileges)" OFF)

# Parts of the library
option(WITH_BULLET "Build BulletIntegration library" OFF)

include(CMakeDependentOption)
option(BUILD_STATIC "Build static libraries (default are shared)" OFF)
cmake_dependent_option(BUILD_STATIC_PIC "Build static libraries with position-independent code" OFF "BUILD_STATIC" OFF)
option(BUILD_TESTS "Build unit tests." OFF)

if(BUILD_TESTS)
    enable_testing()
endif()

# Newlib toolchain supports only static linking
if(CORRADE_TARGET_NACL_NEWLIB)
    set(BUILD_STATIC ON)
endif()

add_subdirectory(modules)
add_subdirectory(src)
