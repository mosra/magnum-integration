#ifndef Magnum_ImGuiIntegration_Widgets_h
#define Magnum_ImGuiIntegration_Widgets_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 ShaddyAQN <ShaddyAQN@gmail.com>
    Copyright © 2018 Tomáš Skřivan <skrivantomas@seznam.cz>
    Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2024 kolbbond <kolbbond@gmail.com>
    Copyright © 2024 Pablo Escobar <mail@rvrs.in>

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

/** @file
 * @brief Function @ref Magnum::ImGuiIntegration::image(), @ref Magnum::ImGuiIntegration::imageButton()
 */

#include "Magnum/ImGuiIntegration/visibility.h" /* defines IMGUI_API */

#include <imgui.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/GL.h>

#include "Magnum/ImGuiIntegration/Integration.h"

namespace Magnum { namespace ImGuiIntegration {

/**
@brief Image widget displaying a @ref GL::Texture2D
@param texture      Texture to display
@param size         Widget size
@param uvRange      UV range on the texture (covers the whole texture by
    default)
@param tintColor    Tint color, default @cpp 0xffffffff_rgbaf @ce
@param borderColor  Border color, default @cpp 0x00000000_rgbaf @ce
*/
inline void image(GL::Texture2D& texture, const Vector2& size,
    const Range2D& uvRange = {{}, Vector2{1.0f}},
    const Color4& tintColor = Color4{1.0f},
    const Color4& borderColor = {})
{
    ImGui::Image(static_cast<ImTextureID>(&texture), ImVec2(size), ImVec2(uvRange.topLeft()), ImVec2(uvRange.bottomRight()), ImColor(tintColor), ImColor(borderColor));
}

/**
@brief ImageButton widget displaying a @ref GL::Texture2D
@param id               Widget ID
@param texture          Texture to display
@param size             Widget size
@param uvRange          UV range on the texture (covers the whole texture by
    default)
@param backgroundColor  Background color, default @cpp 0x00000000_rgbaf @ce
@param tintColor        Tint color, default @cpp 0xffffffff_rgbaf @ce
@m_since_latest
*/
inline bool imageButton(const char* id, GL::Texture2D& texture, const Vector2& size,
    const Range2D& uvRange = {{}, Vector2{1.0f}},
    const Color4& backgroundColor = {},
    const Color4& tintColor = Color4{1.0f})
{
    /* Old function generating an implicit ID and taking frame padding from an
       explicit variable was deprecated in 1.89 and removed in 1.91.1 */
    #if IMGUI_VERSION_NUM >= 19110
    return ImGui::ImageButton(id, static_cast<ImTextureID>(&texture), ImVec2(size), ImVec2(uvRange.topLeft()), ImVec2(uvRange.bottomRight()), ImColor(backgroundColor), ImColor(tintColor));
    #else
    /* This is not exactly the same since the old function pushes another ID
       based on the texture ID, but we can't disable that. Just a best effort
       to still use the user-provided widget ID. There is ImageButtonEx()
       taking an explicit ID, but its signature doesn't seem stable. */
    ImGui::PushID(id);
    /* Negative padding uses the FramePadding style */
    const bool ret = ImGui::ImageButton(static_cast<ImTextureID>(&texture), ImVec2(size), ImVec2(uvRange.topLeft()), ImVec2(uvRange.bottomRight()), -1, ImColor(backgroundColor), ImColor(tintColor));
    ImGui::PopID();
    return ret;
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief ImageButton widget displaying a @ref GL::Texture2D
@m_deprecated_since_latest Using an implicit ID is no longer possible in newer
    ImGui. Use @ref imageButton(const char*, GL::Texture2D&, const Vector2&, const Range2D&, const Color4&, const Color4&)
    instead.
*/
CORRADE_DEPRECATED("use imageButton(const char*, GL::Texture2D&, const Vector2&, const Range2D&, const Color4&, const Color4&) instead") inline bool imageButton(
    GL::Texture2D& texture, const Vector2& size,
    const Range2D& uvRange = {{}, Vector2{1.0f}}, Int framePadding = -1,
    const Color4& backgroundColor = {},
    const Color4& tintColor = Color4{1.0f})
{
    const ImTextureID textureId = static_cast<ImTextureID>(&texture);
    /* Old function generating an implicit ID and taking frame padding from an
       explicit variable was deprecated in 1.89 and removed in 1.91.1. This is
       identical to the obsoleted wrapper code still present (but commented
       out) in 1.91.1. */
    #if IMGUI_VERSION_NUM >= 19110
    /* ImTextureID is already void* */
    ImGui::PushID(textureId);
    if(framePadding >= 0)
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(framePadding, framePadding));
    const bool ret = imageButton("", texture, size, uvRange, backgroundColor, tintColor);
    if(framePadding >= 0)
        ImGui::PopStyleVar();
    ImGui::PopID();
    return ret;
    #else
    return ImGui::ImageButton(textureId, ImVec2(size), ImVec2(uvRange.topLeft()), ImVec2(uvRange.bottomRight()), framePadding, ImColor(backgroundColor), ImColor(tintColor));
    #endif
}
#endif

}}

#endif
