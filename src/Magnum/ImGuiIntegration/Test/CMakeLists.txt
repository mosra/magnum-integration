#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025, 2026
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
#   Copyright © 2022 Pablo Escobar <mail@rvrs.in>
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
set(CMAKE_FOLDER "Magnum/ImGuiIntegration/Test")

corrade_add_test(ImGuiIntegrationContextTest ContextTest.cpp
    LIBRARIES MagnumImGuiIntegration)
corrade_add_test(ImGuiIntegrationTest IntegrationTest.cpp
    LIBRARIES MagnumImGuiIntegration)

corrade_add_test(ImGuiIntegrationUserConfigTest UserConfigTest.cpp
    LIBRARIES MagnumImGuiIntegration)
target_compile_definitions(ImGuiIntegrationUserConfigTest PRIVATE
    MAGNUM_IMGUIINTEGRATION_USER_CONFIG="Magnum/ImGuiIntegration/Test/UserConfigTest.h")

if(MAGNUM_BUILD_GL_TESTS)
    find_package(Corrade REQUIRED PluginManager)
    find_package(Magnum REQUIRED Trade DebugTools)

    if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
        set(IMGUIINTEGRATION_TEST_DIR ".")
    else()
        set(IMGUIINTEGRATION_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
                   ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

    corrade_add_test(ImGuiIntegrationContextGLTest ContextGLTest.cpp
        LIBRARIES
            MagnumImGuiIntegration
            Magnum::Trade
            Magnum::DebugTools
            Magnum::OpenGLTester
        FILES
            ContextTestFiles/draw.png
            ContextTestFiles/draw-scissor.png
            ContextTestFiles/draw-texture.png
            ContextTestFiles/texture.png)
    target_include_directories(ImGuiIntegrationContextGLTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    if(MAGNUM_IMGUIINTEGRATION_BUILD_STATIC)
        # Not required
        find_package(Magnum COMPONENTS AnyImageImporter)
        find_package(MagnumPlugins COMPONENTS StbImageImporter)
        if(Magnum_AnyImageImporter_FOUND)
            target_link_libraries(ImGuiIntegrationContextGLTest PRIVATE Magnum::AnyImageImporter)
        endif()
        if(MagnumPlugins_StbImageImporter_FOUND)
            target_link_libraries(ImGuiIntegrationContextGLTest PRIVATE MagnumPlugins::StbImageImporter)
        endif()
    endif()

    corrade_add_test(ImGuiIntegrationWidgetsGLTest WidgetsGLTest.cpp
        LIBRARIES MagnumImGuiIntegration Magnum::OpenGLTester)
endif()

# GUI test application for quick ability to verify changes w/o having to
# compile examples as well (and to ensure the template APIs don't get out of
# sync with the apps, as the ContextGLTest has only a mock)
find_package(Corrade REQUIRED Main)

if(CORRADE_TARGET_ANDROID)
    find_package(Magnum OPTIONAL_COMPONENTS AndroidApplication)
    if(Magnum_AndroidApplication_FOUND)
        add_library(ImGuiIntegrationAndroidApplicationTest SHARED AndroidApplicationTest.cpp)
        target_link_libraries(ImGuiIntegrationAndroidApplicationTest PRIVATE
            MagnumImGuiIntegration
            Magnum::AndroidApplication
            Corrade::Main)
        if(CMAKE_ANDROID_NDK)
            include(${PROJECT_SOURCE_DIR}/toolchains/modules/UseAndroid.cmake)
            android_create_apk(ImGuiIntegrationAndroidApplicationTest AndroidManifest.xml)
        endif()
    endif()
endif()

if(CORRADE_TARGET_EMSCRIPTEN)
    find_package(Magnum OPTIONAL_COMPONENTS EmscriptenApplication)
    if(Magnum_EmscriptenApplication_FOUND)
        add_executable(ImGuiIntegrationEmscriptenApplicationTest EmscriptenApplicationTest.cpp)
        target_link_libraries(ImGuiIntegrationEmscriptenApplicationTest PRIVATE
            MagnumImGuiIntegration
            Magnum::EmscriptenApplication
            Corrade::Main)
        add_custom_command(TARGET ImGuiIntegrationEmscriptenApplicationTest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
                ${MAGNUM_WEBAPPLICATION_CSS}
                $<TARGET_FILE_DIR:ImGuiIntegrationEmscriptenApplicationTest>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTest.html
                $<TARGET_FILE_DIR:ImGuiIntegrationEmscriptenApplicationTest>/ImGuiIntegrationEmscriptenApplicationTest.html)
    endif()
endif()

if(NOT CORRADE_TARGET_ANDROID)
    find_package(Magnum OPTIONAL_COMPONENTS Sdl2Application)
    if(Magnum_Sdl2Application_FOUND)
        add_executable(ImGuiIntegrationSdl2ApplicationTest WIN32 Sdl2ApplicationTest.cpp)
        target_link_libraries(ImGuiIntegrationSdl2ApplicationTest PRIVATE
            MagnumImGuiIntegration
            Magnum::Sdl2Application
            Corrade::Main)
        if(CORRADE_TARGET_IOS OR CORRADE_TARGET_APPLE)
            # The plist is needed in order to mark the app as DPI-aware
            set_target_properties(ImGuiIntegrationSdl2ApplicationTest PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTest.plist
                MACOSX_BUNDLE ON
                XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
        endif()
        if(CORRADE_TARGET_WINDOWS AND NOT CORRADE_TARGET_WINDOWS_RT)
            if(MSVC)
                target_sources(ImGuiIntegrationSdl2ApplicationTest PRIVATE WindowsHiDPI.manifest)
            elseif(MINGW)
                target_sources(ImGuiIntegrationSdl2ApplicationTest PRIVATE WindowsHiDPI.rc)
            endif()
        endif()
        if(CORRADE_TARGET_EMSCRIPTEN)
            add_custom_command(TARGET ImGuiIntegrationSdl2ApplicationTest POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
                    ${MAGNUM_WEBAPPLICATION_CSS}
                    $<TARGET_FILE_DIR:ImGuiIntegrationSdl2ApplicationTest>
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTest.html
                    $<TARGET_FILE_DIR:ImGuiIntegrationSdl2ApplicationTest>/ImGuiIntegrationSdl2ApplicationTest.html)
        endif()
    endif()
endif()

if(NOT CORRADE_TARGET_ANDROID AND NOT CORRADE_TARGET_IOS AND NOT CORRADE_TARGET_EMSCRIPTEN)
    find_package(Magnum OPTIONAL_COMPONENTS GlfwApplication)
    if(Magnum_GlfwApplication_FOUND)
        add_executable(ImGuiIntegrationGlfwApplicationTest WIN32 GlfwApplicationTest.cpp)
        target_link_libraries(ImGuiIntegrationGlfwApplicationTest PRIVATE
            MagnumImGuiIntegration
            Magnum::GlfwApplication
            Corrade::Main)
        if(CORRADE_TARGET_APPLE)
            # The plist is needed in order to mark the app as DPI-aware
            set_target_properties(ImGuiIntegrationGlfwApplicationTest PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTest.plist
                MACOSX_BUNDLE ON
                XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "YES")
        endif()
        if(CORRADE_TARGET_WINDOWS AND NOT CORRADE_TARGET_WINDOWS_RT)
            if(MSVC)
                target_sources(ImGuiIntegrationGlfwApplicationTest PRIVATE WindowsHiDPI.manifest)
            elseif(MINGW)
                target_sources(ImGuiIntegrationGlfwApplicationTest PRIVATE WindowsHiDPI.rc)
            endif()
        endif()
    endif()
endif()
