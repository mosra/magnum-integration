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


#include "Magnum/LibOVRIntegration/Hmd.h"

#include "Magnum/LibOVRIntegration/HmdEnum.h"
#include "Magnum/LibOVRIntegration/Conversion.h"

#include <Magnum/TextureFormat.h>

#include <OVR_CAPI_GL.h>
#include <OVR_CAPI_Util.h>
#undef near
#undef far

namespace Magnum { namespace LibOvrIntegration {

SwapTextureSet::SwapTextureSet(const Hmd& hmd, TextureFormat format, const Vector2i& size) : _hmd(hmd), _format(format), _size(size) {
    ovrHmd_CreateSwapTextureSetGL(_hmd._hmd, GLenum(_format), _size.x(), _size.y(), &_swapTextureSet);

    /* wrap the texture set for magnum */
    _textures = new Texture2D*[_swapTextureSet->TextureCount];

    for(Int i = 0; i < _swapTextureSet->TextureCount; ++i) {
        _textures[i] = new Texture2D(wrap(_swapTextureSet->Textures[i]));
        _textures[i]->setMinificationFilter(Sampler::Filter::Linear)
                    .setMagnificationFilter(Sampler::Filter::Linear)
                    .setWrapping(Sampler::Wrapping::ClampToEdge);
    }
}

SwapTextureSet::~SwapTextureSet() {
    Int numTextures = _swapTextureSet->TextureCount;

    ovrHmd_DestroySwapTextureSet(_hmd._hmd, _swapTextureSet);

    for(Int i = 0; i < numTextures; ++i) {
        delete _textures[i];
    }
    delete _textures;
}

Texture2D& SwapTextureSet::activeTexture() const {
    return *_textures[_swapTextureSet->CurrentIndex];
}

//----------------------------------------------------------------

Hmd::Hmd(::ovrHmd hmd, HmdStatusFlags flags) : _hmd(hmd), _flags(flags) {
}

Hmd::~Hmd() {
    if(_flags & HmdStatusFlag::HasMirrorTexture) {
        ovrHmd_DestroyMirrorTexture(_hmd, _ovrMirrorTexture);
        _mirrorTexture.reset();
    }

    ovrHmd_Destroy(_hmd);
}

Hmd& Hmd::setEnabledCaps(HmdCapabilities caps){
    ovrHmd_SetEnabledCaps(_hmd, HmdCapabilities::UnderlyingType(caps));
    return *this;
}

Hmd& Hmd::configureTracking(HmdTrackingCapabilities caps, HmdTrackingCapabilities required) {
    ovrHmd_ConfigureTracking(_hmd,
           HmdTrackingCapabilities::UnderlyingType(caps),
           HmdTrackingCapabilities::UnderlyingType(required));
    return *this;
}

Hmd& Hmd::configureRendering() {
    /* get offset from center to left/right eye. The offset lengths may differ. */
    _hmdToEyeViewOffset[0] = ovrHmd_GetRenderDesc(_hmd, ovrEye_Left, _hmd->DefaultEyeFov[0]).HmdToEyeViewOffset;
    _hmdToEyeViewOffset[1] = ovrHmd_GetRenderDesc(_hmd, ovrEye_Right, _hmd->DefaultEyeFov[1]).HmdToEyeViewOffset;

    _viewScale.HmdSpaceToWorldScaleInMeters = 1.0f;
    _viewScale.HmdToEyeViewOffset[0] = _hmdToEyeViewOffset[0];
    _viewScale.HmdToEyeViewOffset[1] = _hmdToEyeViewOffset[1];

    return *this;
}

Vector2i Hmd::fovTextureSize(const Int eye) {
    return Vector2i(ovrHmd_GetFovTextureSize(_hmd, ovrEyeType(eye), _hmd->DefaultEyeFov[eye], 1.0));
}

Texture2D& Hmd::createMirrorTexture(const TextureFormat format, const Vector2i& size) {
    CORRADE_ASSERT(!(_flags & HmdStatusFlag::HasMirrorTexture),
           "Hmd::createMirrorTexture may only be called once, returning result of previous call.",
            *_mirrorTexture);

    ovrResult result = ovrHmd_CreateMirrorTextureGL(
                _hmd,
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
    ovrMatrix4f proj = ovrMatrix4f_Projection(_hmd->DefaultEyeFov[eye], near, far,
                                              ovrProjection_RightHanded | ovrProjection_ClipRangeOpenGL);
    return Matrix4(proj);
}

Matrix4 Hmd::orthoSubProjectionMatrix(const Int eye, const Matrix4& proj, const Vector2& scale, Float distance) const {
    ovrMatrix4f sub = ovrMatrix4f_OrthoSubProjection(ovrMatrix4f(proj), ovrVector2f(scale), distance,
                                                      _hmdToEyeViewOffset[eye].x);
    return Matrix4(sub);
}

Hmd& Hmd::pollEyePoses() {
    _frameTiming = ovrHmd_GetFrameTiming(_hmd, _frameIndex);
    _trackingState = ovrHmd_GetTrackingState(_hmd, _frameTiming.DisplayMidpointSeconds);
    ovr_CalcEyePoses(_trackingState.HeadPose.ThePose, _hmdToEyeViewOffset, _ovrPoses);

    return *this;
}

bool Hmd::isDebugHmd() const {
    return (_flags & HmdStatusFlag::Debug) != HmdStatusFlags{};
}

std::array<DualQuaternion, 2> Hmd::eyePoses() {
    return std::array<DualQuaternion, 2>{{DualQuaternion(_ovrPoses[0]), DualQuaternion(_ovrPoses[1])}};
}


}}
