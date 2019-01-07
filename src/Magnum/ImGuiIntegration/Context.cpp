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

#include "Context.h"

#include <cstring>
#include <imgui.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Extensions.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/Version.h>
#include <Magnum/ImageView.h>

#include "Magnum/ImGuiIntegration/Integration.h"

#ifdef MAGNUM_IMGUIINTEGRATION_BUILD_STATIC
static void importShaderResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumImGuiIntegrationShaders_RCS)
}
#endif

namespace Magnum { namespace ImGuiIntegration {

Context* Context::_instance = nullptr;

Context& Context::get() {
    CORRADE_ASSERT(_instance != nullptr,
        "ImGuiIntegration::Context::get(): no instance exists", *_instance);
    return *_instance;
}

/* Yes, this is godawful ugly, don't kill me for that plz */
Context::Context(const Vector2& size, const Vector2i& windowSize, const Vector2i& framebufferSize): Context{(ImGui::CreateContext(), *ImGui::GetCurrentContext()), size, windowSize, framebufferSize} {}

Context::Context(const Vector2i& size): Context{Vector2{size}, size, size} {}

Context::Context(ImGuiContext&, const Vector2& size, const Vector2i& windowSize, const Vector2i& framebufferSize) {
    CORRADE_ASSERT(_instance == nullptr,
        "ImGuiIntegration::Context: context already created", );

    ImGuiIO &io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab]        = ImGuiKey_Tab;
    io.KeyMap[ImGuiKey_LeftArrow]  = ImGuiKey_LeftArrow;
    io.KeyMap[ImGuiKey_RightArrow] = ImGuiKey_RightArrow;
    io.KeyMap[ImGuiKey_UpArrow]    = ImGuiKey_UpArrow;
    io.KeyMap[ImGuiKey_DownArrow]  = ImGuiKey_DownArrow;
    io.KeyMap[ImGuiKey_PageUp]     = ImGuiKey_PageUp;
    io.KeyMap[ImGuiKey_PageDown]   = ImGuiKey_PageDown;
    io.KeyMap[ImGuiKey_Home]       = ImGuiKey_Home;
    io.KeyMap[ImGuiKey_End]        = ImGuiKey_End;
    io.KeyMap[ImGuiKey_Delete]     = ImGuiKey_Delete;
    io.KeyMap[ImGuiKey_Backspace]  = ImGuiKey_Backspace;
    io.KeyMap[ImGuiKey_Enter]      = ImGuiKey_Enter;
    io.KeyMap[ImGuiKey_Escape]     = ImGuiKey_Escape;
    io.KeyMap[ImGuiKey_A]          = ImGuiKey_A;
    io.KeyMap[ImGuiKey_C]          = ImGuiKey_C;
    io.KeyMap[ImGuiKey_V]          = ImGuiKey_V;
    io.KeyMap[ImGuiKey_X]          = ImGuiKey_X;
    io.KeyMap[ImGuiKey_Y]          = ImGuiKey_Y;
    io.KeyMap[ImGuiKey_Z]          = ImGuiKey_Z;

    /** @todo Set clipboard text once Platform supports it */

    /* Set up framebuffer sizes, font supersampling etc. and upload the glyph
       cache */
    relayout(size, windowSize, framebufferSize);

    _mesh.setPrimitive(GL::MeshPrimitive::Triangles);
    _mesh.addVertexBuffer(
        _vertexBuffer, 0, Implementation::ImGuiShader::Position{},
        Implementation::ImGuiShader::TextureCoordinates{},
        Implementation::ImGuiShader::Color(Implementation::ImGuiShader::Color::Components::Four,
            Implementation::ImGuiShader::Color::DataType::UnsignedByte,
            Implementation::ImGuiShader::Color::DataOption::Normalized));

    _timeline.start();

