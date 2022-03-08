/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2022 Pablo Escobar <mail@rvrs.in>

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

#include <limits>
#include <sstream>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove when AbstractImporter is <string>-free */
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/System.h>
#include <Corrade/Utility/Path.h>
#include <Magnum/Magnum.h>
#include <Magnum/DebugTools/CompareImage.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/OpenGLTester.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Image.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>

#include "Magnum/ImGuiIntegration/Context.hpp"

#include "configure.h"

namespace Magnum { namespace ImGuiIntegration { namespace Test { namespace {

struct InputEvent {
    enum class Modifier: Int {
        Shift = 1 << 0,
        Ctrl = 1 << 1,
        Alt = 1 << 2,
        Super = 1 << 3
    };
    typedef Containers::EnumSet<Modifier> Modifiers;

    InputEvent(Modifiers modifiers) : _modifiers(modifiers) {}

    Modifiers _modifiers;

    Modifiers modifiers() { return _modifiers; }
};

enum class Button: Int {
    Left, Middle, Right
};

struct MouseEvent: public InputEvent {
    typedef Magnum::ImGuiIntegration::Test::Button Button;

    MouseEvent(Button button, Vector2i position, Modifiers modifiers) :
        InputEvent(modifiers), _button(button), _position(position) {}

    Button _button;
    Vector2i _position;

    Button button() { return _button; }
    Vector2i position() { return _position; }

};

struct MouseScrollEvent: public InputEvent {
    MouseScrollEvent(Vector2 offset, Vector2i position, Modifiers modifiers) :
        InputEvent(modifiers), _offset(offset), _position(position) {}

    Vector2 _offset;
    Vector2i _position;

    Vector2 offset() { return _offset; }
    Vector2i position() { return _position; }
};

struct Application {
    enum class Cursor {
        Arrow,
        Hand,
        Hidden,
        TextInput,
        ResizeAll,
        ResizeWE,
        ResizeNS,
        None = 999
    };

    void setCursor(Cursor cursor) { currentCursor = cursor; }
    Cursor currentCursor = Cursor::None;
};

struct KeyEvent: public InputEvent {
    enum class Key: Int {
        Unknown,
        LeftShift, RightShift, LeftCtrl, RightCtrl,
        LeftAlt, RightAlt, LeftSuper, RightSuper,

        Enter, Esc,

        Up, Down, Left, Right, Home, End,
        PageUp, PageDown, Backspace, Insert, Delete,

        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        Space, Tab, Quote, Comma, Period, Minus, Plus, Slash, Percent,
        Semicolon, Equal, LeftBracket, RightBracket, Backslash, Backquote,

        Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,

        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U,
        V, W, X, Y, Z,

        CapsLock, ScrollLock, NumLock, PrintScreen, Pause, Menu,

        NumZero, NumOne, NumTwo, NumThree, NumFour, NumFive,
        NumSix, NumSeven, NumEight, NumNine, NumDecimal, NumDivide,
        NumMultiply, NumSubtract, NumAdd, NumEnter, NumEqual,
    };

    KeyEvent(Key key, Modifiers modifiers) :
        InputEvent(modifiers), _key(key) {}

    Key _key;

    Key key() { return _key; }
};

struct TextInputEvent {
    Containers::ArrayView<const char> text() const { return _text; }

    Containers::ArrayView<const char> _text;
};

struct ContextGLTest: GL::OpenGLTester {
    explicit ContextGLTest();

    void construct();
    void constructExistingContext();
    void constructExistingContextAddFont();
    void constructMove();
    void moveAssignEmpty();

    void release();

    void frame();
    void frameZeroSize();

    void relayout();
    void relayoutDpiChange();
    void relayoutDpiChangeCustomFont();
    void relayoutZeroSize();
    void relayoutRefreshFonts();

    void mouseInput();
    void mouseInputTooFast();
    void keyInput();
    void textInput();
    void updateCursor();

