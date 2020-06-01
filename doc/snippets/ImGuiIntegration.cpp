/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
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

#include <imgui.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>

#include "Magnum/ImGuiIntegration/Integration.h"
#include "Magnum/ImGuiIntegration/Context.h"

using namespace Magnum;

int main() {
{
/* [Integration] */
ImVec2 a{20.0f, 50.0f};
Vector2 b(a);

using namespace Math::Literals;
ImColor c = ImColor(0xff9391_rgbf);
/* [Integration] */
static_cast<void>(b);
static_cast<void>(c);
}

{
/* [Context-usage] */
ImGuiIntegration::Context imgui{{640, 480}};

// ...
/* [Context-usage] */
}

{
/* [Context-usage-state-imgui-only] */
GL::Renderer::enable(GL::Renderer::Feature::Blending);
GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
    GL::Renderer::BlendEquation::Add);
GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
    GL::Renderer::BlendFunction::OneMinusSourceAlpha);
/* [Context-usage-state-imgui-only] */
}
{
/* [Context-custom-fonts] */
ImGui::CreateContext();

ImGui::GetIO().Fonts->AddFontFromFileTTF("SourceSansPro-Regular.ttf", 16.0f);

ImGuiIntegration::Context imgui(*ImGui::GetCurrentContext(), {640, 480});

// ...
/* [Context-custom-fonts] */
}

{
/* [Context-custom-fonts-resource] */
Utility::Resource rs{"fonts"};
Containers::ArrayView<const char> font = rs.getRaw("SourceSansPro-Regular.ttf");

ImFontConfig fontConfig;
fontConfig.FontDataOwnedByAtlas = false;
ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
    const_cast<char*>(font.data()), font.size(), 16.0f, &fontConfig);

// ...
/* [Context-custom-fonts-resource] */
}
}
