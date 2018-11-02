// The MIT License (MIT)
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or distribute
// this software, either in source code form or as a compiled binary, for any
// purpose, commercial or non - commercial, and by any means.
//
// In jurisdictions that recognize copyright laws, the author or authors of
// this software dedicate any and all copyright interest in the software to
// the public domain.We make this dedication for the benefit of the public
// at large and to the detriment of our heirs and successors.We intend this
// dedication to be an overt act of relinquishment in perpetuity of all
// present and future rights to this software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL
// THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "MagnumImGui.h"

#include <Corrade/Utility/Resource.h>

#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Extensions.h>

#include <imgui.h>

using namespace Magnum;

void MagnumImGui::init() {
  ImGui::CreateContext();
  ImGuiIO &io                    = ImGui::GetIO();
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

  //   io.SetClipboardTextFn = ;
  //   io.GetClipboardTextFn = ;

  mTimeline.start();
}

void MagnumImGui::load() {
  ImGuiIO &      io = ImGui::GetIO();
  unsigned char *pixels;
  int            width, height;
  int            pixel_size;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &pixel_size);

  ImageView2D image{ GL::PixelFormat::RGBA,
					GL::PixelType::UnsignedByte,
                    {width, height},
                    {pixels, std::size_t(pixel_size * width * height)}};

  mTexture.setMagnificationFilter(GL::SamplerFilter::Linear)
      .setMinificationFilter(GL::SamplerFilter::Linear)
      .setStorage(0, GL::TextureFormat::RGBA, image.size())
      .setImage(0, GL::TextureFormat::RGBA, image);

  mMesh.setPrimitive(GL::MeshPrimitive::Triangles);
  mMesh.addVertexBuffer(
      mVertexBuffer, 0, ImguiShader::Position{},
      ImguiShader::TextureCoordinates{},
      ImguiShader::Color(ImguiShader::Color::Components::Four,
                         ImguiShader::Color::DataType::UnsignedByte,
                         ImguiShader::Color::DataOption::Normalized));
}

bool MagnumImGui::keyPressReleaseEvent(
    const Platform::Application::KeyEvent &event, bool value) {
  ImGuiIO &io = ImGui::GetIO();

  switch (event.key()) {
  case Platform::Application::KeyEvent::Key::LeftShift:
    io.KeyShift = value;
    break;
  case Platform::Application::KeyEvent::Key::RightShift:
    io.KeyShift = value;
    break;
  case Platform::Application::KeyEvent::Key::LeftCtrl:
    io.KeyCtrl = value;
    break;
  case Platform::Application::KeyEvent::Key::RightCtrl:
    io.KeyCtrl = value;
    break;
  case Platform::Application::KeyEvent::Key::LeftAlt:
    io.KeyAlt = value;
    break;
  case Platform::Application::KeyEvent::Key::RightAlt:
    io.KeyAlt = value;
    break;
  case Platform::Application::KeyEvent::Key::Tab:
    io.KeysDown[ImGuiKey_Tab] = value;
    break;
  case Platform::Application::KeyEvent::Key::Up:
    io.KeysDown[ImGuiKey_UpArrow] = value;
    break;
  case Platform::Application::KeyEvent::Key::Down:
    io.KeysDown[ImGuiKey_DownArrow] = value;
    break;
  case Platform::Application::KeyEvent::Key::Left:
    io.KeysDown[ImGuiKey_LeftArrow] = value;
    break;
  case Platform::Application::KeyEvent::Key::Right:
    io.KeysDown[ImGuiKey_RightArrow] = value;
    break;
  case Platform::Application::KeyEvent::Key::Home:
    io.KeysDown[ImGuiKey_Home] = value;
    break;
  case Platform::Application::KeyEvent::Key::End:
    io.KeysDown[ImGuiKey_End] = value;
    break;
  case Platform::Application::KeyEvent::Key::PageUp:
    io.KeysDown[ImGuiKey_PageUp] = value;
    break;
  case Platform::Application::KeyEvent::Key::PageDown:
    io.KeysDown[ImGuiKey_PageDown] = value;
    break;
  case Platform::Application::KeyEvent::Key::Enter:
    io.KeysDown[ImGuiKey_Enter] = value;
    break;
  case Platform::Application::KeyEvent::Key::Esc:
    io.KeysDown[ImGuiKey_Escape] = value;
    break;
  case Platform::Application::KeyEvent::Key::Backspace:
    io.KeysDown[ImGuiKey_Backspace] = value;
    break;
  case Platform::Application::KeyEvent::Key::Delete:
    io.KeysDown[ImGuiKey_Delete] = value;
    break;
  case Platform::Application::KeyEvent::Key::A:
    io.KeysDown[ImGuiKey_A] = value;
    break;
  case Platform::Application::KeyEvent::Key::C:
    io.KeysDown[ImGuiKey_C] = value;
    break;
  case Platform::Application::KeyEvent::Key::V:
    io.KeysDown[ImGuiKey_V] = value;
    break;
  case Platform::Application::KeyEvent::Key::X:
    io.KeysDown[ImGuiKey_X] = value;
    break;
  case Platform::Application::KeyEvent::Key::Y:
    io.KeysDown[ImGuiKey_Y] = value;
    break;
  case Platform::Application::KeyEvent::Key::Z:
    io.KeysDown[ImGuiKey_Z] = value;
    break;
  default:
    break;
  }

  return io.WantCaptureKeyboard;
}

