#ifndef Magnum_ImGuiIntegration_visibility_h
#define Magnum_ImGuiIntegration_visibility_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 ShaddyAQN <ShaddyAQN@gmail.com>
    Copyright © 2018 Tomáš Skřivan <skrivantomas@seznam.cz>
    Copyright © 2018 Jonathan Hale <squareys@googlemail.com>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/Utility/VisibilityMacros.h>

#include "Magnum/ImGuiIntegration/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_IMGUIINTEGRATION_BUILD_STATIC
    #if defined(MagnumImGuiIntegration_EXPORTS) || defined(MagnumImGuiIntegrationTestLib_EXPORTS)
        #define MAGNUM_IMGUIINTEGRATION_EXPORT CORRADE_VISIBILITY_EXPORT
        /* From imconfig.h to export ImGui symbols. If already defined (for
           example by our FindImGui.cmake), use that definition instead, which
           allows for example a dynamic build with a static ImGui library or
           vice versa. */
        #ifndef IMGUI_API
            #define IMGUI_API CORRADE_VISIBILITY_EXPORT
        #endif
    #else
        #define MAGNUM_IMGUIINTEGRATION_EXPORT CORRADE_VISIBILITY_IMPORT
        #ifndef IMGUI_API
            #define IMGUI_API CORRADE_VISIBILITY_IMPORT
        #endif
    #endif
#else
    #define MAGNUM_IMGUIINTEGRATION_EXPORT CORRADE_VISIBILITY_STATIC
    #ifndef IMGUI_API
        #define IMGUI_API CORRADE_VISIBILITY_STATIC
    #endif
#endif
#else
#define MAGNUM_IMGUIINTEGRATION_EXPORT
#define IMGUI_API
#endif

#endif
