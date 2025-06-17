/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 ShaddyAQN <ShaddyAQN@gmail.com>
    Copyright © 2018 Tomáš Skřivan <skrivantomas@seznam.cz>
    Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 bowling-allie <allie.smith.epic@gmail.com>
    Copyright © 2021 Juan Pedro Bolívar Puente <raskolnikov@gnu.org>
    Copyright © 2022, 2024 Pablo Escobar <mail@rvrs.in>
    Copyright © 2024 kolbbond <kolbbond@gmail.com>

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

#include "Context.h"

#include <cstring>
#include <imgui.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/ImageView.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Extensions.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>

#include "Magnum/ImGuiIntegration/Integration.h"
#include "Magnum/ImGuiIntegration/Widgets.h"

namespace Magnum { namespace ImGuiIntegration {

Context::Context(const Vector2& size, const Vector2i& windowSize, const Vector2i& framebufferSize): Context{*ImGui::CreateContext(), size, windowSize, framebufferSize} {}

Context::Context(const Vector2i& size): Context{Vector2{size}, size, size} {}

Context::Context(ImGuiContext& context, const Vector2& size, const Vector2i& windowSize, const Vector2i& framebufferSize):
    _context{&context},
    _shader{Shaders::FlatGL2D::Configuration{}
        .setFlags(Shaders::FlatGL2D::Flag::Textured|Shaders::FlatGL2D::Flag::VertexColor)}
{
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(&context);

    ImGuiIO &io = ImGui::GetIO();

    /* Tell ImGui that changing mouse cursors is supported */
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

    /* Check if we can support base vertex > 0 in draw commands */
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    if(
    #if defined(MAGNUM_TARGET_WEBGL)
        GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::draw_instanced_base_vertex_base_instance>()
    #elif defined(MAGNUM_TARGET_GLES)
        /* Supported since GLES 3.2 but the extensions aren't fully covered by
           3.2 (and hence not in core) so we still need the version check */
        GL::Context::current().isVersionSupported(GL::Version::GLES320) ||
        GL::Context::current().isExtensionSupported<GL::Extensions::OES::draw_elements_base_vertex>() ||
        GL::Context::current().isExtensionSupported<GL::Extensions::EXT::draw_elements_base_vertex>()
    #else
        /* Core since OpenGL 3.2 */
        GL::Context::current().isExtensionSupported<GL::Extensions::ARB::draw_elements_base_vertex>()
    #endif
    ) {
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    }
    #endif

    /* Set up framebuffer sizes, font supersampling etc. and upload the glyph
       cache */
    relayout(size, windowSize, framebufferSize);

    _mesh.setPrimitive(GL::MeshPrimitive::Triangles);
    _mesh.addVertexBuffer(_vertexBuffer, 0,
        Shaders::FlatGL2D::Position{},
        Shaders::FlatGL2D::TextureCoordinates{},
        Shaders::FlatGL2D::Color4{
            Shaders::FlatGL2D::Color4::DataType::UnsignedByte,
            Shaders::FlatGL2D::Color4::DataOption::Normalized});

    _timeline.start();
}

Context::Context(ImGuiContext& context, const Vector2i& size): Context{context, Vector2{size}, size, size} {}

Context::Context(NoCreateT) noexcept: _context{nullptr}, _shader{NoCreate}, _texture{NoCreate}, _vertexBuffer{NoCreate}, _indexBuffer{NoCreate}, _mesh{NoCreate} {}

Context::Context(Context&& other) noexcept: _context{other._context}, _shader{Utility::move(other._shader)}, _texture{Utility::move(other._texture)}, _vertexBuffer{Utility::move(other._vertexBuffer)}, _indexBuffer{Utility::move(other._indexBuffer)}, _timeline{Utility::move(other._timeline)}, _mesh{Utility::move(other._mesh)}, _supersamplingRatio{other._supersamplingRatio}, _eventScaling{other._eventScaling} {
    other._context = nullptr;
}

Context::~Context() {
    if(_context) {
        /* Ensure we destroy the context we're linked to */
        ImGui::SetCurrentContext(_context);
        ImGui::DestroyContext();
    }
}

Context& Context::operator=(Context&& other) noexcept {
    using Utility::swap;
    swap(_context, other._context);
    swap(_shader, other._shader);
    swap(_texture, other._texture);
    swap(_vertexBuffer, other._vertexBuffer);
    swap(_indexBuffer, other._indexBuffer);
    swap(_timeline, other._timeline);
    swap(_mesh, other._mesh);
    swap(_supersamplingRatio, other._supersamplingRatio);
    swap(_eventScaling, other._eventScaling);
    return *this;
}

ImGuiContext* Context::release() {
    ImGuiContext* context = _context;
    _context = nullptr;
    return context;
}

void Context::relayout(const Vector2& size, const Vector2i& windowSize, const Vector2i& framebufferSize) {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    /* If size of the UI is 1024x576 with a 16px font but it's rendered to a
       3840x2160 framebuffer, we need to supersample the font 3,75x to get
       crisp enough look. This is the same as in Magnum::Ui::UserInterface. */
    const Vector2 supersamplingRatio = Vector2(framebufferSize)/size;

    /* ImGui unfortunately expects that event coordinates == positioning
       coordinates, which means we have to scale the events like they would be
       related to `size` and not `windowSize`. */
    _eventScaling = size/Vector2{windowSize};

    ImGuiIO& io = ImGui::GetIO();

    /* If the supersampling ratio changed, we need to regenerate the font. Do
       that also if the fonts are not loaded yet -- that means these were
       supplied by the user after Context was created (or after last call to
       relayout()). */
    bool allFontsLoaded = !io.Fonts->Fonts.empty();
    for(auto& font: io.Fonts->Fonts) if(!font->IsLoaded()) {
        allFontsLoaded = false;
        break;
    }
    if(_supersamplingRatio != supersamplingRatio || !allFontsLoaded) {
        /* Need to use > 0.0f instead of just != 0 so we catch NaNs too */
        const Float nonZeroSupersamplingRatio = (supersamplingRatio.x() > 0.0f ? supersamplingRatio.x() : 1.0f);

        /* If there's no fonts yet (first run) or only one font and it's the
           one we set earier (has the [SCALED] suffix), wipe it and replace
           with a differently scaled version. Otherwise assume the fonts are
           user-supplied, do not touch them and just rebuild the cache. */
        if(io.Fonts->Fonts.empty() || (io.Fonts->Fonts.size() == 1 && std::strcmp(io.Fonts->Fonts[0]->GetDebugName(), "ProggyClean.ttf, 13px [SCALED]") == 0)) {
            /* Clear all fonts. Can't just replace the default font,
               unfortunately */
            io.Fonts->Clear();

            /* Because ImGui doesn't have native HiDPI support yet, we upscale
               the font for glyph prerendering and then downscale it back for
               the UI */
            ImFontConfig cfg;
            std::strcpy(cfg.Name, "ProggyClean.ttf, 13px [SCALED]");
            cfg.SizePixels = 13.0f*nonZeroSupersamplingRatio;
            io.Fonts->AddFontDefault(&cfg);
        }

        _supersamplingRatio = supersamplingRatio;

        /* Downscale back the upscaled font to achieve supersampling */
        io.FontGlobalScale = 1.0f/nonZeroSupersamplingRatio;

        unsigned char *pixels;
        int width, height;
        int pixelSize;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &pixelSize);
        CORRADE_INTERNAL_ASSERT(width > 0 && height > 0 && pixelSize == 4);

        ImageView2D image{GL::PixelFormat::RGBA,
            GL::PixelType::UnsignedByte, {width, height},
            {pixels, std::size_t(pixelSize*width*height)}};

        _texture = GL::Texture2D{};
        _texture.setMagnificationFilter(GL::SamplerFilter::Linear)
            .setMinificationFilter(GL::SamplerFilter::Linear)
            #ifndef MAGNUM_TARGET_GLES2
            .setStorage(1, GL::TextureFormat::RGBA8, image.size())
            .setSubImage(0, {}, image)
            #else
            .setImage(0, GL::TextureFormat::RGBA, image)
            #endif
            ;

        /* Clear texture to save RAM, we have it on the GPU now */
        io.Fonts->ClearTexData();

        /* Make the texture available through the ImFontAtlas */
        io.Fonts->SetTexID(textureId(_texture));
    }

