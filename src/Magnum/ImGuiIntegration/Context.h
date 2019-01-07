#ifndef Magnum_ImGuiIntegration_Integration_h
#define Magnum_ImGuiIntegration_Integration_h
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
 * @brief Class @ref Magnum::ImGuiIntegration::Context
 */

#include <Magnum/Timeline.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>

#include "Magnum/ImGuiIntegration/visibility.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
struct ImGuiContext;
#endif

namespace Magnum { namespace ImGuiIntegration {

namespace Implementation {

class ImGuiShader: public GL::AbstractShaderProgram {
    public:
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;
        typedef GL::Attribute<2, Vector4> Color;

        explicit ImGuiShader();

        ImGuiShader& setProjectionMatrix(const Matrix4& matrix);
        ImGuiShader& bindTexture(GL::Texture2D& texture);

    private:
        enum: Int {
            TextureLayer = 0
        };

        Int _projMatrixUniform;
};

}

/**
@brief ImGui context

Handles initialization and destruction of ImGui context and implements a
Magnum-based rendering backend.

Use @ref newFrame() to initialize a ImGui frame and finally draw it with
@ref drawFrame() to the currently bound framebuffer.

@section ImGuiIntegration-Context-usage Usage

There should always only be one instance of @ref Context. As soon as this
one instance is created, you can access it via @ref Context::get(). Example:

@snippet ImGuiIntegration.cpp Context-usage

@section ImGuiIntegration-Context-rendering Rendering

ImGui requires @ref GL::Renderer::Feature::ScissorTest "scissor test"
to be enabled and @ref GL::Renderer::Feature::DepthTest "depth test"
to be disabled. @ref GL::Renderer::Feature::Blending "Blending" should be
enabled and set up as in the following snippet:

@snippet ImGuiIntegration.cpp Context-usage-per-frame

@section ImGuiIntegration-Context-events Event handling

The templated @ref handleMousePressEvent(), @ref handleMouseReleaseEvent() etc.
functions are meant to be used inside event handlers of application classes
such as @ref Platform::Sdl2Application, directly passing the @p event parameter
to them. The returned value is then @cpp true @ce if ImGui used the event (and
thus it shouldn't be propagated further) and @cpp false @ce otherwise.
Implementation of those templated functions is provided in a separate
@ref ImGuiIntegration/Context.hpp file, don't forget to include it at the point
of use to avoid linker errors. Example:

@snippet ImGuiIntegration-sdl2.cpp Context-events

@section ImGuiIntegration-Context-dpi DPI awareness

There are three separate concepts for DPI-aware UI rendering:

-   UI size --- size of the user interface to which all widgets are positioned
-   Window size --- size of the window to which all input events are related
-   Framebuffer size --- size of the framebuffer the UI is being rendered to

Depending on the platform and use case, each of these three values can be
different. For example, a game menu screen can have the UI size the same
regardless of window size. Or on Retina macOS you can have different window and
framebuffer size and the UI size might be related to window size but
independent on the framebuffer size.

When using for example @ref Platform::Sdl2Application or other `*Application`
implementations, you usually have three values at your disposal ---
@ref Platform::Sdl2Application::windowSize() "windowSize()",
@ref Platform::Sdl2Application::framebufferSize() "framebufferSize()" and
@ref Platform::Sdl2Application::dpiScaling() "dpiScaling()". ImGui interfaces
are usually positioned with pixel units, getting more room on bigger windows.
A non-DPI-aware setup would be simply this:

@snippet ImGuiIntegration-sdl2.cpp Context-dpi-unaware

If you want the UI to keep a reasonable physical size and stay crisp with
different pixel densities, pass a ratio of window size and DPI scaling to the
UI size:

@snippet ImGuiIntegration-sdl2.cpp Context-dpi-aware

Finally, by clamping the first @p size parameter you can achieve various other
results like limiting it to a minimal / maximal area or have it fully scaled
with window size. When window size, framebuffer size or DPI scaling changes
(usually as a response to @ref Platform::Sdl2Application::viewportEvent() "viewportEvent()"),
call @ref relayout() with the new values. If the pixel density is changed, this
will result in the font caches being rebuilt.

@m_class{m-block m-success}

@par HiDPI fonts
    Note that there are further important steps for DPI awareness if you are
    supplying custom fonts. See the @ref Context(ImGuiContext&, const Vector2&, const Vector2i&, const Vector2i&)
    constructor for more infomation.
@par
    The default font used by ImGui, [Proggy Clean](https://www.dafont.com/proggy-clean.font),
    is a bitmap one, becoming rather blurry in larger sizes. It's recommended
    to switch to a different font for a crisper experience on HiDPI screens.

@experimental
*/
class MAGNUM_IMGUIINTEGRATION_EXPORT Context {
    public:
        /**
         * @brief Constructor
         * @param size                  Size of the user interface to which all
         *      widgets are positioned
         * @param windowSize            Size of the window to which all input
         *      events are related
         * @param framebufferSize       Size of the window framebuffer. On
         *      some platforms with HiDPI screens may be different from window
         *      size.
         *
         * Expects that no instance is created yet. This function creates the
         * ImGui context using @cpp ImGui::CreateContext() @ce and then queries
         * the font glyph cache from ImGui, uploading it to the GPU. If you
         * need to do some extra work on the context and before the font
         * texture gets uploaded, use @ref Context(ImGuiContext&, const Vector2&, const Vector2i&, const Vector2i&)
         * instead.
         *
         * The sizes are allowed to be zero in any dimension, but note that
         * specifying a concrete value later in @ref relayout() may trigger an
         * unnecessary rebuild of the font glyph cache due to different
         * calculated pixel density.
         * @see @ref Context(const Vector2i&),
         *      @ref relayout(const Vector2&, const Vector2i&, const Vector2i&)
         */
        explicit Context(const Vector2& size, const Vector2i& windowSize, const Vector2i& framebufferSize);

