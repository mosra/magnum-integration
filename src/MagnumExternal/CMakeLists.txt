#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
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

# To help Homebrew and other packages, ImGui sources can be cloned to
# src/MagnumExternal/ImGui and we will use those without any extra effort
# from the outside. The *.cpp files from there will get compiled into the
# ImGuiIntegration library and the *.h files will get installed here. The
# FindImGui.cmake module then will look into include/MagnumExternal/ImGui as
# a fallback if IMGUI_DIR is not set and no CMake config file is found.
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ImGui)
    install(FILES
        ImGui/imgui.h
        ImGui/imgui_internal.h
        ImGui/imconfig.h
        ImGui/imstb_textedit.h
        ImGui/imstb_rectpack.h
        ImGui/imstb_truetype.h
        DESTINATION ${MAGNUM_EXTERNAL_INCLUDE_INSTALL_DIR}/ImGui)
endif()
