/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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

#include <Magnum/TextureFormat.h>

#include <OVR_CAPI_GL.h>
#undef near
#undef far

namespace Magnum { namespace OvrIntegration {

SwapTextureSet::SwapTextureSet(const Hmd& hmd, TextureFormat format, const Vector2i& size):
    _hmd(hmd),
    _format(format),
    _size(size)
{
    ovr_CreateSwapTextureSetGL(_hmd.ovrSession(), GLenum(_format), _size.x(), _size.y(), &_swapTextureSet);

    /* wrap the texture set for magnum */
    _textures = Containers::Array<Texture2D>{Containers::NoInit, UnsignedInt(_swapTextureSet->TextureCount)};

    for(UnsignedInt i = 0; i < _textures.size(); ++i) {
        new(&_textures[i]) Texture2D(wrap(_swapTextureSet->Textures[i]));
        _textures[i].setMinificationFilter(Sampler::Filter::Linear)
                    .setMagnificationFilter(Sampler::Filter::Linear)
                    .setWrapping(Sampler::Wrapping::ClampToEdge);
    }
}

SwapTextureSet::~SwapTextureSet() {
    ovr_DestroySwapTextureSet(_hmd.ovrSession(), _swapTextureSet);
}

Texture2D& SwapTextureSet::activeTexture() {
    return _textures[_swapTextureSet->CurrentIndex];
}

//----------------------------------------------------------------

Hmd::Hmd(::ovrSession hmd):
    _session(hmd),
    _hmdDesc(ovr_GetHmdDesc(_session)),
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

Hmd& Hmd::configureTracking(HmdTrackingCapabilities caps, HmdTrackingCapabilities required) {
    ovr_ConfigureTracking(_session, Int(caps), Int(required));
    return *this;
}

Hmd& Hmd::configureRendering() {
    /* get offset from center to left/right eye. The offset lengths may differ. */
    _hmdToEyeViewOffset[0] = ovr_GetRenderDesc(_session, ovrEye_Left, _hmdDesc.DefaultEyeFov[0]).HmdToEyeViewOffset;
    _hmdToEyeViewOffset[1] = ovr_GetRenderDesc(_session, ovrEye_Right, _hmdDesc.DefaultEyeFov[1]).HmdToEyeViewOffset;

    _viewScale.HmdSpaceToWorldScaleInMeters = 1.0f;
    _viewScale.HmdToEyeViewOffset[0] = _hmdToEyeViewOffset[0];
    _viewScale.HmdToEyeViewOffset[1] = _hmdToEyeViewOffset[1];

    return *this;
}

Vector2i Hmd::fovTextureSize(const Int eye) {
    return Vector2i(ovr_GetFovTextureSize(_session, ovrEyeType(eye), _hmdDesc.DefaultEyeFov[eye], 1.0));
}

Texture2D& Hmd::createMirrorTexture(const TextureFormat format, const Vector2i& size) {
    CORRADE_ASSERT(!(_flags & HmdStatusFlag::HasMirrorTexture),
           "Hmd::createMirrorTexture may only be called once, returning result of previous call.",
            *_mirrorTexture);

    ovrResult result = ovr_CreateMirrorTextureGL(
                _session,
                GLenum(format),
                size.x(),
                size.y(),
                &_ovrMirrorTexture);

    if(result != ovrSuccess) {
        ovrErrorInfo info;
        ovr_GetLastErrorInfo(&info);
        Error() << info.ErrorString;
    }

    _mirrorTexture.reset(new Texture2D(wrap(*_ovrMirrorTexture)));

    return *_mirrorTexture;
}

std::unique_ptr<SwapTextureSet> Hmd::createSwapTextureSet(TextureFormat format, const Int eye) {
    return std::unique_ptr<SwapTextureSet>(new SwapTextureSet(*this, format, fovTextureSize(eye)));
}

std::unique_ptr<SwapTextureSet> Hmd::createSwapTextureSet(TextureFormat format, const Vector2i& size) {
    return std::unique_ptr<SwapTextureSet>(new SwapTextureSet(*this, format, size));
}

Matrix4 Hmd::projectionMatrix(const Int eye, Float near, Float far) const {
    ovrMatrix4f proj = ovrMatrix4f_Projection(_hmdDesc.DefaultEyeFov[eye], near, far,
                                              ovrProjection_RightHanded | ovrProjection_ClipRangeOpenGL);
    return Matrix4(proj);
}

Matrix4 Hmd::orthoSubProjectionMatrix(const Int eye, const Matrix4& proj, const Vector2& scale, Float distance) const {
    ovrMatrix4f sub = ovrMatrix4f_OrthoSubProjection(ovrMatrix4f(proj), ovrVector2f(scale), distance,
                                                     _hmdToEyeViewOffset[eye].x);
    return Matrix4(sub);
}

Hmd& Hmd::pollEyePoses() {
    _predictedDisplayTime = ovr_GetPredictedDisplayTime(_session, _frameIndex);
    _trackingState = ovr_GetTrackingState(_session, _predictedDisplayTime, true);
    ovr_CalcEyePoses(_trackingState.HeadPose.ThePose, _hmdToEyeViewOffset, _ovrPoses);

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

}}
