#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025, 2026
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018, 2019 Konstantinos Chatzilygeroudis <costashatz@gmail.com>
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

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/DartIntegration/Test")

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(DARTINTEGRATION_TEST_DIR ".")
else()
    set(DARTINTEGRATION_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# This, and all subsequent checks of the same variable, assumes that Magnum
# itself is built as static and plugins in turn as well. If only the UI library
# is built as static (i.e., MAGNUM_UI_BUILD_STATIC is ON but
# MAGNUM_BUILD_STATIC is OFF), then the plugins are assumed to be dynamic and
# thus don't need to be linked.
if(MAGNUM_BUILD_STATIC)
    find_package(Magnum COMPONENTS AnyImageImporter)
    find_package(MagnumPlugins COMPONENTS AssimpImporter)
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

corrade_add_test(DartIntegrationConvertShapeNodeTest
    ConvertShapeNodeTest.cpp common.h
    LIBRARIES MagnumDartIntegration)
if(DART_utils-urdf_FOUND)
    target_link_libraries(DartIntegrationConvertShapeNodeTest PRIVATE dart-utils-urdf)
elseif(DART_io-urdf_FOUND)
    target_link_libraries(DartIntegrationConvertShapeNodeTest PRIVATE dart-io-urdf)
endif()
if(MAGNUM_BUILD_STATIC)
    if(Magnum_AnyImageImporter_FOUND)
        target_link_libraries(DartIntegrationConvertShapeNodeTest PRIVATE Magnum::AnyImageImporter)
    endif()
    if(MagnumPlugins_AssimpImporter_FOUND)
        target_link_libraries(DartIntegrationConvertShapeNodeTest PRIVATE MagnumPlugins::AssimpImporter)
    endif()
endif()

if(MAGNUM_BUILD_GL_TESTS)
    corrade_add_test(DartIntegrationWorldGLTest
        WorldGLTest.cpp common.h
        LIBRARIES Magnum::OpenGLTester MagnumDartIntegration)
    if(DART_utils-urdf_FOUND)
        target_link_libraries(DartIntegrationWorldGLTest PRIVATE dart-utils-urdf)
    elseif(DART_io-urdf_FOUND)
        target_link_libraries(DartIntegrationWorldGLTest PRIVATE dart-io-urdf)
    endif()
    if(MAGNUM_BUILD_STATIC)
        if(Magnum_AnyImageImporter_FOUND)
            target_link_libraries(DartIntegrationWorldGLTest PRIVATE Magnum::AnyImageImporter)
        endif()
        if(MagnumPlugins_AssimpImporter_FOUND)
            target_link_libraries(DartIntegrationWorldGLTest PRIVATE MagnumPlugins::AssimpImporter)
        endif()
    endif()
endif()
