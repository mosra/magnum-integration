#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025, 2026
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2013 Jan Dupal <dupal.j@gmail.com>
#   Copyright © 2019 bowling-allie <allie.smith.epic@gmail.com>
#   Copyright © 2026 Igal Alkon <igal@alkontek.com>
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
set(CMAKE_FOLDER "Magnum/JoltIntegration")

find_package(Magnum REQUIRED GL Shaders)

if(NOT MAGNUM_USE_EMSCRIPTEN_PORTS_JOLT)
    find_package(Jolt CONFIG REQUIRED)
endif()

if(MAGNUM_BUILD_STATIC)
    set(MAGNUM_JOLTINTEGRATION_BUILD_STATIC 1)
endif()

set(MagnumJoltIntegration_SRCS
    DebugDraw.cpp
)

set(MagnumJoltIntegration_HEADERS
    DebugDraw.h
    Integration.h
    visibility.h)

# JoltIntegration library
add_library(MagnumJoltIntegration ${SHARED_OR_STATIC}
    ${MagnumJoltIntegration_SRCS}
    ${MagnumJoltIntegration_HEADERS})

if(NOT MAGNUM_BUILD_STATIC)
    # Force static visibility for the example executable (no DLL export/import)
    target_compile_definitions(MagnumJoltIntegration PRIVATE MagnumJoltIntegration_EXPORTS)
endif()

set_target_properties(MagnumJoltIntegration PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

target_include_directories(MagnumJoltIntegration PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src)

set_target_properties(MagnumJoltIntegration PROPERTIES DEBUG_POSTFIX "-d")

if(NOT MAGNUM_JOLTINTEGRATION_BUILD_STATIC)
    set_target_properties(MagnumJoltIntegration PROPERTIES
            VERSION ${MAGNUMINTEGRATION_LIBRARY_VERSION}
            SOVERSION ${MAGNUMINTEGRATION_LIBRARY_SOVERSION})
elseif(MAGNUM_BUILD_STATIC_PIC)
    set_target_properties(MagnumJoltIntegration PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_link_libraries(MagnumJoltIntegration PUBLIC
    Magnum::GL
    Magnum::Magnum
    Magnum::Shaders
    Jolt::Jolt)

# If we use the Emscripten port, no find_package() was called and the targets
# are not defined.
if(MAGNUM_USE_EMSCRIPTEN_PORTS_JOLT)
    target_compile_options(MagnumJoltIntegration PUBLIC "SHELL:-s USE_JOLT=1")
    target_link_options(MagnumJoltIntegration PUBLIC "SHELL:-s USE_JOLT=1")
endif()

install(TARGETS MagnumJoltIntegration
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumJoltIntegration_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/JoltIntegration)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/JoltIntegration)

if(MAGNUM_BUILD_TESTS)
    add_subdirectory(Test ${EXCLUDE_FROM_ALL_IF_TEST_TARGET})
endif()

# Magnum-Jolt integration target alias for super-projects
add_library(MagnumIntegration::Jolt ALIAS MagnumJoltIntegration)
