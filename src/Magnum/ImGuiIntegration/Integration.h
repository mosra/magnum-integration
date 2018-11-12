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
 * @brief Class @ef Magnum::ImGuiIntegration::Context
 */

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Timeline.h>
#include <Magnum/GL/Attribute.h>

#include "Magnum/ImGuiIntegration/visibility.h"

namespace Magnum { namespace ImGuiIntegration {

namespace Implementation {

class ImGuiShader : public GL::AbstractShaderProgram {
    public:
        typedef GL::Attribute<0, Vector2> Position;
        typedef GL::Attribute<1, Vector2> TextureCoordinates;
        typedef GL::Attribute<2, Vector4> Color;

        explicit ImGuiShader();

        ImGuiShader& setProjectionMatrix(const Matrix4& matrix) {
            setUniform(_projMatrixUniform, matrix);
            return *this;
        }

        ImGuiShader& bindTexture(GL::Texture2D& texture) {
            texture.bind(TextureLayer);
            return *this;
        }

    private:
        enum: Int {
            TextureLayer = 0
        };

        Int _projMatrixUniform;
};

}

/*
@brief ImGui context

Handles initialization and destruction of ImGui context.

Use @ref newFrame() to initialize a ImGui frame and finally draw it with
@ref drawFrame() to the currently bound framebuffer.

@section ImGuiIntegration-Context-usage Usage

There should always only be one instance of @ref Context. As soon as this
one instance is created, you can access it via @ref Context::get().

Example:

@snippet ImGuiIntegration.cpp Context-usage

@section ImGuiIntegration-Context-rendering Rendering

ImGui requires @ref GL::Renderer::Feature::ScissorTest "scissor test"
to be enabled and @ref GL::Renderer::Feature::DepthTest "depth test"
to be disabled.

@ref GL::Renderer::Feature::DepthTest "Blending" should be enabled with
and set up as in the following snippet:

@snippet ImGuiIntegration.cpp Context-usage-per-frame
*/
class MAGNUM_IMGUIINTEGRATION_EXPORT Context {
    public:

        explicit Context();

        /** @brief Copying is not allowed */
        Context(const Context&) = delete;

        /** @brief Moving is not allowed */
        Context(Context&&) = delete;

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
         * @brief Start a new frame
         * @param winSize       Window size
         * @param viewportSize  Viewport size
         *
         * Initializes a new ImGui frame and updates window and viewport sizes.
         */
        void newFrame(const Vector2i& winSize, const Vector2i& viewportSize);

        /**
         * @brief Draw a frame
         *
         * Draw the frame created by ImGui calls since last call to @ref newFrame()
         * to currently bound framebuffer.
         *
         * See @ref ImGuiIntegration-Context-rendering for more information on
         * which rendering states to set before and after calling this method.
         */
        void drawFrame();

        /**
         * @brief Handle mouse press event
         * @param event The event to handle
         */
        template<class MouseEvent> bool handleMousePressEvent(MouseEvent& event);

        /**
         * @brief Handle mouse release event
         * @param event The event to handle
         */
        template<class MouseEvent> bool handleMouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Handle mouse scroll event
         * @param event The event to handle
         */
        template<class MouseScrollEvent> bool handleMouseScrollEvent(MouseScrollEvent& event);

        /**
         * @brief Handle mouse move event
         * @param event The event to handle
         */
        template<class MouseMoveEvent> bool handleMouseMoveEvent(MouseMoveEvent& event);

        /**
         * @brief Handle key press event
         * @param event The event to handle
         */
        template<class KeyEvent> bool handleKeyPressEvent(KeyEvent& event);

        /**
         * @brief Handle key release event
         * @param event The event to handle
         */
        template<class KeyEvent> bool handleKeyReleaseEvent(KeyEvent& event);

        /**
         * @brief Handle text input event
         * @param event The event to handle
         */
        template<class TextInputEvent> bool handleTextInputEvent(TextInputEvent& event);

    private:
        static Context* _instance;

        Implementation::ImGuiShader _shader;
        GL::Texture2D _texture;
        GL::Buffer _vertexBuffer{GL::Buffer::TargetHint::Array};
        GL::Buffer _indexBuffer{GL::Buffer::TargetHint::ElementArray};
        Timeline _timeline;
        GL::Mesh _mesh;

    private:
        template<class KeyEvent> bool handleKeyEvent(KeyEvent& event, bool value);
        template<class MouseEvent> bool handleMouseEvent(MouseEvent& event, bool value);
};

}}

#endif