    void multipleContexts();

    void drawSetup();
    void drawTeardown();

    void draw();
    void drawTexture();
    void drawScissor();
    void drawVertexOffset();
    void drawIndexOffset();

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager;

        GL::Renderbuffer _color{NoCreate};
        GL::Framebuffer _framebuffer{NoCreate};
};

ContextGLTest::ContextGLTest() {
    addTests({&ContextGLTest::construct,
              &ContextGLTest::constructExistingContext,
              &ContextGLTest::constructExistingContextAddFont,
              &ContextGLTest::constructMove,
              &ContextGLTest::moveAssignEmpty,

              &ContextGLTest::release,

              &ContextGLTest::frame,
              &ContextGLTest::frameZeroSize,

              &ContextGLTest::relayout,
              &ContextGLTest::relayoutDpiChange,
              &ContextGLTest::relayoutDpiChangeCustomFont,
              &ContextGLTest::relayoutZeroSize,
              &ContextGLTest::relayoutRefreshFonts,

              &ContextGLTest::mouseInput,
              &ContextGLTest::mouseInputTooFast,
              &ContextGLTest::keyInput,
              &ContextGLTest::textInput,
              &ContextGLTest::updateCursor,

              &ContextGLTest::multipleContexts});

    addTests({&ContextGLTest::draw,
              &ContextGLTest::drawTexture,
              &ContextGLTest::drawScissor,
              &ContextGLTest::drawVertexOffset,
              &ContextGLTest::drawIndexOffset},
        &ContextGLTest::drawSetup,
        &ContextGLTest::drawTeardown);

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
}

void ContextGLTest::construct() {
    {
        Context c{{}};

        CORRADE_VERIFY(c.context());
        CORRADE_COMPARE(c.context(), ImGui::GetCurrentContext());
        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->GetDebugName(), std::string{"ProggyClean.ttf, 13px [SCALED]"});
        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->FontSize, 13.0f);

        MAGNUM_VERIFY_NO_GL_ERROR();
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Can't test the single-instance assertions because the internal assert
       fires every time */
}

void ContextGLTest::constructExistingContext() {
    {
        ImGuiContext* context = ImGui::CreateContext();

        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 0);

        Context c{*ImGui::GetCurrentContext(), {}};

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_VERIFY(c.context());
        CORRADE_COMPARE(c.context(), context);
        /* No user-supplied font even though we used a custom context, add
           the default one  */
        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->GetDebugName(), std::string{"ProggyClean.ttf, 13px [SCALED]"});
        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->FontSize, 13.0f);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::constructExistingContextAddFont() {
    {
        ImGui::CreateContext();

        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 0);

        ImGui::GetIO().Fonts->AddFontDefault();

        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);

        Context c{*ImGui::GetCurrentContext(), {}};

        MAGNUM_VERIFY_NO_GL_ERROR();

        /* The user-supplied font should not get overriden, even though it's
           the same */
        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->GetDebugName(), std::string{"ProggyClean.ttf, 13px"});
        CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->FontSize, 13.0f);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::constructMove() {
    Context a{{200, 200}};
    ImGuiContext* context = a.context();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(context);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), context);

    Context b{std::move(a)};
    CORRADE_COMPARE(a.context(), nullptr);
    CORRADE_COMPARE(b.context(), context);

    CORRADE_COMPARE(&b.atlasTexture(), ImGui::GetIO().Fonts->TexID);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), context);

    Context c{{}};
    ImGuiContext* cContext = c.context();
    CORRADE_COMPARE(ImGui::GetCurrentContext(), cContext);

    c = std::move(b);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), cContext);
    CORRADE_COMPARE(&b.atlasTexture(), ImGui::GetIO().Fonts->TexID);
    ImGui::SetCurrentContext(c.context());
    CORRADE_COMPARE(&c.atlasTexture(), ImGui::GetIO().Fonts->TexID);

    /* This should not blow up */
    ImGui::SetCurrentContext(cContext);
    c.newFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(ImGui::GetCurrentContext(), context);

    c.drawFrame();

    Utility::System::sleep(1);

    /* ImGui doesn't draw anything the first frame so do it twice. */
    c.newFrame();
    ImGui::Button("test");
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::moveAssignEmpty() {
    Context a{NoCreate};
    Context b{{}};

    /* Move from empty */
    b = std::move(a);
    CORRADE_COMPARE(b.context(), nullptr);

    /* Move into empty */
    b = std::move(a);
    CORRADE_COMPARE(a.context(), nullptr);

    /* Move empty into empty */
    Context c{NoCreate};
    c = std::move(a);
    CORRADE_COMPARE(a.context(), nullptr);
    CORRADE_COMPARE(c.context(), nullptr);
}

