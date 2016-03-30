/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015, 2016 Jonathan Hale <squareys@googlemail.com>

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

#include "Magnum/OvrIntegration/Hmd.h"

#include "Magnum/OvrIntegration/HmdEnum.h"
#include "Magnum/OvrIntegration/Conversion.h"
#include "Magnum/OvrIntegration/Context.h"

#include <Magnum/TextureFormat.h>

#include <OVR_CAPI_GL.h>
#undef near
#undef far

namespace Magnum { namespace OvrIntegration {

TextureSwapChain::TextureSwapChain(const Hmd& hmd, const Vector2i& size):
    _hmd(hmd),
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

    ovrResult result = ovr_CreateTextureSwapChainGL(_hmd.ovrSession(), &desc, &_textureSwapChain);

    if(result != ovrSuccess) {
        Corrade::Utility::Error() << "Could not create texture swap chain:" << Context::get().error().message;
        return;
    }

    Int length = 0;
    ovr_GetTextureSwapChainLength(_hmd.ovrSession(), _textureSwapChain, &length);

    /* wrap the texture set for magnum */
    _textures = Containers::Array<Texture2D>{Containers::NoInit, UnsignedInt(length)};

    for(UnsignedInt i = 0; i < _textures.size(); ++i) {
        UnsignedInt textureId;
        ovr_GetTextureSwapChainBufferGL(_hmd.ovrSession(), _textureSwapChain, i, &textureId);
        new(&_textures[i]) Texture2D(Texture2D::wrap(textureId));
        _textures[i].setMinificationFilter(Sampler::Filter::Linear)
                    .setMagnificationFilter(Sampler::Filter::Linear)
                    .setWrapping(Sampler::Wrapping::ClampToEdge);
    }
}

TextureSwapChain::~TextureSwapChain() {
    ovr_DestroyTextureSwapChain(_hmd.ovrSession(), _textureSwapChain);
}

TextureSwapChain& TextureSwapChain::commit() {
    ovr_CommitTextureSwapChain(_hmd.ovrSession(), _textureSwapChain);
    ovr_GetTextureSwapChainCurrentIndex(_hmd.ovrSession(), _textureSwapChain, &_curIndex);

    return *this;
}

Texture2D& TextureSwapChain::activeTexture() {
    return _textures[_curIndex];
}

//----------------------------------------------------------------

Hmd::Hmd(::ovrSession hmd):
    _session(hmd),
    _hmdDesc(ovr_GetHmdDesc(_session)),
    _ovrMirrorTexture(nullptr),
    _flags(HmdStatusFlag(_hmdDesc.AvailableHmdCaps))
{
    /* _hmdDesc.AvailableHmdCaps is either 0 or ovrHmdCap_DebugDevice,
     * and therefore can be simply cast to HmdStatusFlag */
}

Hmd::~Hmd() {
    if(_flags & HmdStatusFlag::HasMirrorTexture) {
        ovr_DestroyMirrorTexture(_session, _ovrMirrorTexture);
    }

    ovr_Destroy(_session);
}

Hmd& Hmd::configureRendering() {
    /* get offset from center to left/right eye. The offset lengths may differ. */
    _hmdToEyeOffset[0] = ovr_GetRenderDesc(_session, ovrEye_Left, _hmdDesc.DefaultEyeFov[0]).HmdToEyeOffset;
    _hmdToEyeOffset[1] = ovr_GetRenderDesc(_session, ovrEye_Right, _hmdDesc.DefaultEyeFov[1]).HmdToEyeOffset;

    _viewScale.HmdSpaceToWorldScaleInMeters = 1.0f;
    _viewScale.HmdToEyeOffset[0] = _hmdToEyeOffset[0];
    _viewScale.HmdToEyeOffset[1] = _hmdToEyeOffset[1];

    return *this;
}

Vector2i Hmd::fovTextureSize(const Int eye) {
    return Vector2i(ovr_GetFovTextureSize(_session, ovrEyeType(eye), _hmdDesc.DefaultEyeFov[eye], 1.0));
}

Texture2D& Hmd::createMirrorTexture(const Vector2i& size) {
    CORRADE_ASSERT(!(_flags & HmdStatusFlag::HasMirrorTexture),
           "Hmd::createMirrorTexture may only be called once, returning result of previous call.",
            *_mirrorTexture);

    ovrMirrorTextureDesc desc;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.Width = size.x();
    desc.Height = size.y();
    desc.MiscFlags = ovrTextureMisc_None;

    ovrResult result = ovr_CreateMirrorTextureGL(
                _session,
                &desc,
                &_ovrMirrorTexture);

    if(result != ovrSuccess) {
        Corrade::Utility::Error() << "Could not create mirror texture:" << Context::get().error().message;
    }

    /* wrap the opengl texture id as magnum texture */
    UnsignedInt id;
    ovr_GetMirrorTextureBufferGL(_session, _ovrMirrorTexture, &id);
    _mirrorTexture.reset(new Texture2D(std::move(Texture2D::wrap(id))));

    return *_mirrorTexture;
}

std::unique_ptr<TextureSwapChain> Hmd::createTextureSwapChain(const Int eye) {
    return std::unique_ptr<TextureSwapChain>(new TextureSwapChain(*this, fovTextureSize(eye)));
}

std::unique_ptr<TextureSwapChain> Hmd::createTextureSwapChain(const Vector2i& size) {
    return std::unique_ptr<TextureSwapChain>(new TextureSwapChain(*this, size));
}

Matrix4 Hmd::projectionMatrix(const Int eye, Float near, Float far) const {
    ovrMatrix4f proj = ovrMatrix4f_Projection(_hmdDesc.DefaultEyeFov[eye], near, far,
                                              ovrProjection_ClipRangeOpenGL);
    return Matrix4(proj);
}

Matrix4 Hmd::orthoSubProjectionMatrix(const Int eye, const Matrix4& proj, const Vector2& scale, Float distance) const {
    ovrMatrix4f sub = ovrMatrix4f_OrthoSubProjection(ovrMatrix4f(proj), ovrVector2f(scale), distance,
                                                     _hmdToEyeOffset[eye].x);
    return Matrix4(sub);
}

Hmd& Hmd::pollEyePoses() {
    _predictedDisplayTime = ovr_GetPredictedDisplayTime(_session, _frameIndex);
    _trackingState = ovr_GetTrackingState(_session, _predictedDisplayTime, true);
    ovr_CalcEyePoses(_trackingState.HeadPose.ThePose, _hmdToEyeOffset, _ovrPoses);

    return *this;
}

SessionStatusFlags Hmd::sessionStatus() const {
    ovrSessionStatus status;
    ovr_GetSessionStatus(_session, &status);

    return ((status.HasVrFocus) ? SessionStatusFlag::HasVrFocus : SessionStatusFlags{})
         | ((status.HmdPresent) ? SessionStatusFlag::HmdPresent : SessionStatusFlags{});
}

bool Hmd::isDebugHmd() const {
    return (_flags & HmdStatusFlag::Debug) != HmdStatusFlags{};
}

void Hmd::setPerformanceHudMode(const PerformanceHudMode mode) const {
    ovr_SetInt(_session, OVR_PERF_HUD_MODE, Int(mode));
}

void Hmd::setDebugHudStereoMode(const DebugHudStereoMode mode) const {
    ovr_SetInt(_session, OVR_DEBUG_HUD_STEREO_MODE, Int(mode));
}

void Hmd::setLayerHudMode(const LayerHudMode mode) const {
    ovr_SetInt(_session, OVR_LAYER_HUD_MODE, Int(mode));
}

}}
