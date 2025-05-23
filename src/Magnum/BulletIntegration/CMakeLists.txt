#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2013 Jan Dupal <dupal.j@gmail.com>
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

# IDE folder in VS, Xcode etc. CMake 3.12+, older versions have only the FOLDER
# property that would have to be set on each target separately.
set(CMAKE_FOLDER "Magnum/BulletIntegration")

find_package(Magnum REQUIRED GL SceneGraph Shaders)

if(NOT MAGNUM_USE_EMSCRIPTEN_PORTS_BULLET)
    find_package(Bullet REQUIRED)
endif()

if(MAGNUM_BUILD_STATIC)
    set(MAGNUM_BULLETINTEGRATION_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

set(MagnumBulletIntegration_SRCS
    DebugDraw.cpp
    MotionState.cpp)

set(MagnumBulletIntegration_HEADERS
    DebugDraw.h
    Integration.h
    MotionState.h

    visibility.h)

# BulletIntegration library
add_library(MagnumBulletIntegration ${SHARED_OR_STATIC}
    ${MagnumBulletIntegration_SRCS}
    ${MagnumBulletIntegration_HEADERS})
target_include_directories(MagnumBulletIntegration PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)
set_target_properties(MagnumBulletIntegration PROPERTIES DEBUG_POSTFIX "-d")
if(NOT MAGNUM_BULLETINTEGRATION_BUILD_STATIC)
    set_target_properties(MagnumBulletIntegration PROPERTIES VERSION ${MAGNUMINTEGRATION_LIBRARY_VERSION} SOVERSION ${MAGNUMINTEGRATION_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumBulletIntegration PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumBulletIntegration PUBLIC
    Magnum::GL
    Magnum::Magnum
    Magnum::SceneGraph
    Magnum::Shaders)

# If we use the Emscripten port, no find_package() was called and the targets
# are not defined.
if(MAGNUM_USE_EMSCRIPTEN_PORTS_BULLET)
    target_compile_options(MagnumBulletIntegration PUBLIC "SHELL:-s USE_BULLET=1")
    target_link_options(MagnumBulletIntegration PUBLIC "SHELL:-s USE_BULLET=1")
else()
    # Interestingly enough, we need just the LinearMath here
    target_link_libraries(MagnumBulletIntegration PUBLIC Bullet::LinearMath)
endif()

install(TARGETS MagnumBulletIntegration
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumBulletIntegration_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/BulletIntegration)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/BulletIntegration)

if(MAGNUM_BUILD_TESTS)
    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum Bullet integration target alias for superprojects
add_library(MagnumIntegration::Bullet ALIAS MagnumBulletIntegration)