void ContextGLTest::release() {
    ImGuiContext* context;

    {
        Context c{{}};

        CORRADE_VERIFY(c.context());
        CORRADE_COMPARE(ImGui::GetCurrentContext(), c.context());
        context = c.release();
        CORRADE_COMPARE(ImGui::GetCurrentContext(), context);
        CORRADE_COMPARE(c.context(), nullptr);
    }

    CORRADE_COMPARE(ImGui::GetCurrentContext(), context);
    ImGui::DestroyContext(context);
}

void ContextGLTest::frame() {
    Context c{{200, 200}, {70, 70}, {350, 350}};

    /* ImGui doesn't draw anything the first frame so just do a dummy frame
       first in order to have stuff rendered the second and have the loop
       actually covered. It's know that ImGui has one frame lag but this seems
       different, since the button is rendered in the second frame only. */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Utility::System::sleep(1);

    /* This should render stuff now */
    c.newFrame();
    ImGui::Button("test");
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::frameZeroSize() {
    Context c{{0, 200}};

    /* Again a dummy frame first */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Utility::System::sleep(1);

    c.newFrame();

    ImGui::Button("test");

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::relayout() {
    Context c{{400, 400}};

    /* Again a dummy frame first */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->GetDebugName(), std::string{"ProggyClean.ttf, 13px [SCALED]"});
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->FontSize, 13.0f);

    c.relayout({200, 200});

    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->GetDebugName(), std::string{"ProggyClean.ttf, 13px [SCALED]"});
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->FontSize, 13.0f);

    Utility::System::sleep(1);

    /* This should render stuff now */
    c.newFrame();
    ImGui::Button("test");
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::relayoutDpiChange() {
    Context c{{400, 400}};

    /* Again a dummy frame first */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->GetDebugName(), std::string{"ProggyClean.ttf, 13px [SCALED]"});
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->FontSize, 13.0f);

    c.relayout({200, 200}, {70, 70}, {400, 400});

    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->GetDebugName(), std::string{"ProggyClean.ttf, 13px [SCALED]"});
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->FontSize, 26.0f); /* 2x */

    Utility::System::sleep(1);

    /* This should render stuff now */
    c.newFrame();
    ImGui::Button("test");
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::relayoutDpiChangeCustomFont() {
    ImGui::CreateContext();

    ImGui::GetIO().Fonts->AddFontDefault();

    Context c{*ImGui::GetCurrentContext(), {400, 400}};

    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->GetDebugName(), std::string{"ProggyClean.ttf, 13px"});
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->FontSize, 13.0f);

    /* Again a dummy frame first */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->GetDebugName(), std::string{"ProggyClean.ttf, 13px"});
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts[0]->FontSize, 13.0f); /*same */

    c.relayout({200, 200}, {70, 70}, {400, 400});

    Utility::System::sleep(1);

    /* This should render stuff now */
    c.newFrame();
    ImGui::Button("test");
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::relayoutZeroSize() {
    Context c{{200, 200}};

    /* Again a dummy frame first */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    c.relayout({100, 0});

    Utility::System::sleep(1);

    /* This should render stuff now */
    c.newFrame();
    ImGui::Button("test");
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::relayoutRefreshFonts() {
    Context c{{200, 200}};

    /* Clear the default font and add a new one */
    CORRADE_COMPARE(ImGui::GetIO().Fonts->Fonts.size(), 1);
    ImGui::GetIO().Fonts->Clear();
    ImGui::GetIO().Fonts->AddFontDefault();

    /* Relayout should pick that up */
    c.relayout({200, 200});

    /* Again a dummy frame first */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Utility::System::sleep(1);

    /* This should render stuff now */
    c.newFrame();
    ImGui::Button("test");
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void ContextGLTest::mouseInput() {
    Context c{{200, 200}};

    /* Mouse Button */
    CORRADE_VERIFY(!ImGui::IsMouseDown(0)); /* left */
    CORRADE_VERIFY(!ImGui::IsMouseDown(1)); /* right */
    CORRADE_VERIFY(!ImGui::IsMouseDown(2)); /* middle */

    MouseEvent left{Button::Left, {1, 2}, {}};
    MouseEvent right{Button::Right, {3, 4}, {}};
    MouseEvent middle{Button::Middle, {5, 6}, {}};

    /* The queued IO events in imgui 1.87 and up don't report any data until
       newFrame() is called. Our workaround for the legacy IO system that keeps
       imgui from ignoring mouse clicks if both press and release happen in the
       same frame behaves the same way. Events are propagated to it only during
       newFrame() -- which means we need to check *after* it gets called. See
       mouseInputTooFast() below for more. */

    c.handleMousePressEvent(left);
    Utility::System::sleep(1);
    c.newFrame();
    CORRADE_VERIFY(ImGui::IsMouseDown(0)); /* left */
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{1.0f, 2.0f}));
    c.drawFrame();

    c.handleMousePressEvent(right);
    Utility::System::sleep(1);
    c.newFrame();
    CORRADE_VERIFY(ImGui::IsMouseDown(1)); /* right */
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{3.0f, 4.0f}));
    c.drawFrame();

    c.handleMousePressEvent(middle);
    Utility::System::sleep(1);
    c.newFrame();
    CORRADE_VERIFY(ImGui::IsMouseDown(2)); /* middle */
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{5.0f, 6.0f}));
    c.drawFrame();

    c.handleMouseReleaseEvent(right);
    Utility::System::sleep(1);
    c.newFrame();
    CORRADE_VERIFY(!ImGui::IsMouseDown(1)); /* right */
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{3.0f, 4.0f}));
    c.drawFrame();

    c.handleMouseReleaseEvent(left);
    Utility::System::sleep(1);
    c.newFrame();
    CORRADE_VERIFY(!ImGui::IsMouseDown(0)); /* left */
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{1.0f, 2.0f}));
    c.drawFrame();

    c.handleMouseReleaseEvent(middle);
    Utility::System::sleep(1);
    c.newFrame();
    CORRADE_VERIFY(!ImGui::IsMouseDown(2)); /* middle */
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{5.0f, 6.0f}));
    c.drawFrame();

    /* Mouse Movement */
    MouseEvent move{Button{}, {1, 2}, {}};
    c.handleMouseMoveEvent(move);
    Utility::System::sleep(1);
    c.newFrame();
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{1.0f, 2.0f}));
    c.drawFrame();

    /* Scrolling/Mouse Wheel */
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheelH, 0.0f, Float);
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheel, 0.0f, Float);

    MouseScrollEvent scroll{{1.2f, -1.2f}, {17, 23}, {}};
    c.handleMouseScrollEvent(scroll);
    Utility::System::sleep(1);
    c.newFrame();
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{17.0f, 23.0f}));
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheelH, 1.2f, Float);
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheel, -1.2f, Float);
    c.drawFrame();

    /* Unknown buttons shouldn't be propagated to imgui */
    MouseEvent unknownButton{Button(666), {1, 2}, {}};
    CORRADE_VERIFY(!c.handleMousePressEvent(unknownButton));
    CORRADE_VERIFY(!c.handleMouseReleaseEvent(unknownButton));
}

