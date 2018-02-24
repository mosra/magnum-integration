#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018 Konstantinos Chatzilygeroudis <costashatz@gmail.com>
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

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(DARTINTEGRATION_TEST_DIR ".")
else()
    set(DARTINTEGRATION_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

find_package(DART 6.0.0 CONFIG REQUIRED OPTIONAL_COMPONENTS io-urdf utils-urdf)

# The DART URDF library is only needed for the tests
if(DART_utils-urdf_FOUND OR DART_io-urdf_FOUND)
    set(MAGNUM_DART_URDF_FOUND 1)
else()
    set(MAGNUM_DART_URDF_FOUND 0)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

corrade_add_test(DartIntegrationConvertShapeNodeTest ConvertShapeNodeTest.cpp LIBRARIES MagnumDartIntegration ${DART_LIBRARIES})

set_target_properties(
    DartIntegrationConvertShapeNodeTest
    PROPERTIES FOLDER "Magnum/DartIntegration/Test")

if(BUILD_GL_TESTS)
    corrade_add_test(DartIntegrationWorldGLTest WorldGLTest.cpp LIBRARIES Magnum::OpenGLTester MagnumDartIntegration ${DART_LIBRARIES})

    set_target_properties(
        DartIntegrationWorldGLTest
        PROPERTIES FOLDER "Magnum/DartIntegration/Test")
endif()
