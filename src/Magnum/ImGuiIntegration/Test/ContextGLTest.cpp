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

#include <sstream>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/System.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/GL/OpenGLTester.h>

#include "Magnum/ImGuiIntegration/Context.hpp"

namespace Magnum { namespace ImGuiIntegration { namespace Test { namespace {

enum class Modifier: Int {
    Shift, Ctrl, Alt, Super
};
typedef Containers::EnumSet<Modifier> Modifiers;

enum class Button: Int {
    Left, Middle, Right
};

struct MouseEvent {
    typedef Magnum::ImGuiIntegration::Test::Button Button;

    Button _button;
    Vector2i _position;
    Modifiers _modifiers;

    Button button() { return _button; }
    Vector2i position() { return _position; }
    Modifiers modifiers() { return _modifiers; }
};

struct MouseScrollEvent {
    Vector2 _offset;
    Vector2i _position;
    Modifiers _modifiers;

    Vector2 offset() { return _offset; }
    Vector2i position() { return _position; }
    Modifiers modifiers() { return _modifiers; }
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

struct KeyEvent {
    enum class Key: Int {
        Unknown,
        LeftShift, RightShift, LeftCtrl, RightCtrl,
        LeftAlt, RightAlt, LeftSuper, RightSuper,

        Enter, Esc,

        Up, Down, Left, Right, Home, End,
        PageUp, PageDown, Backspace, Insert, Delete,

        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        Space, Tab, Comma, Period, Minus, Plus, Slash, Percent, Smicolon, Equal,

        Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,

        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U,
        V, W, X, Y, Z,

        CapsLock, ScrollLock, NumLock, PrintScreen, Pause, Menu,

        NumZero, NumOne, NumTwo, NumThree, NumFour, NumFive,
        NumSix, NumSeven, NumEight, NumNine, NumDecimal, NumDivide,
        NumMultiply, NumSubtract, NumAdd, NumEnter, NumEqual,
    };

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

    /* In order to avoid imgui ignoring mouse clicks if both press and release
       happens in the same frame, the events are propagated to it only during
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
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{1.0f, 2.0f}));

    /* Scrolling/Mouse Wheel */
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheelH, 0.0f, Float);
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheel, 0.0f, Float);

    MouseScrollEvent scroll{{1.2f, -1.2f}, {17, 23}, Modifiers{}};
    c.handleMouseScrollEvent(scroll);
    CORRADE_COMPARE(Vector2(ImGui::GetMousePos()), (Vector2{17.0f, 23.0f}));
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheelH, 1.2f, Float);
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheel, -1.2f, Float);

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

    KeyEvent keyTab{KeyEvent::Key::Tab};
    c.handleKeyPressEvent(keyTab);
    CORRADE_VERIFY(ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Tab)));

    c.handleKeyReleaseEvent(keyTab);
    CORRADE_VERIFY(!ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Tab)));

    KeyEvent keyAlt{KeyEvent::Key::LeftAlt};
    c.handleKeyPressEvent(keyAlt);
    CORRADE_VERIFY(ImGui::GetIO().KeyAlt);

    /* Unknown keys shouldn't be propagated to imgui */
    KeyEvent unknownKey{KeyEvent::Key(666)};
    CORRADE_VERIFY(!c.handleKeyPressEvent(unknownKey));
    CORRADE_VERIFY(!c.handleKeyReleaseEvent(unknownKey));
}

void ContextGLTest::textInput() {
    Context c{{}};

    TextInputEvent textEvent{{"abc"}};
    c.handleTextInputEvent(textEvent);

    ImWchar expected[3]{'a', 'b', 'c'};
    /* This changed from InputCharacters to InputQueueCharacters in 1.67. Yes,
       it's a private API, but there's no other way to test if the event works
       correctly. It's used only in a test so I think that's acceptable. */
    CORRADE_COMPARE_AS(Containers::arrayView(ImGui::GetIO().InputQueueCharacters.begin(), ImGui::GetIO().InputQueueCharacters.size()),
        Containers::arrayView(expected),
        TestSuite::Compare::Container);
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

    MouseScrollEvent scroll{{1.2f, -1.2f}, {}, Modifiers{}};
    a.handleMouseScrollEvent(scroll);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), a.context());

    MouseEvent move{Button{}, {1, 2}, {}};
    b.handleMouseMoveEvent(move);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), b.context());

    KeyEvent tab{KeyEvent::Key::Tab};
    a.handleKeyPressEvent(tab);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), a.context());

    b.handleKeyReleaseEvent(tab);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), b.context());

    TextInputEvent text{{"abc"}};
    a.handleTextInputEvent(text);
    CORRADE_COMPARE(ImGui::GetCurrentContext(), a.context());
}

}}}}

CORRADE_TEST_MAIN(Magnum::ImGuiIntegration::Test::ContextGLTest)
