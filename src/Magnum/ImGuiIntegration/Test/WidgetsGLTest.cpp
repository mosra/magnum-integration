/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
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

/* Explicitly disable deprecated functions on non-deprecated builds to catch
   issues early. Doing this only in tests so the library itself can be used
   with any newer version, but tests should be always run against the oldest
   supported which is mentioned in doc/namespaces.dox, and which is downloaded
   in all CI targets in package/ci/. The oldest supported version is tracked to
   be roughly two years back. */
#ifndef MAGNUM_BUILD_DEPRECATED
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#endif

#include <Corrade/Utility/System.h>
#include <Magnum/Magnum.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/OpenGLTester.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/TextureFormat.h>

#include "Magnum/ImGuiIntegration/Context.hpp"
#include "Magnum/ImGuiIntegration/Widgets.h"

#ifdef MAGNUM_TARGET_GLES2
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/ImageView.h>
#endif

namespace Magnum { namespace ImGuiIntegration { namespace Test { namespace {

struct WidgetsGLTest: GL::OpenGLTester {
    explicit WidgetsGLTest();

    void drawSetup();
    void drawTeardown();

    void image();
    void imageButton();

    private:
        GL::Renderbuffer _color{NoCreate};
        GL::Framebuffer _framebuffer{NoCreate};
};

WidgetsGLTest::WidgetsGLTest() {
    addTests({&WidgetsGLTest::image,
              &WidgetsGLTest::imageButton},
        &WidgetsGLTest::drawSetup,
        &WidgetsGLTest::drawTeardown);

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
}

void WidgetsGLTest::drawSetup() {
    constexpr Vector2i DrawSize{64, 64};

    _color = GL::Renderbuffer{};
    _color.setStorage(
        #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        DrawSize);

    _framebuffer = GL::Framebuffer{{{}, DrawSize}};
    _framebuffer
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        .clear(GL::FramebufferClear::Color)
        .bind();
}

void WidgetsGLTest::drawTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
}

void WidgetsGLTest::image() {
    /* Checks compilation and no GL errors only */
    Context c{{200, 200}};

    /* Again a dummy frame first as ImGui doesn't draw anything the first frame */
    c.newFrame();
    c.drawFrame();

    GL::Texture2D texture;
    #ifndef MAGNUM_TARGET_GLES2
    texture.setStorage(1, GL::TextureFormat::RGB8, {1, 1});
    #else
    texture.setImage(0, GL::TextureFormat::RGB,
        ImageView2D{GL::PixelFormat::RGB, GL::PixelType::UnsignedByte, {1, 1}});
    #endif

    Utility::System::sleep(1);

    c.newFrame();

    ImGuiIntegration::image(texture, {100, 100});

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void WidgetsGLTest::imageButton() {
    /* Checks compilation and no GL errors only */
    Context c{{200, 200}};

    /* Again a dummy frame first as ImGui doesn't draw anything the first frame */
    c.newFrame();
    c.drawFrame();

    GL::Texture2D texture;
    #ifndef MAGNUM_TARGET_GLES2
    texture.setStorage(1, GL::TextureFormat::RGB8, {1, 1});
    #else
    texture.setImage(0, GL::TextureFormat::RGB,
        ImageView2D{GL::PixelFormat::RGB, GL::PixelType::UnsignedByte, {1, 1}});
    #endif

    Utility::System::sleep(1);

    c.newFrame();

    ImGuiIntegration::imageButton("button", texture, {100, 100},
        {{}, Vector2{1.0f}}, Color4::yellow(), Color4::blue());

    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    ImGuiIntegration::imageButton(texture, {100, 100},
        {{}, Vector2{1.0f}}, 5, Color4::yellow(), Color4::blue());
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

}}}}

CORRADE_TEST_MAIN(Magnum::ImGuiIntegration::Test::WidgetsGLTest)