bool MagnumImGui::mousePressReleaseEvent(
    const Platform::Application::MouseEvent &event, bool value) {
  switch (event.button()) {
  case Magnum::Platform::Application::MouseEvent::Button::Left:
    mMousePressed[0] = value;
    break;
  case Magnum::Platform::Application::MouseEvent::Button::Right:
    mMousePressed[1] = value;
    break;
  case Magnum::Platform::Application::MouseEvent::Button::Middle:
    mMousePressed[2] = value;
    break;
  default:
    break;
  }
  return ImGui::GetIO().WantCaptureMouse;
}

MagnumImGui::MagnumImGui() {
  init();
  load();
}

MagnumImGui::~MagnumImGui() { ImGui::DestroyContext(); }

void MagnumImGui::newFrame(const Vector2i &winSize,
                           const Vector2i &viewportSize) {
  mTimeline.nextFrame();

  ImGuiIO &io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  io.DisplaySize = ImVec2((float)winSize.x(), (float)winSize.y());
  io.DisplayFramebufferScale =
      ImVec2(winSize.x() > 0 ? ((float)viewportSize.x() / winSize.x()) : 0,
             winSize.y() > 0 ? ((float)viewportSize.y() / winSize.y()) : 0);

  io.DeltaTime = mTimeline.previousFrameDuration();
  io.MousePos  = ImVec2(float(mMousePos.x()), float(mMousePos.y()));

  for (int i = 0; i < 3; i++) {
    io.MouseDown[i] = mMousePressed[i];
  }

  io.MouseWheel = mMouseScroll;
  mMouseScroll  = 0.0f;

  ImGui::NewFrame();
}

