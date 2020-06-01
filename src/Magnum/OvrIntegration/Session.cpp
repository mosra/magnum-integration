/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015, 2016, 2017 Jonathan Hale <squareys@googlemail.com>

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

#include "Session.h"

#include <OVR_CAPI_GL.h>
#undef near
#undef far

#include "Magnum/OvrIntegration/Enums.h"
#include "Magnum/OvrIntegration/Context.h"

namespace Magnum { namespace OvrIntegration {

namespace Implementation {

/**
@brief HMD status flag

@see @ref HmdStatusFlags
*/
enum class HmdStatusFlag: UnsignedByte {
    /**
     * A mirror texture was created for the HMD and needs to be destroyed on
     * destruction of the HMD
     */
    HasMirrorTexture = 1 << 0,

    /** The HMD was created as a debug HMD (without real hardware) */
    Debug = ovrHmdCap_DebugDevice /* 0x0010 */
};

/** @brief HMD status flags */
typedef Containers::EnumSet<HmdStatusFlag> HmdStatusFlags;

CORRADE_ENUMSET_OPERATORS(HmdStatusFlags)

}

Buttons InputState::buttons() const {
    return Buttons{static_cast<Button>(_state.Buttons)};
}

Touches InputState::touches() const {
     return Touches{static_cast<Touch>(_state.Touches)};
}

TextureSwapChain::TextureSwapChain(const Session& session, const Vector2i& size):
    _session(session),
    _size(size),
    _curIndex(0)
{
    ovrTextureSwapChainDesc desc;
    desc.Type = ovrTexture_2D;
    desc.Width = size.x();
    desc.Height = size.y();
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.StaticImage = ovrFalse;
    desc.MiscFlags = ovrTextureMisc_None;
    desc.BindFlags = ovrTextureBind_None;

    ovrResult result = ovr_CreateTextureSwapChainGL(_session.ovrSession(), &desc, &_textureSwapChain);

    if(result != ovrSuccess) {
        Corrade::Utility::Error() << "Could not create texture swap chain:" << Context::get().error().message;
        return;
    }

    Int length = 0;
    ovr_GetTextureSwapChainLength(_session.ovrSession(), _textureSwapChain, &length);

    /* wrap the texture swap chain for magnum */
    _textures = Containers::Array<GL::Texture2D>{Containers::NoInit, UnsignedInt(length)};

    for(UnsignedInt i = 0; i < _textures.size(); ++i) {
        UnsignedInt textureId;
        ovr_GetTextureSwapChainBufferGL(_session.ovrSession(), _textureSwapChain, i, &textureId);
        new(&_textures[i]) GL::Texture2D(GL::Texture2D::wrap(textureId));
        _textures[i].setMinificationFilter(GL::SamplerFilter::Linear)
                    .setMagnificationFilter(GL::SamplerFilter::Linear)
                    .setWrapping(GL::SamplerWrapping::ClampToEdge);
    }
}

TextureSwapChain::~TextureSwapChain() {
    ovr_DestroyTextureSwapChain(_session.ovrSession(), _textureSwapChain);
}

TextureSwapChain& TextureSwapChain::commit() {
    ovr_CommitTextureSwapChain(_session.ovrSession(), _textureSwapChain);
    ovr_GetTextureSwapChainCurrentIndex(_session.ovrSession(), _textureSwapChain, &_curIndex);

    return *this;
}

GL::Texture2D& TextureSwapChain::activeTexture() {
    return _textures[_curIndex];
}

Session::Session(::ovrSession session):
    _session(session),
    _hmdDesc(ovr_GetHmdDesc(_session)),
    _ovrMirrorTexture(nullptr),
    /* _hmdDesc.AvailableHmdCaps is either 0 or ovrHmdCap_DebugDevice
       and therefore can be simply cast to HmdStatusFlag */
    _flags(Implementation::HmdStatusFlag(_hmdDesc.AvailableHmdCaps)) {}

Session::~Session() {
    if(_flags & Implementation::HmdStatusFlag::HasMirrorTexture)
        ovr_DestroyMirrorTexture(_session, _ovrMirrorTexture);

    ovr_Destroy(_session);
}

Session& Session::configureRendering() {
    /* get offset from center to left/right eye. The offset lengths may differ. */
    _hmdToEyePose[0] = ovr_GetRenderDesc(_session, ovrEye_Left, _hmdDesc.DefaultEyeFov[0]).HmdToEyePose;
    _hmdToEyePose[1] = ovr_GetRenderDesc(_session, ovrEye_Right, _hmdDesc.DefaultEyeFov[1]).HmdToEyePose;

    _viewScale.HmdSpaceToWorldScaleInMeters = 1.0f;
    _viewScale.HmdToEyePose[0] = _hmdToEyePose[0];
    _viewScale.HmdToEyePose[1] = _hmdToEyePose[1];

    return *this;
}

Vector2i Session::fovTextureSize(const Int eye) {
    return Vector2i(ovr_GetFovTextureSize(_session, ovrEyeType(eye), _hmdDesc.DefaultEyeFov[eye], 1.0));
}

GL::Texture2D& Session::createMirrorTexture(const Vector2i& size, const MirrorOptions mirrorOptions) {
    CORRADE_ASSERT(!(_flags & Implementation::HmdStatusFlag::HasMirrorTexture),
           "Session::createMirrorTexture may only be called once, returning result of previous call.",
            *_mirrorTexture);

    ovrMirrorTextureDesc desc;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.Width = size.x();
    desc.Height = size.y();
    desc.MiscFlags = ovrTextureMisc_None;
    desc.MirrorOptions = UnsignedInt(mirrorOptions);

    ovrResult result = ovr_CreateMirrorTextureWithOptionsGL(
                _session,
                &desc,
                &_ovrMirrorTexture);

    if(result != ovrSuccess) {
        Corrade::Utility::Error() << "Could not create mirror texture:" << Context::get().error().message;
    }

    /* wrap the opengl texture id as magnum texture */
    UnsignedInt id;
    ovr_GetMirrorTextureBufferGL(_session, _ovrMirrorTexture, &id);
    _mirrorTexture.reset(new GL::Texture2D(GL::Texture2D::wrap(id)));

    return *_mirrorTexture;
}

std::unique_ptr<TextureSwapChain> Session::createTextureSwapChain(const Int eye) {
    return std::unique_ptr<TextureSwapChain>(new TextureSwapChain(*this, fovTextureSize(eye)));
}

std::unique_ptr<TextureSwapChain> Session::createTextureSwapChain(const Vector2i& size) {
    return std::unique_ptr<TextureSwapChain>(new TextureSwapChain(*this, size));
}

Matrix4 Session::projectionMatrix(const Int eye, Float near, Float far) const {
    const ovrMatrix4f proj = ovrMatrix4f_Projection(_hmdDesc.DefaultEyeFov[eye],
        near, far, ovrProjection_ClipRangeOpenGL);
    return Matrix4(proj);
}

Matrix4 Session::orthoSubProjectionMatrix(const Int eye, const Matrix4& proj, const Vector2& scale, Float distance) const {
    const ovrMatrix4f sub = ovrMatrix4f_OrthoSubProjection(ovrMatrix4f(proj),
        ovrVector2f(scale), distance, _hmdToEyePose[eye].Position.x);
    return Matrix4(sub);
}

Session& Session::pollTrackers() {
    _predictedDisplayTime = ovr_GetPredictedDisplayTime(_session, _frameIndex);
    _trackingState = ovr_GetTrackingState(_session, _predictedDisplayTime, true);
    return *this;
}

Session& Session::pollEyePoses() {
    pollTrackers();
    ovr_CalcEyePoses(_trackingState.HeadPose.ThePose, _hmdToEyePose, _ovrPoses);
    return *this;
}

Session& Session::pollController(const ControllerType types, InputState& state) {
    ovr_GetInputState(_session, ovrControllerType(types), &state.ovrInputState());
    return *this;
}

SessionStatusFlags Session::status() const {
    ovrSessionStatus status;
    ovr_GetSessionStatus(_session, &status);
    const SessionStatusFlags none = SessionStatusFlags{};

    return ((status.IsVisible) ? SessionStatusFlag::IsVisible : none) |
           ((status.HmdPresent) ? SessionStatusFlag::HmdPresent : none) |
           ((status.HmdMounted) ? SessionStatusFlag::HmdMounted : none) |
           ((status.DisplayLost) ? SessionStatusFlag::DisplayLost : none) |
           ((status.ShouldQuit) ? SessionStatusFlag::ShouldQuit : none) |
           ((status.ShouldRecenter) ? SessionStatusFlag::ShouldRecenter : none);
}

bool Session::isDebugHmd() const {
    return (_flags & Implementation::HmdStatusFlag::Debug) != Implementation::HmdStatusFlags{};
}

void Session::setPerformanceHudMode(const PerformanceHudMode mode) const {
    ovr_SetInt(_session, OVR_PERF_HUD_MODE, Int(mode));
}

void Session::setDebugHudStereoMode(const DebugHudStereoMode mode) const {
    ovr_SetInt(_session, OVR_DEBUG_HUD_STEREO_MODE, Int(mode));
}

void Session::setLayerHudMode(const LayerHudMode mode) const {
    ovr_SetInt(_session, OVR_LAYER_HUD_MODE, Int(mode));
}

}}