void ContextGLTest::mouseInputTooFast() {
    Context c{{200, 200}};

    CORRADE_VERIFY(!ImGui::IsMouseDown(0));

    /* It's not reported immediately */
    MouseEvent left{Button::Left, {1, 2}, {}};
    c.handleMousePressEvent(left);
    c.handleMouseReleaseEvent(left);
    CORRADE_VERIFY(!ImGui::IsMouseDown(0));

    /* Only during the newFrame call */
    c.newFrame();
    CORRADE_VERIFY(ImGui::IsMouseDown(0));
    c.drawFrame();

    /* And mouse up is reported in the next one */
    c.newFrame();
    CORRADE_VERIFY(!ImGui::IsMouseDown(0));
    c.drawFrame();
}

void ContextGLTest::keyInput() {
    Context c{{}};

    CORRADE_VERIFY(!ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Tab)));

    /* The queued IO events in imgui 1.87 and up don't report any data until
       newFrame() is called */

    KeyEvent keyTab{KeyEvent::Key::Tab, {}};
    c.handleKeyPressEvent(keyTab);
    c.newFrame();
    CORRADE_VERIFY(ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Tab)));
    c.drawFrame();

    c.handleKeyReleaseEvent(keyTab);
    c.newFrame();
    CORRADE_VERIFY(!ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Tab)));
    c.drawFrame();

    /* Key presses don't affect imgui modifier key state */
    KeyEvent keyAlt{KeyEvent::Key::LeftAlt, {}};
    c.handleKeyPressEvent(keyAlt);
    c.newFrame();
    CORRADE_VERIFY(!ImGui::GetIO().KeyAlt);
    c.drawFrame();

    /* Only event modifier flags do */
    KeyEvent modifierAlt{KeyEvent::Key::LeftAlt, KeyEvent::Modifier::Alt};
    c.handleKeyPressEvent(modifierAlt);
    c.newFrame();
    CORRADE_VERIFY(ImGui::GetIO().KeyAlt);
    c.drawFrame();

    /* Unknown keys shouldn't be propagated to imgui */
    KeyEvent unknownKey{KeyEvent::Key(666), {}};
    CORRADE_VERIFY(!c.handleKeyPressEvent(unknownKey));
    CORRADE_VERIFY(!c.handleKeyReleaseEvent(unknownKey));
}

