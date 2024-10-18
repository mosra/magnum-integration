#ifndef Magnum_ImGuiIntegration_Context_hpp
#define Magnum_ImGuiIntegration_Context_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 ShaddyAQN <ShaddyAQN@gmail.com>
    Copyright © 2018 Tomáš Skřivan <skrivantomas@seznam.cz>
    Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 Guillaume Jacquemin <williamjcm@users.noreply.github.com>
    Copyright © 2019 Marco Melorio <m.melorio@icloud.com>
    Copyright © 2022, 2024 Pablo Escobar <mail@rvrs.in>
    Copyright © 2022 Stanislaw Halik <sthalik@misaki.pl>

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
 * @brief Template method implementations for input handling methods in @ref Magnum::ImGuiIntegration::Context
 */

#include "Magnum/ImGuiIntegration/visibility.h" /* defines IMGUI_API */

#include <imgui.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/ImGuiIntegration/Integration.h"
#include "Magnum/ImGuiIntegration/Context.h"

namespace Magnum { namespace ImGuiIntegration {

template<class KeyEvent> bool Context::handleKeyEvent(KeyEvent& event, bool value) {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    ImGuiIO &io = ImGui::GetIO();
    const typename KeyEvent::Modifiers modifiers = event.modifiers();

    #if IMGUI_VERSION_NUM >= 18823
    io.AddKeyEvent(ImGuiMod_Ctrl, modifiers >= KeyEvent::Modifier::Ctrl);
    io.AddKeyEvent(ImGuiMod_Shift, modifiers >= KeyEvent::Modifier::Shift);
    io.AddKeyEvent(ImGuiMod_Alt, modifiers >= KeyEvent::Modifier::Alt);
    io.AddKeyEvent(ImGuiMod_Super, modifiers >= KeyEvent::Modifier::Super);
    #else
    io.AddKeyEvent(ImGuiKey_ModCtrl, modifiers >= KeyEvent::Modifier::Ctrl);
    io.AddKeyEvent(ImGuiKey_ModShift, modifiers >= KeyEvent::Modifier::Shift);
    io.AddKeyEvent(ImGuiKey_ModAlt, modifiers >= KeyEvent::Modifier::Alt);
    io.AddKeyEvent(ImGuiKey_ModSuper, modifiers >= KeyEvent::Modifier::Super);
    #endif

    #ifndef DOXYGEN_GENERATING_OUTPUT /* it insists on documenting _c() */
    switch(event.key()) {
        /* LCOV_EXCL_START */
        #define _c(key, imgui) \
            case KeyEvent::Key::key: \
                io.AddKeyEvent(ImGuiKey_ ## imgui, value); \
                break;

        _c(Tab, Tab)
        _c(Left, LeftArrow)
        _c(Right, RightArrow)
        _c(Up, UpArrow)
        _c(Down, DownArrow)
        _c(PageUp, PageUp)
        _c(PageDown, PageDown)
        _c(Home, Home)
        _c(End, End)
        _c(Delete, Delete)
        _c(Backspace, Backspace)
        _c(Space, Space)
        _c(Enter, Enter)
        _c(Esc, Escape)
        _c(Insert, Insert)
        _c(Quote, Apostrophe)
        _c(Comma, Comma)
        _c(Minus, Minus)
        _c(Period, Period)
        _c(Slash, Slash)
        _c(Semicolon, Semicolon)
        _c(Equal, Equal)
        _c(LeftBracket, LeftBracket)
        _c(Backslash, Backslash)
        _c(RightBracket, RightBracket)
        _c(Backquote, GraveAccent)
        _c(CapsLock, CapsLock)
         _c(ScrollLock, ScrollLock)
        _c(NumLock, NumLock)
        _c(PrintScreen, PrintScreen)
        _c(Pause, Pause)
        _c(NumZero, Keypad0)
        _c(NumOne, Keypad1)
        _c(NumTwo, Keypad2)
        _c(NumThree, Keypad3)
        _c(NumFour, Keypad4)
        _c(NumFive, Keypad5)
        _c(NumSix, Keypad6)
        _c(NumSeven, Keypad7)
        _c(NumEight, Keypad8)
        _c(NumNine, Keypad9)
        _c(NumDecimal, KeypadDecimal)
        _c(NumDivide, KeypadDivide)
        _c(NumMultiply, KeypadMultiply)
        _c(NumSubtract, KeypadSubtract)
        _c(NumAdd, KeypadAdd)
        _c(NumEnter, KeypadEnter)
        _c(NumEqual, KeypadEqual)
        _c(LeftShift, LeftShift)
        _c(LeftCtrl, LeftCtrl)
        _c(LeftAlt, LeftAlt)
        _c(LeftSuper, LeftSuper)
        _c(RightShift, RightShift)
        _c(RightCtrl, RightCtrl)
        _c(RightAlt, RightAlt)
        _c(RightSuper, RightSuper)
        _c(Menu, Menu)
        _c(Zero, 0)
        _c(One, 1)
        _c(Two, 2)
        _c(Three, 3)
        _c(Four, 4)
        _c(Five, 5)
        _c(Six, 6)
        _c(Seven, 7)
        _c(Eight, 8)
        _c(Nine, 9)
        _c(A, A)
        _c(B, B)
        _c(C, C)
        _c(D, D)
        _c(E, E)
        _c(F, F)
        _c(G, G)
        _c(H, H)
        _c(I, I)
        _c(J, J)
        _c(K, K)
        _c(L, L)
        _c(M, M)
        _c(N, N)
        _c(O, O)
        _c(P, P)
        _c(Q, Q)
        _c(R, R)
        _c(S, S)
        _c(T, T)
        _c(U, U)
        _c(V, V)
        _c(W, W)
        _c(X, X)
        _c(Y, Y)
        _c(Z, Z)
        _c(F1, F1)
        _c(F2, F2)
        _c(F3, F3)
        _c(F4, F4)
        _c(F5, F5)
        _c(F6, F6)
        _c(F7, F7)
        _c(F8, F8)
        _c(F9, F9)
        _c(F10, F10)
        _c(F11, F11)
        _c(F12, F12)

        #undef _c
        /* LCOV_EXCL_STOP */

        /* Unknown key, do nothing */
        default: return false;
    }
    #endif

    return io.WantCaptureKeyboard;
}

/* Not all applications have Finger / Pen pointers, so employing a dirty SFINAE
   trick with an overload that returns false if given enum value is not
   available. Similar to a trick for working around missing Cursor enums
   below. */
namespace Implementation {

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_IMGUIINTEGRATION_OPTIONAL_POINTER(pointer)                   \
    template<class Pointer> constexpr bool is##pointer##Pointer(Pointer p, decltype(Pointer::pointer)* = nullptr) { \
        return p == Pointer::pointer;                                       \
    }                                                                       \
    constexpr bool is##pointer##Pointer(...) {                              \
        return false;                                                       \
    }
MAGNUM_IMGUIINTEGRATION_OPTIONAL_POINTER(Finger)
MAGNUM_IMGUIINTEGRATION_OPTIONAL_POINTER(Pen)
#undef MAGNUM_IMGUIINTEGRATION_OPTIONAL_POINTER
#endif

}

template<class PointerEvent> bool Context::handlePointerEvent(PointerEvent& event, bool value) {
    /* Ignore secondary touches, ImGui doesn't support multi-touch */
    if(!event.isPrimary())
        return false;

    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    ImGuiIO& io = ImGui::GetIO();
    const Vector2 position = event.position()*_eventScaling;

    ImGuiMouseButton buttonId;
    /* Finger and pen still reports as mouse left */
    if(event.pointer() == decltype(event.pointer())::MouseLeft ||
       Implementation::isFingerPointer(event.pointer()) ||
       Implementation::isPenPointer(event.pointer()))
        buttonId = ImGuiMouseButton_Left;
    else if(event.pointer() == decltype(event.pointer())::MouseRight)
        buttonId = ImGuiMouseButton_Right;
    else if(event.pointer() == decltype(event.pointer())::MouseMiddle)
        buttonId = ImGuiMouseButton_Middle;
    else
        /* Unknown button, do nothing */
        return false;

    io.AddMousePosEvent(position.x(), position.y());
    io.AddMouseButtonEvent(buttonId, value);

    return io.WantCaptureMouse;
}

template<class PointerEvent> bool Context::handlePointerPressEvent(PointerEvent& event) {
    return handlePointerEvent(event, true);
}

template<class PointerEvent> bool Context::handlePointerReleaseEvent(PointerEvent& event) {
    return handlePointerEvent(event, false);
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
template<class MouseEvent> bool Context::handleMouseEvent(MouseEvent& event, bool value) {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    ImGuiIO& io = ImGui::GetIO();
    const Vector2 position = Vector2(event.position())*_eventScaling;

    ImGuiMouseButton buttonId;
    switch(event.button()) {
        case MouseEvent::Button::Left:
            buttonId = ImGuiMouseButton_Left;
            break;
        case MouseEvent::Button::Right:
            buttonId = ImGuiMouseButton_Right;
            break;
        case MouseEvent::Button::Middle:
            buttonId = ImGuiMouseButton_Middle;
            break;

        /* Unknown button, do nothing */
        default: return false;
    }

    io.AddMousePosEvent(position.x(), position.y());
    io.AddMouseButtonEvent(buttonId, value);

    return io.WantCaptureMouse;
}

template<class MouseEvent> bool Context::handleMousePressEvent(MouseEvent& event) {
    return handleMouseEvent(event, true);
}

template<class MouseEvent> bool Context::handleMouseReleaseEvent(MouseEvent& event) {
    return handleMouseEvent(event, false);
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

template<class ScrollEvent> bool Context::handleScrollEvent(ScrollEvent& event) {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    ImGuiIO& io = ImGui::GetIO();
    const Vector2 position = event.position()*_eventScaling;

    io.AddMousePosEvent(position.x(), position.y());
    io.AddMouseWheelEvent(event.offset().x(), event.offset().y());

    return io.WantCaptureMouse;
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
template<class MouseScrollEvent> bool Context::handleMouseScrollEvent(MouseScrollEvent& event) {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    ImGuiIO& io = ImGui::GetIO();
    const Vector2 position = Vector2(event.position())*_eventScaling;

    io.AddMousePosEvent(position.x(), position.y());
    io.AddMouseWheelEvent(event.offset().x(), event.offset().y());

    return io.WantCaptureMouse;
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

template<class PointerMoveEvent> bool Context::handlePointerMoveEvent(PointerMoveEvent& event) {
    /* Ignore secondary touches, ImGui doesn't support multi-touch */
    if(!event.isPrimary())
        return false;

    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    ImGuiIO& io = ImGui::GetIO();
    const Vector2 position = event.position()*_eventScaling;

    /* If the event additionally changes the set of pressed buttons, try to
       translate that to ImGui as well */
    Containers::Optional<ImGuiMouseButton> buttonId;
    if(event.pointer()) {
        /* Finger and pen still reports as mouse left */
        if(*event.pointer() == decltype(*event.pointer())::MouseLeft ||
           Implementation::isFingerPointer(*event.pointer()) ||
           Implementation::isPenPointer(*event.pointer()))
            buttonId = ImGuiMouseButton_Left;
        else if(*event.pointer() == decltype(*event.pointer())::MouseRight)
            buttonId = ImGuiMouseButton_Right;
        else if(*event.pointer() == decltype(*event.pointer())::MouseMiddle)
            buttonId = ImGuiMouseButton_Middle;
    }

    io.AddMousePosEvent(position.x(), position.y());
    /* The button is pressed if it's contained in the set of currently
       pressed pointers. If event.pointer() is a NullOpt, this isn't
       reached. */
    if(buttonId)
        io.AddMouseButtonEvent(*buttonId, *event.pointer() <= event.pointers());

    return io.WantCaptureMouse;
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
template<class MouseMoveEvent> bool Context::handleMouseMoveEvent(MouseMoveEvent& event) {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    ImGuiIO& io = ImGui::GetIO();
    const Vector2 position = Vector2(event.position())*_eventScaling;

    io.AddMousePosEvent(position.x(), position.y());

    return io.WantCaptureMouse;
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

template<class KeyEvent> bool Context::handleKeyPressEvent(KeyEvent& event) {
    return handleKeyEvent(event, true);
}

template<class KeyEvent> bool Context::handleKeyReleaseEvent(KeyEvent& event) {
    return handleKeyEvent(event, false);
}

template<class TextInputEvent> bool Context::handleTextInputEvent(TextInputEvent& event) {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    ImGui::GetIO().AddInputCharactersUTF8(event.text().data());
    return false;
}

/* GLFW doesn't have all cursor types, so employing a dirty SFINAE trick that
   falls back to Arrow if given enum value is not available */
namespace Implementation {

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_IMGUIINTEGRATION_OPTIONAL_CURSOR(cursor)                     \
    template<class U> struct Optional##cursor##Cursor {                     \
        template<class T> constexpr static typename T::Cursor get(T*, decltype(T::Cursor::cursor)* = nullptr) { return T::Cursor::cursor; } \
        constexpr static typename U::Cursor get(...) { return U::Cursor::Arrow; } \
        public:                                                             \
            constexpr static typename U::Cursor Cursor = get(static_cast<U*>(nullptr)); \
    };
MAGNUM_IMGUIINTEGRATION_OPTIONAL_CURSOR(ResizeAll)
MAGNUM_IMGUIINTEGRATION_OPTIONAL_CURSOR(ResizeNESW)
MAGNUM_IMGUIINTEGRATION_OPTIONAL_CURSOR(ResizeNWSE)
MAGNUM_IMGUIINTEGRATION_OPTIONAL_CURSOR(No)
#undef MAGNUM_IMGUIINTEGRATION_OPTIONAL_CURSOR
#endif

}

template<class Application> void Context::updateApplicationCursor(Application& application) {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    switch(ImGui::GetMouseCursor()) {
        case ImGuiMouseCursor_TextInput:
            application.setCursor(Application::Cursor::TextInput);
            return;
        case ImGuiMouseCursor_ResizeAll:
            application.setCursor(Implementation::OptionalResizeAllCursor<Application>::Cursor);
            return;
        case ImGuiMouseCursor_ResizeNS:
            application.setCursor(Application::Cursor::ResizeNS);
            return;
        case ImGuiMouseCursor_ResizeEW:
            application.setCursor(Application::Cursor::ResizeWE);
            return;
        case ImGuiMouseCursor_ResizeNESW:
            application.setCursor(Implementation::OptionalResizeNESWCursor<Application>::Cursor);
            return;
        case ImGuiMouseCursor_ResizeNWSE:
            application.setCursor(Implementation::OptionalResizeNWSECursor<Application>::Cursor);
            return;
        case ImGuiMouseCursor_Hand:
            application.setCursor(Application::Cursor::Hand);
            return;
        case ImGuiMouseCursor_NotAllowed:
            application.setCursor(Implementation::OptionalNoCursor<Application>::Cursor);
            return;
        case ImGuiMouseCursor_None:
            application.setCursor(Application::Cursor::Hidden);
            return;

        /* For unknown cursors we set Arrow as well */
        case ImGuiMouseCursor_Arrow:
        default:
            application.setCursor(Application::Cursor::Arrow);
            return;
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

}}

#endif
