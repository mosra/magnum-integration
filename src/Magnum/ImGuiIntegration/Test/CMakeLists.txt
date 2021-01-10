#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
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

corrade_add_test(ImGuiContextTest ContextTest.cpp
    LIBRARIES MagnumImGuiIntegration)
corrade_add_test(ImGuiIntegrationTest IntegrationTest.cpp
    LIBRARIES MagnumImGuiIntegration)

set_target_properties(
    ImGuiContextTest
    ImGuiIntegrationTest
    PROPERTIES FOLDER "Magnum/ImGuiIntegration/Test")

if(BUILD_GL_TESTS)
    corrade_add_test(ImGuiContextGLTest ContextGLTest.cpp
        LIBRARIES MagnumImGuiIntegration Magnum::OpenGLTester)
    corrade_add_test(ImGuiWidgetsGLTest WidgetsGLTest.cpp
        LIBRARIES MagnumImGuiIntegration Magnum::OpenGLTester)

    set_target_properties(
        ImGuiContextGLTest
        ImGuiWidgetsGLTest
        PROPERTIES FOLDER "Magnum/ImGuiIntegration/Test")
endif()

# GUI test application for quick ability to verify changes w/o having to
# compile examples as well (and to ensure the template APIs don't get out of
# sync with the apps, as the ContextGLTest has only a mock)
find_package(Corrade REQUIRED Main)
if(CORRADE_TARGET_ANDROID)
    find_package(Magnum REQUIRED AndroidApplication)
elseif(CORRADE_TARGET_EMSCRIPTEN)
    find_package(Magnum REQUIRED EmscriptenApplication)
else()
    find_package(Magnum REQUIRED Sdl2Application)
endif()

if(CORRADE_TARGET_ANDROID)
    add_library(ImGuiApplicationTest SHARED ApplicationTest.cpp)
    target_link_libraries(ImGuiApplicationTest PRIVATE Magnum::AndroidApplication)
    if(CMAKE_ANDROID_NDK)
        include(${PROJECT_SOURCE_DIR}/toolchains/modules/UseAndroid.cmake)
        android_create_apk(ImGuiApplicationTest AndroidManifest.xml)
    endif()
else()
    add_executable(ImGuiApplicationTest WIN32 ApplicationTest.cpp)
    if(CORRADE_TARGET_EMSCRIPTEN)
        target_link_libraries(ImGuiApplicationTest PRIVATE Magnum::EmscriptenApplication)
        add_custom_command(TARGET ImGuiApplicationTest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
                ${MAGNUM_WEBAPPLICATION_CSS}
                $<TARGET_FILE_DIR:ImGuiApplicationTest>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTest.html
                $<TARGET_FILE_DIR:ImGuiApplicationTest>/ImGuiApplicationTest.html)
    else()
        target_link_libraries(ImGuiApplicationTest PRIVATE Magnum::Sdl2Application)
        if(CORRADE_TARGET_IOS OR CORRADE_TARGET_APPLE)
            # The plist is needed in order to mark the app as DPI-aware
            set_target_properties(ImGuiApplicationTest PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTest.plist
                MACOSX_BUNDLE ON
                XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
        endif()
        if(CORRADE_TARGET_WINDOWS AND NOT CORRADE_TARGET_WINDOWS_RT)
            if(MSVC)
                target_sources(ImGuiApplicationTest PRIVATE WindowsHiDPI.manifest)
            elseif(MINGW)
                target_sources(ImGuiApplicationTest PRIVATE WindowsHiDPI.rc)
            endif()
        endif()
    endif()
endif()
target_link_libraries(ImGuiApplicationTest PRIVATE
    MagnumImGuiIntegration
    Corrade::Main)
set_target_properties(ImGuiApplicationTest PROPERTIES FOLDER "Magnum/ImGuiIntegration/Test")