void ContextGLTest::textInput() {
    Context c{{}};

    /* The queued IO events in imgui 1.87 and up don't report any data until
       newFrame() is called */

    TextInputEvent textEvent{{"abc"}};
    c.handleTextInputEvent(textEvent);
    c.newFrame();
    ImWchar expected[3]{'a', 'b', 'c'};
    /* This changed from InputCharacters to InputQueueCharacters in 1.67. Yes,
       it's a private API, but there's no other way to test if the event works
       correctly. It's used only in a test so I think that's acceptable. */
    CORRADE_COMPARE_AS(Containers::arrayView(ImGui::GetIO().InputQueueCharacters.begin(), ImGui::GetIO().InputQueueCharacters.size()),
        Containers::arrayView(expected),
        TestSuite::Compare::Container);
    c.drawFrame();
}

void ContextGLTest::updateCursor() {
    Context c{{}};

    Application app;

    /* Default (should be an arrow) */
    c.updateApplicationCursor(app);
    CORRADE_VERIFY(app.currentCursor == Application::Cursor::Arrow);

    /* Change to a cursor that is present in all apps */
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    c.updateApplicationCursor(app);
    CORRADE_VERIFY(app.currentCursor == Application::Cursor::Hand);

    /* Change to a cursor that is unknown -> fallback to an arrow */
    ImGui::SetMouseCursor(ImGuiMouseCursor_COUNT);
    c.updateApplicationCursor(app);
    CORRADE_VERIFY(app.currentCursor == Application::Cursor::Arrow);

    /* Change to a cursor that is conditional but present in this app */
    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    c.updateApplicationCursor(app);
    CORRADE_VERIFY(app.currentCursor == Application::Cursor::ResizeAll);

    /* Change to a cursor that is conditional and not present -> fallback to
       an arrow */
    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
    c.updateApplicationCursor(app);
    CORRADE_VERIFY(app.currentCursor == Application::Cursor::Arrow);
}

