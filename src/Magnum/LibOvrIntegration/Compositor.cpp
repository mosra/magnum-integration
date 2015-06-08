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

LayerEyeFov::LayerEyeFov(): Layer(LayerType::EyeFov) {
}

LayerEyeFov& LayerEyeFov::setColorTexture(const int eye, SwapTextureSet& textureSet) {
    _layer.EyeFov.ColorTexture[eye] = &textureSet.getOvrSwapTextureSet();

    return *this;
}

LayerEyeFov& LayerEyeFov::setViewport(const int eye, const Range2Di& viewport) {
    _layer.EyeFov.Viewport[eye] = ovrRecti(viewport);

    return *this;
}

LayerEyeFov& LayerEyeFov::setRenderPoses(const Hmd& hmd) {
    _layer.EyeFov.RenderPose[0] = hmd._ovrPoses[0];
    _layer.EyeFov.RenderPose[1] = hmd._ovrPoses[1];

    return *this;
}

LayerEyeFov& LayerEyeFov::setFov(const Hmd& hmd) {
    _layer.EyeFov.Fov[0] = hmd._hmd->DefaultEyeFov[0];
    _layer.EyeFov.Fov[1] = hmd._hmd->DefaultEyeFov[1];

    return *this;
}

//----------------------------------------------------------------

Compositor::Compositor(): _layers(), _wrappedLayers() {
}

typedef Corrade::Containers::EnumSet<LayerType> LayerTypes;

Layer& Compositor::addLayer(const LayerType type) {
    switch (type) {
        case LayerType::Direct:
            CORRADE_ASSERT(false, "LayerType::Direct is not implemented.", addLayerEyeFov());
            break;
        case LayerType::EyeFov:
            return addLayerEyeFov();
        case LayerType::EyeFovDepth:
            CORRADE_ASSERT(false, "LayerType::EyeFovDepth is not implemented.", addLayerEyeFov());
            break;
        case LayerType::QuadHeadLocked:
            CORRADE_ASSERT(false, "LayerType::QuadHeadLocked is not implemented.", addLayerEyeFov());
            break;
        case LayerType::QuadInWorld:
            CORRADE_ASSERT(false, "LayerType::QuadInWorld is not implemented.", addLayerEyeFov());
            break;
    }
    CORRADE_ASSERT_UNREACHABLE();
}

LayerEyeFov& Compositor::addLayerEyeFov() {
    _wrappedLayers.push_back(std::move(std::unique_ptr<Layer>(new LayerEyeFov())));
    _layers.emplace_back(&_wrappedLayers.back().get()->layerHeader());

    return *static_cast<LayerEyeFov*>(_wrappedLayers.back().get());
}

Compositor& Compositor::submitFrame(const Hmd& hmd) {
    ovrHmd_SubmitFrame(hmd.getOvrHmd(), 0, &hmd.getOvrViewScale(), _layers.data(), _layers.size());

    return *this;
}

}}
