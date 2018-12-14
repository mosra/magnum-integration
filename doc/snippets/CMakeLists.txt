#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

set_directory_properties(PROPERTIES
    CORRADE_CXX_STANDARD 11
    CORRADE_USE_PEDANTIC_FLAGS ON)

if(WITH_BULLET)
    add_library(snippets-BulletIntegration STATIC BulletIntegration.cpp)
    target_link_libraries(snippets-BulletIntegration PRIVATE MagnumBulletIntegration)
    set_target_properties(snippets-BulletIntegration PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

if(WITH_DART)
    add_library(snippets-DartIntegration STATIC DartIntegration.cpp)
    target_link_libraries(snippets-DartIntegration PRIVATE MagnumDartIntegration)
    set_target_properties(snippets-DartIntegration PROPERTIES FOLDER "Magnum/doc/snippets")
endif()

if(WITH_GLM)
    add_library(snippets-GlmIntegration STATIC GlmIntegration.cpp)
    target_link_libraries(snippets-GlmIntegration PRIVATE MagnumGlmIntegration)
    set_target_properties(snippets-GlmIntegration PROPERTIES FOLDER "Magnum/doc/snippets")
endif()