void ContextGLTest::multipleContexts() {
    Context a{{200, 200}};
    Context b{{400, 300}};
    CORRADE_VERIFY(a.context() != b.context());
    CORRADE_COMPARE(ImGui::GetCurrentContext(), b.context());

    a.newFrame();
    a.drawFrame();
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(ImGui::GetCurrentContext(), a.context());

    b.newFrame();
    b.drawFrame();
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(ImGui::GetCurrentContext(), b.context());

    Utility::System::sleep(1);

    /* This should render stuff now */
    a.newFrame();
    ImGui::Button("test");
    a.drawFrame();
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(ImGui::GetCurrentContext(), a.context());

    /* This should render stuff now */
    b.newFrame();
    ImGui::Button("test");
    b.drawFrame();
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(ImGui::GetCurrentContext(), b.context());

    /* Verify that event handlers also switch to proper context */

    MouseEvent left{Button::Left, {1, 2}, {}};
    a.handleMousePressEvent(left);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), a.context());

    b.handleMouseReleaseEvent(left);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), b.context());

    MouseScrollEvent scroll{{1.2f, -1.2f}, {}, {}};
    a.handleMouseScrollEvent(scroll);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), a.context());

    MouseEvent move{Button{}, {1, 2}, {}};
    b.handleMouseMoveEvent(move);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), b.context());

    KeyEvent tab{KeyEvent::Key::Tab, {}};
    a.handleKeyPressEvent(tab);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), a.context());

    b.handleKeyReleaseEvent(tab);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), b.context());

    TextInputEvent text{{"abc"}};
    a.handleTextInputEvent(text);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), a.context());
}

constexpr Color4 DrawClearColor{0.5f, 0.5f, 1.0f, 1.0f};

void ContextGLTest::drawSetup() {
    GL::Renderer::setClearColor(DrawClearColor);

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

void ContextGLTest::drawTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
}

void ContextGLTest::draw() {
    Context c{{200, 200}, {70, 70}, _framebuffer.viewport().size()};

    /* ImGui doesn't draw anything the first frame */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Utility::System::sleep(1);

    c.newFrame();

    /* Last drawlist that gets rendered, covers the entire display */
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    const ImVec2& size = ImGui::GetIO().DisplaySize;

    drawList->AddRectFilled({size.x*0.1f, size.y*0.2f}, {size.x*0.9f, size.y*0.8f},
        IM_COL32(255, 128, 128, 255));
    drawList->AddRectFilled({size.x*0.5f, size.y*0.5f}, size,
        /* Transparent to make sure alpha blending works (blend state and draw
           order shouldn't be changed by drawFrame()) */
        IM_COL32(255, 255, 255, 128));
    drawList->AddTriangleFilled({0.0f, 0.0f}, {size.x*0.5f, 0.0f}, {size.x*0.25f, size.y*0.5f},
        IM_COL32(128, 255, 128, 255));

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Catch also ABI and interface mismatch errors */
    if(!(_manager.load("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.load("PngImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / PngImporter plugin can't be loaded.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Path::join(IMGUIINTEGRATION_TEST_DIR, "ContextTestFiles/draw.png"),
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.5f}));
}