    /* Display size is the window size. Scaling of this to the actual window
       and framebuffer size is done on the magnum side when rendering. */
    io.DisplaySize = ImVec2(Vector2(size));
    /* io.DisplayFramebufferScale is currently not used by imgui (1.66b), so
       why bother */
    /** @todo revisit when there's progress on https://github.com/ocornut/imgui/issues/1676 */
}

void Context::relayout(const Vector2i& size) {
    relayout(Vector2{size}, size, size);
}

void Context::newFrame() {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    _timeline.nextFrame();

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = _timeline.previousFrameDuration();
    /* Since v1.68 and https://github.com/ocornut/imgui/commit/3c07ec6a6126fb6b98523a9685d1f0f78ca3c40c,
       ImGui disallows zero delta time to "prevent subtle issues".
       Unfortunately that *does* cause subtle issues, especially in combination
       with SDL2 on Windows -- when the window is being dragged across the
       screen, SDL temporarily halts all event processing and then fires all
       pending events at once, causing zero delta time. A bugreport for this
       is opened since 2016 -- https://bugzilla.libsdl.org/show_bug.cgi?id=2077
       but there was nothing done last time I checked (March 2020). More info
       also at https://github.com/mosra/magnum-integration/issues/57 */
    if(ImGui::GetFrameCount() != 0)
        io.DeltaTime = Math::max(io.DeltaTime, std::numeric_limits<float>::epsilon());

    ImGui::NewFrame();
}

void Context::drawFrame() {
    /* Ensure we use the context we're linked to */
    ImGui::SetCurrentContext(_context);

    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();
    const Vector2 fbSize = Vector2{io.DisplaySize}*Vector2{io.DisplayFramebufferScale};
    if(!fbSize.product()) return;

    ImDrawData* drawData = ImGui::GetDrawData();
    CORRADE_INTERNAL_ASSERT(drawData); /* This is always valid after Render() */
    drawData->ScaleClipRects(io.DisplayFramebufferScale);

    const Matrix3 projection =
        Matrix3::translation({-1.0f, 1.0f})*
        Matrix3::scaling({2.0f/Vector2(io.DisplaySize)})*
        Matrix3::scaling({1.0f, -1.0f});
    _shader.setTransformationProjectionMatrix(projection);

    for(std::int_fast32_t n = 0; n < drawData->CmdListsCount; ++n) {
        const ImDrawList* cmdList = drawData->CmdLists[n];

        _vertexBuffer.setData(
            {cmdList->VtxBuffer.Data, std::size_t(cmdList->VtxBuffer.Size)},
            GL::BufferUsage::StreamDraw);
        _indexBuffer.setData(
            {cmdList->IdxBuffer.Data, std::size_t(cmdList->IdxBuffer.Size)},
            GL::BufferUsage::StreamDraw);

        for(std::int_fast32_t c = 0; c < cmdList->CmdBuffer.Size; ++c) {
            const ImDrawCmd* pcmd = &cmdList->CmdBuffer[c];

            if(pcmd->UserCallback) {
                /* User callback, registered via ImDrawList::AddCallback().
                   ImDrawCallback_ResetRenderState is a special callback value
                   used by the user to request the renderer to reset render
                   state. We do not have anything to do here though. */
                if(pcmd->UserCallback != ImDrawCallback_ResetRenderState)
                    pcmd->UserCallback(cmdList, pcmd);
                continue;
            }

            GL::Renderer::setScissor(Range2Di{Range2D{
                {pcmd->ClipRect.x, fbSize.y() - pcmd->ClipRect.w},
                {pcmd->ClipRect.z, fbSize.y() - pcmd->ClipRect.y}}
                    .scaled(_supersamplingRatio)});

            /* Only > 0 if ImGuiBackendFlags_RendererHasVtxOffset is set */
            _mesh.setBaseVertex(pcmd->VtxOffset);
            _mesh.setCount(pcmd->ElemCount);
            _mesh.setIndexBuffer(_indexBuffer, pcmd->IdxOffset*sizeof(ImDrawIdx),
                sizeof(ImDrawIdx) == 2
                ? GL::MeshIndexType::UnsignedShort
                : GL::MeshIndexType::UnsignedInt);

            /* We're storing just texture IDs, so make a non-owning instance
               around it, and assume it's already created */
            GL::Texture2D texture = GL::Texture2D::wrap(
                #if IMGUI_VERSION_NUM >= 19131
                pcmd->TextureId,
                #else
                reinterpret_cast<std::uintptr_t>(pcmd->GetTexID()),
                #endif
                GL::ObjectFlag::Created);

            _shader
                .bindTexture(texture)
                .draw(_mesh);
        }
    }

    /* Reset scissor rectangle back to the full framebuffer size. Instead the
       users would be required to disable the scissor right after as otherwise
       the framebuffer clear would only happen on whatever the last scissor
       was. (And I hope the floating-point precision is enough here.) */
    GL::Renderer::setScissor(Range2Di{Range2D{{}, fbSize}.scaled(_supersamplingRatio)});
}

}}
