#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>
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
set(CMAKE_FOLDER "Magnum/doc/snippets")

# On MSVC remove /W3, as we are replacing it with /W4
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set_directory_properties(PROPERTIES
    CORRADE_CXX_STANDARD 11
    CORRADE_USE_PEDANTIC_FLAGS ON)

if(MAGNUM_WITH_BULLET)
    add_library(snippets-BulletIntegration STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} BulletIntegration.cpp)
    target_link_libraries(snippets-BulletIntegration PRIVATE MagnumBulletIntegration)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-BulletIntegration)
    endif()
endif()

if(MAGNUM_WITH_DART)
    add_library(snippets-DartIntegration STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} DartIntegration.cpp)
    target_link_libraries(snippets-DartIntegration PRIVATE MagnumDartIntegration)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-DartIntegration)
    endif()
endif()

if(MAGNUM_WITH_EIGEN)
    add_library(snippets-EigenIntegration STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} EigenIntegration.cpp)
    target_link_libraries(snippets-EigenIntegration PRIVATE MagnumEigenIntegration)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-EigenIntegration)
    endif()
endif()

if(MAGNUM_WITH_GLM)
    add_library(snippets-GlmIntegration STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} GlmIntegration.cpp)
    target_link_libraries(snippets-GlmIntegration PRIVATE MagnumGlmIntegration)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-GlmIntegration)
    endif()
endif()

if(MAGNUM_WITH_IMGUI)
    add_library(snippets-ImGuiIntegration STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} ImGuiIntegration.cpp)
    target_link_libraries(snippets-ImGuiIntegration PRIVATE MagnumImGuiIntegration)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-ImGuiIntegration)
    endif()

    find_package(Magnum COMPONENTS Sdl2Application QUIET)
    if(Magnum_Sdl2Application_FOUND)
        add_library(snippets-ImGuiIntegration-sdl2 STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} ImGuiIntegration-sdl2.cpp)
        target_link_libraries(snippets-ImGuiIntegration-sdl2 PRIVATE
            MagnumImGuiIntegration
            Magnum::Sdl2Application)
        if(CORRADE_TESTSUITE_TEST_TARGET)
            add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-ImGuiIntegration-sdl2)
        endif()
    endif()
endif()

if(MAGNUM_WITH_OVR)
    add_library(snippets-OvrIntegration STATIC ${EXCLUDE_FROM_ALL_IF_TEST_TARGET} OvrIntegration.cpp)
    target_link_libraries(snippets-OvrIntegration PRIVATE MagnumOvrIntegration)
    if(CORRADE_TESTSUITE_TEST_TARGET)
        add_dependencies(${CORRADE_TESTSUITE_TEST_TARGET} snippets-OvrIntegration)
    endif()
endif()