void ContextGLTest::drawTexture() {
    /* Catch also ABI and interface mismatch errors */
    if(!(_manager.load("PngImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("PngImporter plugin not found.");

    Context c{{200, 200}, {70, 70}, _framebuffer.viewport().size()};

    /* ImGui doesn't draw anything the first frame */
    c.newFrame();
    c.drawFrame();

    Containers::Pointer<Trade::AbstractImporter> importer = _manager.instantiate("PngImporter");

    CORRADE_VERIFY(importer->openFile(Utility::Path::join(IMGUIINTEGRATION_TEST_DIR, "ContextTestFiles/texture.png")));
    auto image = importer->image2D(0);
    CORRADE_VERIFY(image);
    CORRADE_COMPARE(image->format(), PixelFormat::RGB8Unorm);

    GL::Texture2D texture1;
    texture1.setImage(0, GL::TextureFormat::RGB, *image)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base);

    for(auto row: image->mutablePixels<Color3ub>())
    for(Color3ub& p: row)
        p = Color3ub{255} - p;

    GL::Texture2D texture2;
    texture2.setImage(0, GL::TextureFormat::RGB, *image)
        .setMagnificationFilter(GL::SamplerFilter::Nearest)
        .setMinificationFilter(GL::SamplerFilter::Nearest, GL::SamplerMipmap::Base);

    c.newFrame();

    /* Last drawlist that gets rendered, covers the entire display */
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    const ImVec2& size = ImGui::GetIO().DisplaySize;

    /* Full UV range */
    drawList->AddImage(static_cast<ImTextureID>(&texture1), {0.0f, 0.0f}, {size.x, size.y*0.5f});
    /* Custom UV rect */
    drawList->AddImage(static_cast<ImTextureID>(&texture2), {0.0f, size.y*0.5f}, size,
        {0.25f, 0.25f}, {1.0f, 0.75f});

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.load("AnyImageImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter plugin not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Path::join(IMGUIINTEGRATION_TEST_DIR, "ContextTestFiles/draw-texture.png"),
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.5f}));
}

void ContextGLTest::drawScissor() {
    Context c{{200, 200}, {70, 70}, _framebuffer.viewport().size()};

    /* ImGui doesn't draw anything the first frame */
    c.newFrame();
    c.drawFrame();

    Utility::System::sleep(1);

    c.newFrame();

    /* Last drawlist that gets rendered, covers the entire display */
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    const ImVec2& size = ImGui::GetIO().DisplaySize;

    drawList->PushClipRect({size.x*0.3f, size.y*0.2f}, {size.x*0.7f, size.y*0.8f});
    drawList->AddRectFilled({0.0f, 0.0f}, size, IM_COL32(255, 128, 128, 255));
    /* Restoring full scissor rect should work */
    drawList->PushClipRect({0.0f, 0.0f}, size);
    drawList->AddRectFilled({size.x*0.5f, size.y*0.5f}, size, IM_COL32(255, 255, 255, 128));
    drawList->PushClipRect({size.x*0.125f, size.y*0.125f}, size);
    drawList->AddTriangleFilled({0.0f, 0.0f}, {size.x*0.5f, 0.0f}, {size.x*0.25f, size.y*0.5f},
        IM_COL32(128, 255, 128, 255));
    /* Empty scissor rect should result in nothing being drawn */
    drawList->PushClipRect({0.0f, 0.0f}, {0.0f, 0.0f});
    drawList->AddCircleFilled({size.x*0.5f, size.y*0.5f}, size.x*0.25f,
        IM_COL32(255, 0, 0, 255));

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Catch also ABI and interface mismatch errors */
    if(!(_manager.load("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.load("PngImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / PngImporter plugin can't be loaded.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Path::join(IMGUIINTEGRATION_TEST_DIR, "ContextTestFiles/draw-scissor.png"),
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.5f}));

    /* Scissor should be reset to the framebuffer size, otherwise the next
       clear wouldn't cover the entire framebuffer */
    _framebuffer.clear(GL::FramebufferClear::Color);

    /* No need to use a .png for this. CompareImage allows StridedArrayView2D
       only for the actual value, so we can't use a view with stride 0 and have
       to fill an entire image instead. */
    Containers::Array<Color4ub> pixels{NoInit, size_t(_framebuffer.viewport().size().product())};
    for(Color4ub& p: pixels)
        p = Color4ub{DrawClearColor*255.0f};

    CORRADE_COMPARE_WITH(
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        (ImageView2D{PixelFormat::RGBA8Unorm, _framebuffer.viewport().size(), pixels}),
        (DebugTools::CompareImage{1.0f, 0.5f}));
}

