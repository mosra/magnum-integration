/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015
              Jonathan Hale <squareys@googlemail.com>

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

#include "Magnum/LibOVRIntegration/Compositor.h"

#include "Magnum/LibOVRIntegration/Conversion.h"
#include "Magnum/LibOVRIntegration/Hmd.h"

#include <OVR_CAPI_GL.h>
#include <OVR_CAPI_Util.h>

namespace Magnum { namespace LibOvrIntegration {

Layer::Layer(const LayerType type): _layer(), _type(type) {
    _layer.Header.Type = ovrLayerType(Corrade::Containers::EnumSet<LayerType>::UnderlyingType(_type));
    _layer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;
}

Layer& Layer::setEnabled(bool enabled) {
    if(enabled) {
        _layer.Header.Type = ovrLayerType(Corrade::Containers::EnumSet<LayerType>::UnderlyingType(_type));
    } else {
        _layer.Header.Type = ovrLayerType_Disabled;
    }
    return *this;
}

//----------------------------------------------------------------
LayerDirect::LayerDirect(): Layer(LayerType::Direct) {
}

LayerDirect& LayerDirect::setColorTexture(const int eye, const SwapTextureSet& textureSet) {
    _layer.Direct.ColorTexture[eye] = &textureSet.getOvrSwapTextureSet();

    return *this;
}

LayerDirect& LayerDirect::setViewport(const int eye, const Range2Di& viewport) {
    _layer.Direct.Viewport[eye] = ovrRecti(viewport);

    return *this;
}

//----------------------------------------------------------------

LayerEyeFov::LayerEyeFov(): Layer(LayerType::EyeFov) {
}

LayerEyeFov& LayerEyeFov::setColorTexture(const int eye, const SwapTextureSet& textureSet) {
    _layer.EyeFov.ColorTexture[eye] = &textureSet.getOvrSwapTextureSet();

    return *this;
}

LayerEyeFov& LayerEyeFov::setViewport(const int eye, const Range2Di& viewport) {
    _layer.EyeFov.Viewport[eye] = ovrRecti(viewport);

    return *this;
}

LayerEyeFov& LayerEyeFov::setRenderPoses(const Hmd& hmd) {
    const ovrPosef* poses = hmd.getOvrEyePoses();
    _layer.EyeFov.RenderPose[0] = poses[0];
    _layer.EyeFov.RenderPose[1] = poses[1];

    return *this;
}

LayerEyeFov& LayerEyeFov::setFov(const Hmd& hmd) {
    const ovrFovPort* fov = hmd.getOvrHmd()->DefaultEyeFov;
    _layer.EyeFov.Fov[0] = fov[0];
    _layer.EyeFov.Fov[1] = fov[1];

    return *this;
}

//----------------------------------------------------------------

TimewarpProjectionDescription::TimewarpProjectionDescription(const Matrix4& projectionMatrix) {
    _projectionDesc = ovrTimewarpProjectionDesc_FromProjection(ovrMatrix4f(projectionMatrix));
}

LayerEyeFovDepth::LayerEyeFovDepth(): Layer(LayerType::EyeFovDepth) {
}

LayerEyeFovDepth& LayerEyeFovDepth::setColorTexture(const int eye, const SwapTextureSet& textureSet) {
    _layer.EyeFovDepth.ColorTexture[eye] = &textureSet.getOvrSwapTextureSet();

    return *this;
}

LayerEyeFovDepth& LayerEyeFovDepth::setViewport(const int eye, const Range2Di& viewport) {
    _layer.EyeFovDepth.Viewport[eye] = ovrRecti(viewport);

    return *this;
}

LayerEyeFovDepth& LayerEyeFovDepth::setRenderPoses(const Hmd& hmd) {
    const ovrPosef* poses = hmd.getOvrEyePoses();
    _layer.EyeFov.RenderPose[0] = poses[0];
    _layer.EyeFov.RenderPose[1] = poses[1];

    return *this;
}

LayerEyeFovDepth& LayerEyeFovDepth::setFov(const Hmd& hmd) {
    const ovrFovPort* fov = hmd.getOvrHmd()->DefaultEyeFov;
    _layer.EyeFov.Fov[0] = fov[0];
    _layer.EyeFov.Fov[1] = fov[1];

    return *this;
}

LayerEyeFovDepth& LayerEyeFovDepth::setDepthTexture(const int eye, const SwapTextureSet& textureSet) {
    _layer.EyeFovDepth.DepthTexture[eye] = &textureSet.getOvrSwapTextureSet();

    return *this;
}

LayerEyeFovDepth& LayerEyeFovDepth::setTimewarpProjDesc(const TimewarpProjectionDescription& desc) {
    _layer.EyeFovDepth.ProjectionDesc = desc.getOvrTimewarpProjectionDesc();

    return *this;
}

//----------------------------------------------------------------

LayerQuad::LayerQuad(bool headLocked):
    Layer((headLocked) ? LayerType::QuadHeadLocked : LayerType::QuadInWorld)
{
}

LayerQuad& LayerQuad::setColorTexture(const SwapTextureSet& textureSet) {
    _layer.Quad.ColorTexture = &textureSet.getOvrSwapTextureSet();

    return *this;
}

LayerQuad& LayerQuad::setViewport(const Range2Di& viewport) {
    _layer.Quad.Viewport = ovrRecti(viewport);

    return *this;
}

LayerQuad& LayerQuad::setCenterPose(DualQuaternion pose) {
    _layer.Quad.QuadPoseCenter = ovrPosef(pose);

    return *this;
}

LayerQuad& LayerQuad::setQuadSize(Vector2 size) {
    _layer.Quad.QuadSize = ovrVector2f(size);

    return *this;
}


//----------------------------------------------------------------

Compositor::Compositor(): _layers(), _wrappedLayers() {
}

typedef Corrade::Containers::EnumSet<LayerType> LayerTypes;

Layer& Compositor::addLayer(const LayerType type) {
    switch (type) {
        case LayerType::Direct:
            return addLayerDirect();
        case LayerType::EyeFov:
            return addLayerEyeFov();
        case LayerType::EyeFovDepth:
            return addLayerEyeFovDepth();
        case LayerType::QuadHeadLocked:
            return addLayerQuadHeadLocked();
        case LayerType::QuadInWorld:
            return addLayerQuadInWorld();
    }
    CORRADE_ASSERT_UNREACHABLE();
}

Layer& Compositor::addLayer(std::unique_ptr<Layer> layer) {
    _wrappedLayers.push_back(std::move(layer));
    _layers.emplace_back(&_wrappedLayers.back().get()->layerHeader());

    return *_wrappedLayers.back().get();
}

LayerDirect& Compositor::addLayerDirect() {
    return static_cast<LayerDirect&>(addLayer(std::move(std::unique_ptr<Layer>(new LayerDirect()))));
}

LayerEyeFov& Compositor::addLayerEyeFov() {
    return static_cast<LayerEyeFov&>(addLayer(std::move(std::unique_ptr<Layer>(new LayerEyeFov()))));
}

LayerEyeFovDepth& Compositor::addLayerEyeFovDepth() {
    return static_cast<LayerEyeFovDepth&>(addLayer(std::move(std::unique_ptr<Layer>(new LayerEyeFov()))));
}

LayerQuad& Compositor::addLayerQuadHeadLocked() {
    return static_cast<LayerQuad&>(addLayer(std::move(std::unique_ptr<Layer>(new LayerQuad(true)))));
}

LayerQuad& Compositor::addLayerQuadInWorld() {
    return static_cast<LayerQuad&>(addLayer(std::move(std::unique_ptr<Layer>(new LayerEyeFov()))));
}

Compositor& Compositor::submitFrame(Hmd& hmd) {
    ovrHmd_SubmitFrame(hmd.getOvrHmd(), hmd.incFrameIndex(), &hmd.getOvrViewScale(), _layers.data(), _layers.size());

    return *this;
}

}}
