#ifndef Magnum_ImGuiIntegration_Test_ApplicationTest_hpp
#define Magnum_ImGuiIntegration_Test_ApplicationTest_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 ShaddyAQN <ShaddyAQN@gmail.com>
    Copyright © 2018 Tomáš Skřivan <skrivantomas@seznam.cz>
    Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2018 Natesh Narain <nnaraindev@gmail.com>

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

#include <Magnum/Math/Time.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

namespace Magnum { namespace ImGuiIntegration { namespace Test {

/* Assumes some Application header is included before this file, currently
   tests mainly just that everything compiles. See Sdl2ApplicationTest.cpp etc.
   for concrete usage. */

#ifdef MAGNUM_APPLICATION_MAIN
using namespace Math::Literals;

class ApplicationTest: public Platform::Application {
    public:
        explicit ApplicationTest(const Arguments& arguments);

        void drawEvent() override;

        void viewportEvent(ViewportEvent& event) override;

        #ifndef CORRADE_TARGET_ANDROID
        void keyPressEvent(KeyEvent& event) override{
            if(_imgui.handleKeyPressEvent(event)) return;
        }

        void keyReleaseEvent(KeyEvent& event) override {
            if(_imgui.handleKeyReleaseEvent(event)) return;
        }
        #endif

        /* Set to 0 to test the deprecated mouse events instead */
        #if 1
        void pointerPressEvent(PointerEvent& event) override {
            if(_imgui.handlePointerPressEvent(event)) return;
        }
        void pointerReleaseEvent(PointerEvent& event) override {
            if(_imgui.handlePointerReleaseEvent(event)) return;
        }
        void pointerMoveEvent(PointerMoveEvent& event) override {
            if(_imgui.handlePointerMoveEvent(event)) return;
        }
        #ifndef CORRADE_TARGET_ANDROID
        void scrollEvent(ScrollEvent& event) override {
            if(_imgui.handleScrollEvent(event)) {
                /* Prevent scrolling the page */
                event.setAccepted();
                return;
            }
        }
        #endif
        #else
        CORRADE_IGNORE_DEPRECATED_PUSH
        void mousePressEvent(MouseEvent& event) override {
            if(_imgui.handleMousePressEvent(event)) return;
        }
        void mouseReleaseEvent(MouseEvent& event) override {
            if(_imgui.handleMouseReleaseEvent(event)) return;
        }
        void mouseMoveEvent(MouseMoveEvent& event) override {
            if(_imgui.handleMouseMoveEvent(event)) return;
        }
        #ifndef CORRADE_TARGET_ANDROID
        void mouseScrollEvent(MouseScrollEvent& event) override {
            if(_imgui.handleMouseScrollEvent(event)) {
                /* Prevent scrolling the page */
                event.setAccepted();
                return;
            }
        }
        #endif
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        #ifndef CORRADE_TARGET_ANDROID
        void textInputEvent(TextInputEvent& event) override {
            if(_imgui.handleTextInputEvent(event)) return;
        }
        #endif

    private:
        ImGuiIntegration::Context _imgui{NoCreate};
};

ApplicationTest::ApplicationTest(const Arguments& arguments):
    Platform::Application{arguments, Configuration{}
        .setTitle("Magnum ImGui Application Test")
        #ifndef CORRADE_TARGET_ANDROID
        .setWindowFlags(Configuration::WindowFlag::Resizable)
        #endif
    }
{
    _imgui = ImGuiIntegration::Context(Vector2{windowSize()}/dpiScaling(),
        windowSize(), framebufferSize());

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
        GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
    /* Have some sane speed, please */
    setMinimalLoopPeriod(16.0_msec);
    #endif
}

void ApplicationTest::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    _imgui.newFrame();

    #ifndef CORRADE_TARGET_ANDROID
    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();
    #endif

    ImGui::ShowDemoWindow();

    #ifndef CORRADE_TARGET_ANDROID
    /* Update application cursor */
    _imgui.updateApplicationCursor(*this);
    #endif

    _imgui.drawFrame();

    swapBuffers();
    redraw();
}

void ApplicationTest::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    _imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
        event.windowSize(), event.framebufferSize());
}
#endif

}}}

#endif