    _instance = this;
}

Context::Context(ImGuiContext& context, const Vector2i& size): Context{context, Vector2{size}, size, size} {}

void Context::relayout(const Vector2& size, const Vector2i& windowSize, const Vector2i& framebufferSize) {
    /* If size of the UI is 1024x576 with a 16px font but it's rendered to a
       3840x2160 framebuffer, we need to supersample the font 3,75x to get
       crisp enough look. This is the same as in Magnum::Ui::UserInterface. */
    const Vector2 supersamplingRatio = Vector2(framebufferSize)/size;

    /* ImGui unfortunately expects that event coordinates == positioning
       coordinates, which means we have to scale the events like they would be
       related to `size` and not `windowSize`. */
    _eventScaling = size/Vector2{windowSize};

    ImGuiIO& io = ImGui::GetIO();

    /* If the supersampling ratio changed, we need to regenerate the font */
    if(_supersamplingRatio != supersamplingRatio) {
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
            .setStorage(1, GL::TextureFormat::RGBA8, image.size())
            .setSubImage(0, {}, image);

        /* Clear texture to save RAM, we have it on the GPU now */
        io.Fonts->ClearTexData();
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

Context::~Context() {
    CORRADE_INTERNAL_ASSERT(_instance == this);

    /* Not initialized/initialization failed */
    if(!_instance) return;

    ImGui::DestroyContext();

    _instance = nullptr;
}

void Context::newFrame() {
    _timeline.nextFrame();

    ImGui::GetIO().DeltaTime = _timeline.previousFrameDuration();

    ImGui::NewFrame();
}

void Context::drawFrame() {
    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();
    const Vector2 fbSize = Vector2{io.DisplaySize}*Vector2{io.DisplayFramebufferScale};
    if(!fbSize.product()) return;

    ImDrawData* drawData = ImGui::GetDrawData();
    CORRADE_INTERNAL_ASSERT(drawData); /* This is always valid after Render() */
    drawData->ScaleClipRects(io.DisplayFramebufferScale);

    const Matrix4 projection =
        Matrix4::translation({-1.0f, 1.0f, 0.0f})
        *Matrix4::scaling({2.0f/Vector2(io.DisplaySize), 1.0f})
        *Matrix4::scaling({1.0f, -1.0f, -1.0f});
    _shader.setProjectionMatrix(projection);

    for(std::int_fast32_t n = 0; n < drawData->CmdListsCount; ++n) {
        const ImDrawList* cmdList = drawData->CmdLists[n];
        ImDrawIdx indexBufferOffset = 0;

        _vertexBuffer.setData(
            {cmdList->VtxBuffer.Data, std::size_t(cmdList->VtxBuffer.Size)},
            GL::BufferUsage::StreamDraw);
        _indexBuffer.setData(
            {cmdList->IdxBuffer.Data, std::size_t(cmdList->IdxBuffer.Size)},
            GL::BufferUsage::StreamDraw);

        for(std::int_fast32_t c = 0; c < cmdList->CmdBuffer.Size; ++c) {
            const ImDrawCmd* pcmd = &cmdList->CmdBuffer[c];

            auto userTexture = static_cast<GL::Texture2D*>(pcmd->TextureId);
            _shader.bindTexture(userTexture ? *userTexture : _texture);

            GL::Renderer::setScissor(Range2Di{Range2D{
                {pcmd->ClipRect.x, fbSize.y() - pcmd->ClipRect.w},
                {pcmd->ClipRect.z, fbSize.y() - pcmd->ClipRect.y}}
                    .scaled(_supersamplingRatio)});

            _mesh.setCount(pcmd->ElemCount);
            _mesh.setIndexBuffer(_indexBuffer, indexBufferOffset*sizeof(ImDrawIdx),
                sizeof(ImDrawIdx) == 2
                ? GL::MeshIndexType::UnsignedShort
                : GL::MeshIndexType::UnsignedInt);

            indexBufferOffset += pcmd->ElemCount;

            _mesh.draw(_shader);
        }
    }
}

namespace Implementation {

ImGuiShader::ImGuiShader() {
    #ifdef MAGNUM_IMGUIINTEGRATION_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumImGuiIntegrationShaders"))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumImGuiIntegrationShaders");

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GL330, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    GL::Shader vert{version, GL::Shader::Type::Vertex};
    GL::Shader frag{version, GL::Shader::Type::Fragment};

    #ifndef MAGNUM_TARGET_GLES
    /* Enable explicit attribute locations in shader if they are supported,
       otherwise bind locations directly */
    if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version)) {
        vert.addSource({"#define EXPLICIT_ATTRIB_LOCATION\n"});
    } else {
        bindAttributeLocation(Position::Location, "position");
        bindAttributeLocation(TextureCoordinates::Location, "textureCoords");
        bindAttributeLocation(Color::Location, "color");
    }

    /* Enable explicit uniform locations if they are supported */
    if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version)) {
        vert.addSource({"#define EXPLICIT_UNIFORM_LOCATION\n"});
    }
    #endif

    vert.addSource(rs.get("ImGui.vert"));
    frag.addSource(rs.get("ImGui.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    _projMatrixUniform = uniformLocation("projectionMatrix");

    setUniform(uniformLocation("colorTexture"), TextureLayer);
}

}

}}
