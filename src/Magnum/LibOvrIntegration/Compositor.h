#ifndef Magnum_LibOvrIntegration_Compositor_h
#define Magnum_LibOvrIntegration_Compositor_h
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

/** @file
 * @brief Class @ref Magnum::LibOvrIntegration::Compositor,
 *        class @ref Magnum::LibOvrIntegration::Layer,
 *        class @ref Magnum::LibOvrIntegration::EyeFovLayer,
 *        enum @ref Magnum::LibOvrIntegration::LayerType
 *
 * @author Jonathan Hale (Squareys)
 */

#include <Magnum/Magnum.h>

#include <memory>
#include <vector>

#include "Magnum/LibOvrIntegration/visibility.h"
#include "Magnum/LibOvrIntegration/LibOvrIntegration.h"

#include <OVR_CAPI.h>

namespace Magnum { namespace LibOvrIntegration {

enum class LayerType: UnsignedByte {

    /** @brief Described by ovrLayerEyeFov. */
    EyeFov = ovrLayerType_EyeFov,

    /** @brief Described by ovrLayerEyeFovDepth. */
    EyeFovDepth = ovrLayerType_EyeFovDepth,

    /** @brief Described by ovrLayerQuad. */
    QuadInWorld = ovrLayerType_QuadInWorld,

    /** @brief Described by ovrLayerQuad. Displayed in front of your face, moving with the head. */
    QuadHeadLocked = ovrLayerType_QuadHeadLocked,

    /** @brief Described by ovrLayerDirect. Passthrough for debugging and custom rendering. */
    Direct = ovrLayerType_Direct

};

/**
 * @brief Wrapper around an ovrLayerHeader.
 *
 * If you need to be able to change the layer, use one of the type specific
 * layer classes instead: @ref LayerEyeFov
 *
 * @author Jonathan Hale (Squareys)
 */
class MAGNUM_LIBOVRINTEGRATION_EXPORT Layer {
    public:
        /**
         * @brief Contructor.
         * @param type Type of this layer.
         */
        explicit Layer(const LayerType type);

        /** @brief Copying is not allowed. */
        Layer(const Layer& context) = delete;
        Layer& operator=(const Layer&) = delete;

        /**
         * @brief Set whether to process this layer in high quality.
         *
         * High quality mode costs performance, but looks better.
         *
         * @param b If true, layer will be in high quality mode, if false, the
         *          layer will be in lower quality mode.
         * @return Reference to self (for method chaining)
         */
        Layer& setHighQuality(bool highQuality) {
            if(highQuality) {
                _layer.Header.Flags |= ovrLayerFlag_HighQuality;
            } else {
                _layer.Header.Flags &= ~ovrLayerFlag_HighQuality;
            }
            return *this;
        }

        /**
         * @brief Enable/disable a the layer.
         * @param enabled If true, the layer will be enabled.
         * @return Reference to self (for method chaining)
         */
        Layer& setEnabled(bool enabled);

        /** @brief Type of this layer. */
        LayerType layerType() const {
            return _type;
        }

        /** @brief The underlying ovrLayerHeader. */
        const ovrLayerHeader& layerHeader() const {
            return _layer.Header;
        }

    protected:
        ovrLayer_Union _layer;

    private:
        const LayerType _type;

        friend class Compositor;
};

/**
 * @brief Wrapper around ovrLayerDirect.
 * @author Jonathan Hale (Squareys)
 */
class MAGNUM_LIBOVRINTEGRATION_EXPORT LayerDirect: public Layer {
    public:
        /** @brief Constructor. */
        explicit LayerDirect();

        /** @brief Copying is not allowed. */
        LayerDirect(const LayerDirect&) = delete;

        /**
         * @brief Set color texture.
         * @param eye Index of the eye the color texture is set for.
         * @param textureSet @ref SwapTextureSet to set as color texture.
         * @return Reference to self (for method chaining)
         */
        LayerDirect& setColorTexture(const int eye, const SwapTextureSet& textureSet);

        /**
         * @brief Set the viewport.
         * @param eye Eye index to set the viewport for.
         * @param viewport Viewport to set to.
         * @return Reference to self (for method chaining)
         */
        LayerDirect& setViewport(const int eye, const Range2Di& viewport);

};

/**
 * @brief Wrapper around ovrLayerEveFov.
 * @author Jonathan Hale (Squareys)
 */
class MAGNUM_LIBOVRINTEGRATION_EXPORT LayerEyeFov: public Layer {
    public:
        /** @brief Constructor. */
        explicit LayerEyeFov();

        /** @brief Copying is not allowed. */
        LayerEyeFov(const LayerEyeFov&) = delete;

        /**
         * @brief Set color texture.
         * @param eye Index of the eye the color texture is set for.
         * @param textureSet @ref SwapTextureSet to set as color texture.
         * @return Reference to self (for method chaining)
         */
        LayerEyeFov& setColorTexture(const int eye, const SwapTextureSet& textureSet);

        /**
         * @brief Set the viewport.
         * @param eye Eye index to set the viewport for.
         * @param viewport Viewport to set to.
         * @return Reference to self (for method chaining)
         */
        LayerEyeFov& setViewport(const int eye, const Range2Di& viewport);

        /**
         * @brief Set the render pose.
         * @param hmd Hmd to get the render pose from.
         * @return Reference to self (for method chaining)
         */
        LayerEyeFov& setRenderPoses(const Hmd& hmd);

