#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025, 2026
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

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/YogaIntegration")

find_package(MagnumExtras REQUIRED Ui)
# Since 2.0.0 the project provides a CMake config file, force it. Before 2.0 it
# didn't even have an install target, so assume those versions just aren't used
# at all.
find_package(yoga CONFIG REQUIRED)

if(MAGNUM_BUILD_STATIC)
    set(MAGNUM_YOGAINTEGRATION_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

# Use try_compile() to figure out at least a rough Yoga version in order to
# apply various workarounds or at least have XFAILs in tests. Because of course
# this damn thing exposes no version macro anywhere, nor it encodes the version
# in any other distributed file because META doesn't give a fuck, they can pin
# to the latest version as easily as they can require use of janky C++20 to
# build the damn thing. So I have to do a similar nasty compile-fail-compile
# loop as with AssimpImporter, where I painstakingly plow through diffs between
# versions to find something that differs that I could hook to. Fortunately I
# only need to know just a few versions so far.
foreach(_version 302000 300000 200001)
    try_compile(_works ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/checkYogaVersion.cpp
        LINK_LIBRARIES yoga::yogacore
        COMPILE_DEFINITIONS
            -DCHECK_VERSION=${_version}
            # The damn thing uses constexpr when building for C++, so have to
            # enable C++11. Could also build as C, but right now the top-level
            # CMake project() call enables just CXX because it makes the
            # initial CMake run twice as fast as it only checks one compiler
            # and not two.
            ${CORRADE_CXX11_STANDARD_FLAG}
        OUTPUT_VARIABLE _version_output)
    if(_works)
        set(YOGA_VERSION ${_version})
        break()
    endif()
endforeach()
if(NOT YOGA_VERSION)
    message(SEND_ERROR "Could not detect Yoga version. Maybe even older than 2.0.1? ${_version_output}")
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configureInternal.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configureInternal.h)

set(MagnumYogaIntegration_GracefulAssert_SRCS
    Layouter.cpp)

set(MagnumYogaIntegration_HEADERS
    Layouter.h
    YogaIntegration.h

    visibility.h)

# YogaIntegration library
add_library(MagnumYogaIntegration ${SHARED_OR_STATIC}
    ${MagnumYogaIntegration_GracefulAssert_SRCS}
    ${MagnumYogaIntegration_HEADERS})
target_include_directories(MagnumYogaIntegration PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)
set_target_properties(MagnumYogaIntegration PROPERTIES DEBUG_POSTFIX "-d")
if(NOT MAGNUM_BULLETINTEGRATION_BUILD_STATIC)
    set_target_properties(MagnumYogaIntegration PROPERTIES VERSION ${MAGNUMINTEGRATION_LIBRARY_VERSION} SOVERSION ${MAGNUMINTEGRATION_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumYogaIntegration PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumYogaIntegration PUBLIC
    MagnumExtras::Ui
    yoga::yogacore)

install(TARGETS MagnumYogaIntegration
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumYogaIntegration_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/YogaIntegration)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/YogaIntegration)

if(MAGNUM_BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumYogaIntegrationTestLib ${SHARED_OR_STATIC}
        ${MagnumYogaIntegration_GracefulAssert_SRCS})
    target_include_directories(MagnumYogaIntegrationTestLib PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src)
    set_target_properties(MagnumYogaIntegrationTestLib PROPERTIES DEBUG_POSTFIX "-d")
    target_compile_definitions(MagnumYogaIntegrationTestLib PRIVATE
        "CORRADE_GRACEFUL_ASSERT" "MagnumYogaIntegration_EXPORTS")
    if(MAGNUM_BUILD_STATIC_PIC)
        set_target_properties(MagnumYogaIntegrationTestLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    target_link_libraries(MagnumYogaIntegrationTestLib PUBLIC
        MagnumExtras::Ui
        yoga::yogacore)

    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum Yoga integration target alias for superprojects
add_library(MagnumIntegration::Yoga ALIAS MagnumYogaIntegration)
