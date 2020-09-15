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
#include <Magnum/GL/Renderer.h>
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
/* [Context-custom-fonts-after] */
ImGuiIntegration::Context imgui({640, 480});

// ...

ImGui::GetIO().Fonts->Clear();
ImGui::GetIO().Fonts->AddFontFromFileTTF("SourceSansPro-Regular.ttf", 16.0f);

imgui.relayout(windowSize());
/* [Context-custom-fonts-after] */
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

{
/* [Context-text-input] */
_imgui.newFrame();
if(ImGui::GetIO().WantTextInput && !isTextInputActive())
    startTextInput();
else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
    stopTextInput();
/* [Context-text-input] */
}

{
/* [Context-usage-per-frame] */
GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
    GL::Renderer::BlendEquation::Add);
GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
    GL::Renderer::BlendFunction::OneMinusSourceAlpha);

_imgui.newFrame();

// ImGui widget calls here ...

_imgui.updateApplicationCursor(*this);

GL::Renderer::enable(GL::Renderer::Feature::Blending);
GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);

_imgui.drawFrame();

GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
GL::Renderer::disable(GL::Renderer::Feature::Blending);

// ...

redraw();
/* [Context-usage-per-frame] */
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
