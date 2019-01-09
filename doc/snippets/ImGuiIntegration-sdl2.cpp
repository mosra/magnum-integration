/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
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
#include <Magnum/Platform/Sdl2Application.h>

#include "Magnum/ImGuiIntegration/Context.h"

using namespace Magnum;

struct MyApp: Platform::Sdl2Application {
void foo() {
{
/* [Context-dpi-unaware] */
ImGuiIntegration::Context imgui{windowSize()};
/* [Context-dpi-unaware] */
}
{
/* [Context-dpi-aware] */
ImGuiIntegration::Context imgui{
    Vector2{windowSize()}/dpiScaling(), windowSize(), framebufferSize()};
/* [Context-dpi-aware] */
}

{
/* [Context-custom-fonts-dpi] */
ImGui::CreateContext();

const Vector2 size = Vector2{windowSize()}/dpiScaling();

ImGui::GetIO().Fonts->AddFontFromFileTTF("SourceSansPro-Regular.ttf",
    16.0f*framebufferSize().x()/size.x());

ImGuiIntegration::Context imgui(*ImGui::GetCurrentContext(),
    size, windowSize(), framebufferSize());

// ...
/* [Context-custom-fonts-dpi] */
}
}

void viewportEvent(ViewportEvent& event) override;
void mousePressEvent(MouseEvent& event) override;
void mouseReleaseEvent(MouseEvent& event) override;

ImGuiIntegration::Context _imgui{Vector2i{}};
Float _supersamplingRatio{};
};

/* [Context-events] */
#include <Magnum/ImGuiIntegration/Context.hpp>

// ...

void MyApp::mousePressEvent(MouseEvent& event) {
    if(_imgui.handleMousePressEvent(event)) return;

    // event not handled by ImGui, handle it for the app itself
}

void MyApp::mouseReleaseEvent(MouseEvent& event) {
    if(_imgui.handleMouseReleaseEvent(event)) return;

    // ...
}

// ...
/* [Context-events] */

/* [Context-relayout-fonts-dpi] */
void MyApp::viewportEvent(ViewportEvent& event) {
    // ...

    const Vector2 size = Vector2{event.windowSize()}/event.dpiScaling();

    /* Reload fonts if pixel density changed */
    const Float supersamplingRatio = event.framebufferSize().x()/size.x();
    if(supersamplingRatio != _supersamplingRatio) {
        _supersamplingRatio = supersamplingRatio;

        ImGui::GetIO().Fonts->Clear(); // important
        ImGui::GetIO().Fonts->AddFontFromFileTTF("SourceSansPro-Regular.ttf",
            16.0f*supersamplingRatio);
    }

    _imgui.relayout(size, event.windowSize(), event.framebufferSize());
}
/* [Context-relayout-fonts-dpi] */
