#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
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

find_package(Eigen3 REQUIRED)
# We could drop this once we can use at least 3.3.1 (Ubuntu 16.04 has only 3.3
# beta, which doesn't have this target yet), however for Travis and AppVeyor
# we're using FindEigen3.cmake from the downloaded sources (because the
# Eigen3Config.cmake, which produces the actual targets, is not there -- only
# Eigen3Config.cmake.in). See the YML files for an extended rant. Also,
# FindEigen3 only defines EIGEN3_INCLUDE_DIR, not even EIGEN3_INCLUDE_DIRS, so
# be extra careful. I also wanted to set INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
# of MagnumEigenIntegration to ${EIGEN3_INCLUDE_DIR} to avoid this target
# creation but that was ignored by CMake (?!). This works, tho.
# http://eigen.tuxfamily.org/index.php?title=ChangeLog#Eigen_3.3.1
if(NOT TARGET Eigen3::Eigen)
    add_library(Eigen3::Eigen INTERFACE IMPORTED)
    set_target_properties(Eigen3::Eigen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${EIGEN3_INCLUDE_DIR})
endif()

set(MagnumEigenIntegration_HEADERS
    GeometryIntegration.h
    Integration.h)

# EigenIntegration library
add_library(MagnumEigenIntegration INTERFACE)
set_target_properties(MagnumEigenIntegration PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/src
    INTERFACE_LINK_LIBRARIES "Magnum::Magnum;Eigen3::Eigen")

# Force IDEs to display all header files in project view
add_custom_target(MagnumEigenIntegration-headers SOURCES ${MagnumEigenIntegration_HEADERS})
set_target_properties(MagnumEigenIntegration-headers PROPERTIES FOLDER "Magnum/EigenIntegration")

install(FILES ${MagnumEigenIntegration_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/EigenIntegration)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum Eigen integration target alias for superprojects
add_library(MagnumIntegration::Eigen ALIAS MagnumEigenIntegration)