        /**
         * @brief Set fov for this layer.
         * @param hmd Hmd to get the default eye fov to set to.
         * @return Reference to self (for method chaining)
         */
        LayerEyeFov& setFov(const Hmd& hmd);

};

class MAGNUM_LIBOVRINTEGRATION_EXPORT TimewarpProjectionDescription {
    public:
        explicit TimewarpProjectionDescription(const Matrix4& projectionMatrix);

        const ovrTimewarpProjectionDesc& getOvrTimewarpProjectionDesc() const {
            return _projectionDesc;
        }

    private:
        ovrTimewarpProjectionDesc _projectionDesc;
};

/**
 * @brief Wrapper around ovrLayerEveFovDepth.
 * @author Jonathan Hale (Squareys)
 */
class MAGNUM_LIBOVRINTEGRATION_EXPORT LayerEyeFovDepth: public Layer {
    public:
        /** @brief Constructor. */
        explicit LayerEyeFovDepth();

        /** @brief Copying is not allowed. */
        LayerEyeFovDepth(const LayerEyeFovDepth&) = delete;

        /**
         * @brief Set color texture.
         * @param eye Index of the eye the color texture is set for.
         * @param textureSet @ref SwapTextureSet to set as color texture.
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setColorTexture(const int eye, const SwapTextureSet& textureSet);

        /**
         * @brief Set the viewport.
         * @param eye Eye index to set the viewport for.
         * @param viewport Viewport to set to.
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setViewport(const int eye, const Range2Di& viewport);

        /**
         * @brief Set the render pose.
         * @param hmd Hmd to get the render pose from.
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setRenderPoses(const Hmd& hmd);

        /**
         * @brief Set fov for this layer.
         * @param hmd Hmd to get the default eye fov to set to.
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setFov(const Hmd& hmd);

        /**
         * @brief Set depth texture.
         * @param eye Index of the eye the depth texture is set for.
         * @param textureSet @ref SwapTextureSet to set as depth texture.
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setDepthTexture(const int eye, const SwapTextureSet& textureSet);

        /**
         * @brief setTimewarpProjectionDesc
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setTimewarpProjDesc(const TimewarpProjectionDescription& desc);
};

/**
 * @brief Wrapper around ovrLayerQuad.
 * @author Jonathan Hale (Squareys)
 * @see @ref ovrLayerQuad
 */
class MAGNUM_LIBOVRINTEGRATION_EXPORT LayerQuad: public Layer {
    public:
        /** @brief Constructor. */
        explicit LayerQuad(bool headLocked = false);

        /** @brief Copying is not allowed. */
        LayerQuad(const LayerQuad&) = delete;

        /**
         * @brief Set color texture.
         * @param textureSet @ref SwapTextureSet to set as color texture.
         * @return Reference to self (for method chaining)
         */
        LayerQuad& setColorTexture(const SwapTextureSet& textureSet);

        /**
         * @brief Set the viewport.
         * @param viewport Viewport to set to.
         * @return Reference to self (for method chaining)
         */
        LayerQuad& setViewport(const Range2Di& viewport);

        /**
         * @brief Set position and orientation of the center of the quad.
         *
         * Position is specified in meters.
         *
         * @param pose Center pose of the quad.
         * @return Reference to self (for method chaining)
         */
        LayerQuad& setCenterPose(DualQuaternion pose);

        /**
         * @brief Set width and heigh of the quad in meters.
         * @param size Size of the quad.
         * @return Reference to self (for method chaining)
         */
        LayerQuad& setQuadSize(Vector2 size);

};

/**
@brief Compositor

Wraps compositing related functions of LibOVR.

@todoc Usage...

@author Jonathan Hale (Squareys)
*/
class MAGNUM_LIBOVRINTEGRATION_EXPORT Compositor {
    public:
        /* @brief Copying is not allowed. */
        Compositor(const Compositor&) = delete;

        /* @brief Moving is not allowed. */
        Compositor(Compositor&&) = delete;

        /**
         * @brief Add a layer of specific type.
         * @param type Layer type.
         * @return Reference to the created layer.
         */
        Layer& addLayer(const LayerType type);

        /**
         * @brief Create a @ref LayerDirect.
         * @return Reference to the created layer.
         */
        LayerDirect& addLayerDirect();

        /**
         * @brief Create a @ref LayerEyeFov.
         * @return Reference to the created layer.
         */
        LayerEyeFov& addLayerEyeFov();

        /**
         * @brief Create a @ref LayerEyeFovDepth.
         * @return Reference to the created layer.
         */
        LayerEyeFovDepth& addLayerEyeFovDepth();

        /**
         * @brief Create a @ref LayerQuad with @ref LayerType::QuadHeadLocked.
         * @return Reference to the created layer.
         */
        LayerQuad& addLayerQuadHeadLocked();

        /**
         * @brief Create a @ref LayerQuad with @ref LayerType::QuadInWorld.
         * @return Reference to the created layer.
         */
        LayerQuad& addLayerQuadInWorld();

        /**
         * @brief Submit the frame to the compositor.
         * @param hmd Hmd to render to.
         * @return Reference to self (for method chaining)
         */
        Compositor& submitFrame(const Hmd& hmd);

    private:

        explicit Compositor();

        Layer& addLayer(std::unique_ptr<Layer> layer);

        std::vector<const ovrLayerHeader*> _layers;
        std::vector<std::unique_ptr<Layer>> _wrappedLayers;

        friend class LibOvrContext;
};

}}

#endif