void MagnumImGui::drawFrame() {
  ImGui::Render();

  ImGuiIO &io        = ImGui::GetIO();
  int      fb_width  = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
  int      fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
  if (fb_width == 0 || fb_height == 0)
    return;

  auto draw_data = ImGui::GetDrawData();
  if (!draw_data)
    return;

  draw_data->ScaleClipRects(io.DisplayFramebufferScale);

  GL::Renderer::enable(GL::Renderer::Feature::Blending);
  GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add,
                             Magnum::GL::Renderer::BlendEquation::Add);
  GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
	  GL::Renderer::BlendFunction::OneMinusSourceAlpha);
  GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
  GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
  GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);

  const Matrix4 ortho_projection{
      {2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f},
      {0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f},
      {0.0f, 0.0f, -1.0f, 0.0f},
      {-1.0f, 1.0f, 0.0f, 1.0f},
  };
  mShader.setProjectMatrix(ortho_projection);

  for (int n = 0; n < draw_data->CmdListsCount; n++) {
    const ImDrawList *cmd_list          = draw_data->CmdLists[n];
    ImDrawIdx         idx_buffer_offset = 0;

    mVertexBuffer.setData(
        {cmd_list->VtxBuffer.Data, std::size_t(cmd_list->VtxBuffer.Size)},
		GL::BufferUsage::StreamDraw);
    mIndexBuffer.setData(
        {cmd_list->IdxBuffer.Data, std::size_t(cmd_list->IdxBuffer.Size)},
		GL::BufferUsage::StreamDraw);

    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
      const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];

      auto user_texture = static_cast<GL::Texture2D*>(pcmd->TextureId);
      if (user_texture)
      {
          mShader.setTexture(*user_texture);
      }
      else
      {
          mShader.setTexture(mTexture);
      }

      GL::Renderer::setScissor(
          {{(int)pcmd->ClipRect.x, fb_height - (int)(pcmd->ClipRect.w)},
           {(int)(pcmd->ClipRect.z), fb_height - (int)(pcmd->ClipRect.y)}});

      mMesh.setCount(pcmd->ElemCount);
      mMesh.setIndexBuffer(mIndexBuffer, idx_buffer_offset * sizeof(ImDrawIdx),
                           sizeof(ImDrawIdx) == 2
                               ? GL::MeshIndexType::UnsignedShort
                               : GL::MeshIndexType::UnsignedInt);

      idx_buffer_offset += pcmd->ElemCount;

      mMesh.draw(mShader);
    }
  }

  GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
  GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
  GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
}

bool MagnumImGui::mousePressEvent(
    const Platform::Application::MouseEvent &event) {
  return mousePressReleaseEvent(event, true);
}

bool MagnumImGui::mouseReleaseEvent(
    const Platform::Application::MouseEvent &event) {
  return mousePressReleaseEvent(event, false);
}

bool MagnumImGui::mouseScrollEvent(
    const Platform::Application::MouseScrollEvent &event) {
  mMouseScroll += event.offset().y();
  return ImGui::GetIO().WantCaptureMouse;
}

bool MagnumImGui::mouseMoveEvent(
    const Platform::Application::MouseMoveEvent &event) {
  mMousePos = event.position();
  return ImGui::GetIO().WantCaptureMouse;
}

bool MagnumImGui::keyPressEvent(const Platform::Application::KeyEvent &event) {
  return keyPressReleaseEvent(event, true);
}

bool MagnumImGui::keyReleaseEvent(
    const Platform::Application::KeyEvent &event) {
  return keyPressReleaseEvent(event, false);
}

bool MagnumImGui::textInputEvent(
    const Platform::Application::TextInputEvent &event) {
  ImGuiIO &io = ImGui::GetIO();
  io.AddInputCharactersUTF8(event.text().data());
  return false;
}

