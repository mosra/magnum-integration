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

#include <sstream>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Magnum.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/OpenGLTester.h>

#include "Magnum/ImGuiIntegration/Integration.h"
#include "Magnum/ImGuiIntegration/Integration.hpp"
#include "Magnum/ImGuiIntegration/Widgets.h"

namespace Magnum { namespace ImGuiIntegration { namespace Test {

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

struct IntegrationGLTest: GL::OpenGLTester {
    explicit IntegrationGLTest();

    void construction();
    void get();

    void frame();
    void frameZeroSize();

    void mouseInput();
    void keyInput();
    void textInput();

    void widgets();
};

IntegrationGLTest::IntegrationGLTest() {
    addTests({&IntegrationGLTest::construction,
              &IntegrationGLTest::get,

              &IntegrationGLTest::frame,
              &IntegrationGLTest::frameZeroSize,

              &IntegrationGLTest::mouseInput,
              &IntegrationGLTest::keyInput,
              &IntegrationGLTest::textInput,

              &IntegrationGLTest::widgets});

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha, GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
}

void IntegrationGLTest::construction() {
    {
        Context c;
        MAGNUM_VERIFY_NO_GL_ERROR();
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Can't test the single-instance assertions because the internal assert
       fires every time */
}

void IntegrationGLTest::get() {
    {
        std::ostringstream out;
        Error redirectError{&out};

        Context::get();
        CORRADE_COMPARE(out.str(),
            "ImGuiIntegration::Context::get(): no instance exists\n");
    }

    {
        Context c;
        CORRADE_COMPARE(&Context::get(), &c);
    }
}

void IntegrationGLTest::frame() {
    Context c;

    c.newFrame({200, 200}, {200, 200});
    MAGNUM_VERIFY_NO_GL_ERROR();

    ImGui::Button("test");

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void IntegrationGLTest::frameZeroSize() {
    Context c;

    c.newFrame({0, 200}, {200, 0});
    MAGNUM_VERIFY_NO_GL_ERROR();

    ImGui::Button("test");

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void IntegrationGLTest::mouseInput() {
    Context c;

    /* Mouse Button */
    CORRADE_VERIFY(!ImGui::IsMouseDown(0)); /* left */
    CORRADE_VERIFY(!ImGui::IsMouseDown(1)); /* right */
    CORRADE_VERIFY(!ImGui::IsMouseDown(2)); /* middle */

    MouseEvent left{Button::Left, {1, 2}, {}};
    MouseEvent right{Button::Right, {1, 2}, {}};
    MouseEvent middle{Button::Middle, {1, 2}, {}};

    c.handleMousePressEvent(left);
    CORRADE_VERIFY(ImGui::IsMouseDown(0)); /* left */
    c.handleMousePressEvent(right);
    CORRADE_VERIFY(ImGui::IsMouseDown(1)); /* right */
    c.handleMousePressEvent(middle);
    CORRADE_VERIFY(ImGui::IsMouseDown(2)); /* middle */

    c.handleMouseReleaseEvent(right);
    CORRADE_VERIFY(!ImGui::IsMouseDown(1)); /* right */
    c.handleMouseReleaseEvent(left);
    CORRADE_VERIFY(!ImGui::IsMouseDown(0)); /* left */
    c.handleMouseReleaseEvent(middle);
    CORRADE_VERIFY(!ImGui::IsMouseDown(2)); /* middle */

    /* Mouse Movement */
    MouseEvent move{Button{}, {1, 2}, {}};
    c.handleMouseMoveEvent(move);
    const auto& mousePos = ImGui::GetMousePos();
    CORRADE_COMPARE(Vector2(mousePos.x, mousePos.y), Vector2(1.0f, 2.0f));

    /* Scrolling/Mouse Wheel */
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheelH, 0.0f, Float);
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheel, 0.0f, Float);

    MouseScrollEvent scroll{{1.2f, -1.2f}, {}, Modifiers{}};
    c.handleMouseScrollEvent(scroll);
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheelH, 1.2f, Float);
    CORRADE_COMPARE_AS(ImGui::GetIO().MouseWheel, -1.2f, Float);

    /* Unknown keys shouldn't be propagated to imgui */
    MouseEvent unknownButton{Button(666), {1, 2}, {}};
    CORRADE_VERIFY(!c.handleMousePressEvent(unknownButton));
    CORRADE_VERIFY(!c.handleMouseReleaseEvent(unknownButton));
}

void IntegrationGLTest::keyInput() {
    Context c;

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

void IntegrationGLTest::textInput() {
    Context c;

    TextInputEvent textEvent{{"abc"}};
    c.handleTextInputEvent(textEvent);

    ImWchar expected[4]{'a', 'b', 'c', '\0'};
    CORRADE_COMPARE_AS(Containers::arrayView(ImGui::GetIO().InputCharacters, 4),
        Containers::arrayView(expected),
        TestSuite::Compare::Container);
}

void IntegrationGLTest::widgets() {
    /* Checks compilation and no GL errors only */
    Context c;

    GL::Texture2D texture;
    texture.setStorage(1, GL::TextureFormat::RGB8, {1, 1});

    c.newFrame({200, 200}, {200, 200});
    image(texture, {100, 100});

    c.drawFrame();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

}}}

CORRADE_TEST_MAIN(Magnum::ImGuiIntegration::Test::IntegrationGLTest)
