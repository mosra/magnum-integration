#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2017 sigman78 <sigman78@gmail.com>
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

find_package(GLM REQUIRED)

if(BUILD_STATIC)
    set(MAGNUM_GLMINTEGRATION_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

set(MagnumGlmIntegration_SRCS
    Integration.cpp)

set(MagnumGlmIntegration_HEADERS
    Integration.h
    GtcIntegration.h
    GtxIntegration.h

    visibility.h)

# GlmIntegration library
add_library(MagnumGlmIntegration ${SHARED_OR_STATIC}
    ${MagnumGlmIntegration_SRCS}
    ${MagnumGlmIntegration_HEADERS})
target_include_directories(MagnumGlmIntegration PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)
set_target_properties(MagnumGlmIntegration PROPERTIES
    DEBUG_POSTFIX "-d"
    FOLDER "Magnum/GlmIntegration")
if(NOT BUILD_STATIC)
    set_target_properties(MagnumGlmIntegration PROPERTIES VERSION ${MAGNUMINTEGRATION_LIBRARY_VERSION} SOVERSION ${MAGNUMINTEGRATION_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumGlmIntegration PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumGlmIntegration PUBLIC Magnum::Magnum GLM::GLM)

install(TARGETS MagnumGlmIntegration
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumGlmIntegration_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/GlmIntegration)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/GlmIntegration)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum GLM integration target alias for superprojects
add_library(MagnumIntegration::Glm ALIAS MagnumGlmIntegration)