void ContextGLTest::drawVertexOffset() {
    Context c{{200, 200}, {70, 70}, _framebuffer.viewport().size()};

    if(!(ImGui::GetIO().BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset))
        CORRADE_SKIP("Vertex offset not supported");

    if(sizeof(ImDrawIdx) != 2)
        CORRADE_SKIP("ImDrawIdx is too large");

    /* ImGui doesn't draw anything the first frame */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Utility::System::sleep(1);

    c.newFrame();

    /* Last drawlist that gets rendered, covers the entire display */
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    const ImVec2& size = ImGui::GetIO().DisplaySize;

    drawList->AddRectFilled({0.0f, 0.0f}, size, IM_COL32(255, 0, 0, 255));
    CORRADE_COMPARE(drawList->VtxBuffer.Size, 4);

    while(drawList->VtxBuffer.Size + 4 < std::numeric_limits<ImDrawIdx>::max()) {
        drawList->AddRectFilled({0.0f, 0.0f}, size, IM_COL32(255, 0, 0, 255));
    }
    drawList->AddRectFilled({0.0f, 0.0f}, size, IM_COL32(0, 255, 0, 255));

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<Color4ub> pixels{NoInit, size_t(_framebuffer.viewport().size().product())};
    for(Color4ub& p: pixels)
        p = Color4ub{0, 255, 0, 255};

    CORRADE_COMPARE_WITH(
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        (ImageView2D{PixelFormat::RGBA8Unorm, _framebuffer.viewport().size(), pixels}),
        (DebugTools::CompareImage{1.0f, 0.5f}));
}

void ContextGLTest::drawIndexOffset() {
    Context c{{200, 200}, {70, 70}, _framebuffer.viewport().size()};

    /* ImGui doesn't draw anything the first frame */
    c.newFrame();
    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Utility::System::sleep(1);

    c.newFrame();

    /* Last drawlist that gets rendered, covers the entire display */
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    const ImVec2& size = ImGui::GetIO().DisplaySize;

    /* The backend should use ImDrawCmd::IdxOffset instead of calculating the
       current index buffer offset by adding up ImDrawCmd::ElemCount. Swapping
       the draw commands will render the first rect last if IdxOffset is used
       directly. */
    drawList->AddRectFilled({0.0f, 0.0f}, size, IM_COL32(0, 255, 0, 255));
    /* Force the creation of a new draw command */
    drawList->AddDrawCmd();
    drawList->AddRectFilled({0.0f, 0.0f}, size, IM_COL32(255, 0, 0, 255));

    CORRADE_COMPARE(drawList->CmdBuffer.Size, 2);

    const ImDrawCmd cmd = drawList->CmdBuffer.back();
    drawList->CmdBuffer.pop_back();
    drawList->CmdBuffer.push_front(cmd);

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<Color4ub> pixels{NoInit, size_t(_framebuffer.viewport().size().product())};
    for(Color4ub& p: pixels)
        p = Color4ub{0, 255, 0, 255};

    CORRADE_COMPARE_WITH(
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}),
        (ImageView2D{PixelFormat::RGBA8Unorm, _framebuffer.viewport().size(), pixels}),
        (DebugTools::CompareImage{1.0f, 0.5f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::ImGuiIntegration::Test::ContextGLTest)
