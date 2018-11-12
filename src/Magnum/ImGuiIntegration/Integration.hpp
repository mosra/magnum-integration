/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

/** @file
 * @brief Template method implementations for input handling methods in @ef Magnum::ImGuiIntegration::Context
 */

#include "Magnum/ImGuiIntegration/Integration.h"
#include "Magnum/ImGuiIntegration/Conversion.h"

#include <Corrade/Utility/Resource.h>
#include <imgui.h>

namespace Magnum { namespace ImGuiIntegration {

template<class KeyEvent> bool Context::handleKeyEvent(KeyEvent& event, bool value) {
    ImGuiIO &io = ImGui::GetIO();

    switch(event.key()) {
        case KeyEvent::Key::LeftShift:
        case KeyEvent::Key::RightShift:
            io.KeyShift = value;
            break;
        case KeyEvent::Key::LeftCtrl:
        case KeyEvent::Key::RightCtrl:
            io.KeyCtrl = value;
            break;
        case KeyEvent::Key::LeftAlt:
        case KeyEvent::Key::RightAlt:
            io.KeyAlt = value;
            break;
        case KeyEvent::Key::Tab:
            io.KeysDown[ImGuiKey_Tab] = value;
            break;
        case KeyEvent::Key::Up:
            io.KeysDown[ImGuiKey_UpArrow] = value;
            break;
        case KeyEvent::Key::Down:
            io.KeysDown[ImGuiKey_DownArrow] = value;
            break;
        case KeyEvent::Key::Left:
            io.KeysDown[ImGuiKey_LeftArrow] = value;
            break;
        case KeyEvent::Key::Right:
            io.KeysDown[ImGuiKey_RightArrow] = value;
            break;
        case KeyEvent::Key::Home:
            io.KeysDown[ImGuiKey_Home] = value;
            break;
        case KeyEvent::Key::End:
            io.KeysDown[ImGuiKey_End] = value;
            break;
        case KeyEvent::Key::PageUp:
            io.KeysDown[ImGuiKey_PageUp] = value;
            break;
        case KeyEvent::Key::PageDown:
            io.KeysDown[ImGuiKey_PageDown] = value;
            break;
        case KeyEvent::Key::Enter:
            io.KeysDown[ImGuiKey_Enter] = value;
            break;
        case KeyEvent::Key::Esc:
            io.KeysDown[ImGuiKey_Escape] = value;
            break;
        case KeyEvent::Key::Backspace:
            io.KeysDown[ImGuiKey_Backspace] = value;
            break;
        case KeyEvent::Key::Delete:
            io.KeysDown[ImGuiKey_Delete] = value;
            break;
        case KeyEvent::Key::A:
            io.KeysDown[ImGuiKey_A] = value;
            break;
        case KeyEvent::Key::C:
            io.KeysDown[ImGuiKey_C] = value;
            break;
        case KeyEvent::Key::V:
            io.KeysDown[ImGuiKey_V] = value;
            break;
        case KeyEvent::Key::X:
            io.KeysDown[ImGuiKey_X] = value;
            break;
        case KeyEvent::Key::Y:
            io.KeysDown[ImGuiKey_Y] = value;
            break;
        case KeyEvent::Key::Z:
            io.KeysDown[ImGuiKey_Z] = value;
            break;
        default:
            break;
    }

    return io.WantCaptureKeyboard;
}

template<class MouseEvent> bool Context::handleMouseEvent(MouseEvent& event, bool value) {
    switch(event.button()) {
        case MouseEvent::Button::Left:
            ImGui::GetIO().MouseDown[0] = value;
            break;
        case MouseEvent::Button::Right:
            ImGui::GetIO().MouseDown[1] = value;
            break;
        case MouseEvent::Button::Middle:
            ImGui::GetIO().MouseDown[2] = value;
            break;
        default:
            break;
    }
    return ImGui::GetIO().WantCaptureMouse;
}

template<class MouseEvent> bool Context::handleMousePressEvent(MouseEvent& event) {
    return handleMouseEvent(event, true);
}

template<class MouseEvent> bool Context::handleMouseReleaseEvent(MouseEvent& event) {
    return handleMouseEvent(event, false);
}

template<class MouseScrollEvent> bool Context::handleMouseScrollEvent(MouseScrollEvent& event) {
    ImGui::GetIO().MouseWheel += event.offset().y();
    ImGui::GetIO().MouseWheelH += event.offset().x();
    return ImGui::GetIO().WantCaptureMouse;
}

template<class MouseMoveEvent> bool Context::handleMouseMoveEvent(MouseMoveEvent& event) {
    ImGui::GetIO().MousePos = ImVec2(Vector2(event.position()));
    return ImGui::GetIO().WantCaptureMouse;
}

template<class KeyEvent> bool Context::handleKeyPressEvent(KeyEvent& event) {
    return handleKeyEvent(event, true);
}

template<class KeyEvent> bool Context::handleKeyReleaseEvent(KeyEvent& event) {
    return handleKeyEvent(event, false);
}

template<class TextInputEvent> bool Context::handleTextInputEvent(TextInputEvent& event) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharactersUTF8(event.text().data());
    return false;
}

}}
