#.rst:
# Find GLM
# -------------
#
# Finds the GLM library. This module defines:
#
#  GLM_FOUND                - True if GLM library is found
#  GLM::GLM                 - GLM imported target
#
# Additionally these variables are defined for internal usage:
#
#  GLM_INCLUDE_DIR          - Include dir
#

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

# Include dir
find_path(GLM_INCLUDE_DIR
    NAMES glm/glm.hpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM DEFAULT_MSG
    GLM_INCLUDE_DIR)

mark_as_advanced(FORCE GLM_INCLUDE_DIR)

if(NOT TARGET GLM::GLM)
    add_library(GLM::GLM INTERFACE IMPORTED)
    set_target_properties(GLM::GLM PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${GLM_INCLUDE_DIR})
endif()