        /**
         * @brief Construct without DPI awareness
         *
         * Equivalent to calling @ref Context(const Vector2&, const Vector2i&, const Vector2i&)
         * with @p size passed to all three parameters.
         * @see @ref relayout(const Vector2i&)
         */
        explicit Context(const Vector2i& size);

        /**
         * @brief Construct from an existing context
         * @param context               Existing ImGui context
         * @param size                  Size of the user interface to which all
         *      widgets are positioned
         * @param windowSize            Size of the window to which all inputs
         *      events are related
         * @param framebufferSize       Size of the window framebuffer. On
         *      some platforms with HiDPI screens may be different from window
         *      size.
         *
         * Expects that no instance is created yet; takes ownership of the
         * passed context, deleting it on destruction. In comparison to
         * @ref Context(const Vector2&, const Vector2i&, const Vector2i&) this
         * constructor is useful if you need to do some work before the font
         * glyph cache gets uploaded to the GPU, for example adding custom
         * fonts.
         *
         * Note that, in order to have the custom fonts crisp, you have to
         * pre-scale their size by the ratio of @p size and @p framebufferSize.
         * If you don't do that, the fonts will appear tiny on HiDPI screens.
         * Example:
         *
         * @snippet ImGuiIntegration-sdl2.cpp Context-custom-fonts-dpi
         *
         * @see @ref Context(ImGuiContext&, const Vector2i&),
         *      @ref relayout(const Vector2&, const Vector2i&, const Vector2i&)
         */
        explicit Context(ImGuiContext& context, const Vector2& size, const Vector2i& windowSize, const Vector2i& framebufferSize);

        /**
         * @brief Construct from an existing context without DPI awareness
         *
         * Equivalent to calling @ref Context(ImGuiContext&, const Vector2&, const Vector2i&, const Vector2i&)
         * with @p size passed to the last three parameters. In comparison to
         * @ref Context(const Vector2i&) this constructor is useful if you need
         * to do some work before the font glyph cache gets uploaded to the
         * GPU, for example adding custom fonts:
         *
         * @snippet ImGuiIntegration.cpp Context-custom-fonts
         *
         * @see @ref relayout(const Vector2i&)
         */
        explicit Context(ImGuiContext& context, const Vector2i& size);

        /** @brief Copying is not allowed */
        Context(const Context&) = delete;

        /** @brief Moving is not allowed */
        Context(Context&&) = delete;

        /**
         * @brief Destructor
         *
         * Calls @cpp ImGui::DeleteContext() @ce.
         */
        ~Context();

        /** @brief Copying is not allowed */
        Context& operator=(const Context&) = delete;

        /** @brief Moving is not allowed */
        Context& operator=(Context&&) = delete;

        /**
         * @brief Global context instance
         *
         * Expects that an instance has been created.
         */
        static Context& get();

