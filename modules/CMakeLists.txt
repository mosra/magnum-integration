#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
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

set(MagnumIntegration_MODULES
    FindMagnumIntegration.cmake
    MagnumIntegrationConfig.cmake)

# IMPORTANT: When adding a new module here, be sure to update the
# find_path(_MAGNUMINTEGRATION_DEPENDENCY_MODULE_DIR ...) list in
# FindMagnumIntegration.cmake to avoid breakages when the directory contains
# only that new module.
set(MagnumIntegration_DEPENDENCY_MODULES )
if(MAGNUM_WITH_BULLET)
    list(APPEND MagnumIntegration_DEPENDENCY_MODULES
        FindBullet.cmake)
endif()
if(MAGNUM_WITH_GLM)
    list(APPEND MagnumIntegration_DEPENDENCY_MODULES
        FindGLM.cmake)
endif()
if(MAGNUM_WITH_IMGUI)
    list(APPEND MagnumIntegration_DEPENDENCY_MODULES
        FindImGui.cmake)
endif()
if(MAGNUM_WITH_OVR)
    list(APPEND MagnumIntegration_DEPENDENCY_MODULES
        FindOVR.cmake)
endif()

install(FILES ${MagnumIntegration_MODULES} DESTINATION ${MAGNUMINTEGRATION_CMAKE_MODULE_INSTALL_DIR})
if(MagnumIntegration_DEPENDENCY_MODULES)
    install(FILES ${MagnumIntegration_DEPENDENCY_MODULES} DESTINATION ${MAGNUMINTEGRATION_CMAKE_MODULE_INSTALL_DIR}/dependencies)
endif()

# Magnum Integration dependency module dir for superprojects
set(_MAGNUMINTEGRATION_DEPENDENCY_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