ImguiShader::ImguiShader() {

  const char *vertex_shader =
      // "#ifndef NEW_GLSL\n"
      // "#define in attribute\n"
      // "#define out varying\n"
      // "#endif\n"
      "\n"
      "#ifdef EXPLICIT_UNIFORM_LOCATION\n"
      "#extension GL_ARB_explicit_uniform_location: enable\n"
      "#endif\n"
      "#ifdef EXPLICIT_ATTRIB_LOCATION\n"
      "#extension GL_ARB_explicit_attrib_location: enable\n"
      "#endif\n"
      "#ifdef EXPLICIT_UNIFORM_LOCATION\n"
      "layout(location = 0)\n"
      "#endif\n"
      "  uniform mediump mat4 ProjMtx;\n"
      "\n"
      "#ifdef EXPLICIT_ATTRIB_LOCATION\n"
      "layout(location = 0)\n"
      "#endif\n"
      "  in mediump vec2 Position;\n"
      "\n"
      "#ifdef EXPLICIT_ATTRIB_LOCATION\n"
      "layout(location = 1)\n"
      "#endif\n"
      "  in mediump  vec2 UV;\n"
      "\n"
      "#ifdef EXPLICIT_ATTRIB_LOCATION\n"
      "layout(location = 2)\n"
      "#endif\n"
      "  in mediump vec4 Color;\n"
      "\n"
      "out mediump vec2 Frag_UV;\n"
      "out mediump vec4 Frag_Color;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  Frag_UV = UV;\n"
      "  Frag_Color = Color;\n"
      "  gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
      "}\n";

  const char *fragment_shader =
      // "#ifndef NEW_GLSL\n"
      // "#define in varying\n"
      // "#define color gl_FragColor\n"
      // "#define texture texture2D\n"
      // "#endif\n"
      "\n"
      "#ifndef RUNTIME_CONST\n"
      "#define const\n"
      "#endif\n"
      "\n"
      "#ifdef EXPLICIT_TEXTURE_LAYER\n"
      "layout(binding = 0)\n"
      "#endif\n"
      "uniform lowp sampler2D Texture;\n"
      "\n"
      "in mediump vec2 Frag_UV;\n"
      "in mediump vec4 Frag_Color;\n"
      "\n"
      //"#ifdef NEW_GLSL\n"
      "out lowp vec4 color;\n"
      //"#endif\n"
      "\n"
      "void main()\n"
      "{\n"
      "  color = Frag_Color * texture( Texture, Frag_UV.st);\n"
      "}\n";

#ifndef MAGNUM_TARGET_GLES
  const GL::Version version = GL::Context::current().supportedVersion(
      { GL::Version::GL330, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
#else
  const Version version =
      Context::current().supportedVersion({Version::GLES300, Version::GLES200});
#endif

  GL::Shader vert{version, GL::Shader::Type::Vertex};
  GL::Shader frag{version, GL::Shader::Type::Fragment};

#ifndef MAGNUM_TARGET_GLES
  if (version != GL::Version::GL210) {
    vert.addSource({"#define NEW_GLSL\n"});
    frag.addSource({"#define NEW_GLSL\n"});
  }

  // Enable explicit attribute locations in shader if they are supported, otherwise bind locations directly
  if (GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version)) {
    vert.addSource({"#define EXPLICIT_ATTRIB_LOCATION\n"});
  }
  else {
    bindAttributeLocation(Position::Location, "Position");
    bindAttributeLocation(TextureCoordinates::Location, "UV");
    bindAttributeLocation(Color::Location, "Color");
  }

  // Enable explicit uniform locations if they are supported
  if (GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version)) {
    vert.addSource({"#define EXPLICIT_UNIFORM_LOCATION\n"});
  }

#else
  if (version != Version::GLES200) {
    vert.addSource({"#define NEW_GLSL\n"});
    frag.addSource({"#define NEW_GLSL\n"});
  }
#endif

  vert.addSource({vertex_shader});
  frag.addSource({fragment_shader});

  CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

  attachShaders({vert, frag});

  CORRADE_INTERNAL_ASSERT_OUTPUT(link());

  mProjMatrixUniform = uniformLocation("ProjMtx");

  setUniform(uniformLocation("Texture"), TextureLayer);
}

namespace ImGui {

void Image(Magnum::GL::Texture2D& texture, Magnum::Vector2 size)
{
  Image(texture, size, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 }, { 0, 0, 0, 0 });
}
void Image(Magnum::GL::Texture2D& texture, Magnum::Vector2 size, Magnum::Vector2 uv0, Magnum::Vector2 uv1)
{
    Image(texture, size, uv0, uv1, { 1, 1, 1, 1 }, { 0, 0, 0, 0 });
}

void Image(Magnum::GL::Texture2D& texture, Magnum::Vector2 size, Magnum::Vector4 tint, Magnum::Vector4 border)
{
    Image(texture, size, { 0, 0 }, { 1, 1 }, tint, border);
}

void Image(Magnum::GL::Texture2D& texture, Magnum::Vector2 size, Magnum::Vector2 uv0, Magnum::Vector2 uv1, Magnum::Vector4 tint, Magnum::Vector4 border)
{
    Image(
        static_cast<ImTextureID>(&texture),
        { size[0], size[1] },
        { uv0[0], uv0[1] }, { uv1[0], uv1[1] },
        { tint[0], tint[1], tint[2], tint[3] },
        { border[0], border[1], border[2], border[3] }
    );
}

}