        /**
         * @brief Relayout the context
         *
         * Adapts the internal state for a new window size or pixel density. In
         * case the pixel density gets changed, font glyph caches are rebuilt
         * to match the new pixel density.
         *
         * The sizes are allowed to be zero in any dimension, but note that it
         * may trigger an unwanted rebuild of the font glyph cache due to
         * different calculated pixel density.
         *
         * If you supplied custom fonts via the @ref Context(ImGuiContext&, const Vector2&, const Vector2i&, const Vector2i&)
         * constructor and pixel density changed, in order to regenerate them
         * you have to clear the font atlas and re-add all fonts again with a
         * different scaling *before* calling this function, for example:
         *
         * @snippet ImGuiIntegration-sdl2.cpp Context-relayout-fonts-dpi
         *
         * If you don't do that, the fonts stay at the original scale, not
         * matching the new UI scaling anymore. If you didn't supply any custom
         * font, the function will reconfigure the builtin font automatically.
         */
        void relayout(const Vector2& size, const Vector2i& windowSize, const Vector2i& framebufferSize);

        /**
         * @brief Relayout the context
         *
         * Equivalent to calling @ref relayout(const Vector2&, const Vector2i&, const Vector2i&)
         * with @p size passed to all three parameters.
         */
        void relayout(const Vector2i& size);

        /**
         * @brief Start a new frame
         *
         * Initializes a new ImGui frame using @cpp ImGui::NewFrame() @ce.
         */
        void newFrame();

        /**
         * @brief Draw a frame
         *
         * Calls @cpp ImGui::Render() @ce and then draws the frame created by
         * ImGui calls since last call to @ref newFrame() to currently bound
         * framebuffer.
         *
         * See @ref ImGuiIntegration-Context-rendering for more information on
         * which rendering states to set before and after calling this method.
         */
        void drawFrame();

        /**
         * @brief Handle mouse press event
         *
         * Returns @cpp true @ce if ImGui wants to capture the mouse (so the
         * event shouldn't be further propagated to the rest of the
         * application), @cpp false @ce otherwise.
         */
        template<class MouseEvent> bool handleMousePressEvent(MouseEvent& event);

        /**
         * @brief Handle mouse release event
         *
         * Returns @cpp true @ce if ImGui wants to capture the mouse (so the
         * event shouldn't be further propagated to the rest of the
         * application), @cpp false @ce otherwise.
         */
        template<class MouseEvent> bool handleMouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Handle mouse scroll event
         *
         * Returns @cpp true @ce if ImGui wants to capture the mouse (so the
         * event shouldn't be further propagated to the rest of the
         * application), @cpp false @ce otherwise.
         */
        template<class MouseScrollEvent> bool handleMouseScrollEvent(MouseScrollEvent& event);

        /**
         * @brief Handle mouse move event
         *
         * Returns @cpp true @ce if ImGui wants to capture the mouse (so the
         * event shouldn't be further propagated to the rest of the
         * application), @cpp false @ce otherwise.
         */
        template<class MouseMoveEvent> bool handleMouseMoveEvent(MouseMoveEvent& event);

        /**
         * @brief Handle key press event
         *
         * Returns @cpp true @ce if ImGui wants to capture the keyboard (so the
         * event shouldn't be further propagated to the rest of the
         * application), @cpp false @ce otherwise.
         */
        template<class KeyEvent> bool handleKeyPressEvent(KeyEvent& event);

        /**
         * @brief Handle key release event
         *
         * Returns @cpp true @ce if ImGui wants to capture the keyboard (so the
         * event shouldn't be further propagated to the rest of the
         * application), @cpp false @ce otherwise.
         */
        template<class KeyEvent> bool handleKeyReleaseEvent(KeyEvent& event);

        /**
         * @brief Handle text input event
         *
         * Returns @cpp true @ce if ImGui wants to capture the input (so the
         * event shouldn't be further propagated to the rest of the
         * application), @cpp false @ce otherwise.
         */
        template<class TextInputEvent> bool handleTextInputEvent(TextInputEvent& event);

    private:
        static Context* _instance;

        Implementation::ImGuiShader _shader;
        GL::Texture2D _texture{NoCreate};
        GL::Buffer _vertexBuffer{GL::Buffer::TargetHint::Array};
        GL::Buffer _indexBuffer{GL::Buffer::TargetHint::ElementArray};
        Timeline _timeline;
        GL::Mesh _mesh;
        Vector2 _supersamplingRatio,
            _eventScaling;

    private:
        template<class KeyEvent> bool handleKeyEvent(KeyEvent& event, bool value);
        template<class MouseEvent> bool handleMouseEvent(MouseEvent& event, bool value);
};

}}

#endif
