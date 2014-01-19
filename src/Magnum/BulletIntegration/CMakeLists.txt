#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2013 Jan Dupal <dupal.j@gmail.com>
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

find_package(Magnum REQUIRED SceneGraph Shapes)
find_package(Bullet REQUIRED)

# Silence pedantic warnings in Bullet headers
include_directories(SYSTEM ${BULLET_INCLUDE_DIRS})

set(MagnumBulletIntegration_SRCS
    ConvertShape.cpp
    MotionState.cpp)

set(MagnumBulletIntegration_HEADERS
    ConvertShape.h
    Integration.h
    MotionState.h

    visibility.h)

add_library(MagnumBulletIntegration SHARED ${MagnumBulletIntegration_SRCS})
target_link_libraries(MagnumBulletIntegration
    ${MAGNUM_LIBRARIES}
    ${MAGNUM_SCENEGRAPH_LIBRARIES}
    ${MAGNUM_SHAPES_LIBRARIES}
    ${BULLET_LIBRARIES})

install(TARGETS MagnumBulletIntegration DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumBulletIntegration_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/BulletIntegration)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
