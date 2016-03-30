#ifndef Magnum_OvrIntegration_Compositor_h
#define Magnum_OvrIntegration_Compositor_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

/** @file
 * @brief Class @ref Magnum::OvrIntegration::Compositor, @ref Magnum::OvrIntegration::Layer, @ref Magnum::OvrIntegration::LayerDirect, @ref Magnum::OvrIntegration::LayerEyeFov, @ref Magnum::OvrIntegration::LayerEyeFovDepth, @ref Magnum::OvrIntegration::LayerQuad, @ref Magnum::OvrIntegration::TimewarpProjectionDescription, enum @ref Magnum::OvrIntegration::LayerType
 *
 * @author Jonathan Hale (Squareys)
 */

#include <memory>
#include <vector>
#include <Magnum/Magnum.h>
#include <OVR_CAPI.h>

#include "Magnum/OvrIntegration/visibility.h"
#include "Magnum/OvrIntegration/OvrIntegration.h"

namespace Magnum { namespace OvrIntegration {

/**
@brief Layer type

@see @ref Layer, @ref Compositor::addLayer()
*/
enum class LayerType: Int {
    /**
     * Described by `ovrLayerEyeFov`.
     * @see @ref LayerEyeFov, @ref Compositor::addLayerEyeFov()
     */
    EyeFov = ovrLayerType_EyeFov,

    /**
     * Described by `ovrLayerQuad`.
     * @see @ref LayerQuad, @ref Compositor::addLayerQuad()
     */
    Quad = ovrLayerType_Quad,

    /**
     * Described by `ovrLayerEyeMatrix`.
     * @see @ref LayerEyeMatrix, @ref Compositor::addLayerEyeMatrix()
     */
    EyeMatrix = ovrLayerType_EyeMatrix,
};

/**
@brief Wrapper around `ovrLayerHeader`

If you need to be able to change layer specific data, use one of the layer
classes instead: @ref LayerDirect, @ref LayerEyeFov, @ref LayerEyeFov or
@ref LayerQuad.

@author Jonathan Hale (Squareys)
*/
class MAGNUM_OVRINTEGRATION_EXPORT Layer {
    public:
        /**
         * @brief Contructor
         * @param type      Type of this layer
         */
        explicit Layer(LayerType type);

        /** @brief Copying is not allowed. */
        Layer(const Layer&) = delete;

        /** @brief Copying is not allowed */
        Layer& operator=(const Layer&) = delete;

        /**
         * @brief Set whether to process this layer in high quality
         * @return Reference to self (for method chaining)
         *
         * High quality mode costs performance, but looks better.
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
         * @brief Whether this layer is processed in high quality
         */
        bool isHighQuality() const {
            return (_layer.Header.Flags & ovrLayerFlag_HighQuality);
        }

        /**
         * @brief Enable/disable the layer
         * @return Reference to self (for method chaining)
         */
        Layer& setEnabled(bool enabled);

        /** @brief Type of this layer */
        LayerType layerType() const {
            return _type;
        }

        /** @brief The underlying `ovrLayerHeader` */
        const ovrLayerHeader& layerHeader() const {
            return _layer.Header;
        }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        ovrLayer_Union _layer;

    private:
        const LayerType _type;

        friend class Compositor;
};

/**
@brief Superclass for layers which can be locked relative to the HMD

@author Jonathan Hale (Squareys)
*/
class MAGNUM_OVRINTEGRATION_EXPORT HeadLockableLayer: public Layer {
    public:
        /** @brief Constructor */
        explicit HeadLockableLayer(LayerType type): Layer(type) {}

        /**
         * @brief Set whether to lock this layer to the players head
         * @return Reference to self (for method chaining)
         *
         * The surface will move relative to the HMD rather than to the
         * sensor/torso, remaining still while the head moves.
         */
        HeadLockableLayer& setHeadLocked(bool headLocked) {
            if(headLocked) {
                _layer.Header.Flags |= ovrLayerFlag_HeadLocked;
            } else {
                _layer.Header.Flags &= ~ovrLayerFlag_HeadLocked;
            }
            return *this;
        }

        /**
         * @brief Whether this layer is locked to the players head
         */
        bool isHeadLocked() const {
            return (_layer.Header.Flags & ovrLayerFlag_HeadLocked);
        }
};

/**
@brief Wrapper around `ovrLayerDirect`

@author Jonathan Hale (Squareys)
*/
class MAGNUM_OVRINTEGRATION_EXPORT LayerDirect: public Layer {
    public:
        /** @brief Constructor */
        explicit LayerDirect();

        /**
         * @brief Set color texture
         * @param eye           Index of the eye the color texture is set for
         * @param textureSet    Swap texture set to set as color texture
         * @return Reference to self (for method chaining)
         */
        LayerDirect& setColorTexture(Int eye, const TextureSwapChain& textureSet);

        /**
         * @brief Set the viewport
         * @param eye           Eye index to set the viewport for
         * @param viewport      Viewport to set to
         * @return Reference to self (for method chaining)
         */
        LayerDirect& setViewport(Int eye, const Range2Di& viewport);
};

/**
@brief Wrapper around `ovrLayerEveFov`

@author Jonathan Hale (Squareys)
*/
class MAGNUM_OVRINTEGRATION_EXPORT LayerEyeFov: public HeadLockableLayer {
    public:
        /** @brief Constructor */
        explicit LayerEyeFov();

        /**
         * @brief Set color texture
         * @param eye           Index of the eye the color texture is set for
         * @param textureSet    Swap texture set to set as color texture
         * @return Reference to self (for method chaining)
         */
        LayerEyeFov& setColorTexture(Int eye, const TextureSwapChain& textureSet);

        /**
         * @brief Set the viewport
         * @param eye           Eye index to set the viewport for
         * @param viewport      Viewport to set to
         * @return Reference to self (for method chaining)
         */
        LayerEyeFov& setViewport(Int eye, const Range2Di& viewport);

        /**
         * @brief Set the render pose
         * @param hmd           HMD to get the render pose from
         * @return Reference to self (for method chaining)
         */
        LayerEyeFov& setRenderPoses(const Hmd& hmd);

        /**
         * @brief Set fov for this layer
         * @param hmd           HMD to get the default eye fov to set to
         * @return Reference to self (for method chaining)
         */
        LayerEyeFov& setFov(const Hmd& hmd);
};

/**
@brief Timewarp projection description

@see @ref LayerEyeFovDepth
@author Jonathan Hale (Squareys)
*/
class MAGNUM_OVRINTEGRATION_EXPORT TimewarpProjectionDescription {
    public:
        /** @brief Constructor */
        explicit TimewarpProjectionDescription(const Matrix4& projectionMatrix);

        /** @brief Corresponding `ovrTimewarpProjectionDesc` */
        const ::ovrTimewarpProjectionDesc& ovrTimewarpProjectionDesc() const {
            return _projectionDesc;
        }

    private:
        ::ovrTimewarpProjectionDesc _projectionDesc;
};

/**
@brief Wrapper around `ovrLayerEveFovDepth`

@author Jonathan Hale (Squareys)
*/
class MAGNUM_OVRINTEGRATION_EXPORT LayerEyeFovDepth: public HeadLockableLayer {
    public:
        explicit LayerEyeFovDepth();

        /**
         * @brief Set color texture
         * @param eye           Index of the eye the color texture is set for
         * @param textureSet    Swap texture set to set as color texture
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setColorTexture(Int eye, const SwapTextureSet& textureSet);

        /**
         * @brief Set the viewport
         * @param eye           Eye index to set the viewport for
         * @param viewport      Viewport to set to
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setViewport(Int eye, const Range2Di& viewport);

        /**
         * @brief Set the render pose
         * @param hmd           HMD to get the render pose from
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setRenderPoses(const Hmd& hmd);

        /**
         * @brief Set FoV for this layer
         * @param hmd           HMD to get the default eye fov to set to
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setFov(const Hmd& hmd);

        /**
         * @brief Set depth texture
         * @param eye           Index of the eye the depth texture is set for
         * @param textureSet    Swap texture set to set as depth texture
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setDepthTexture(Int eye, const SwapTextureSet& textureSet);

        /**
         * @brief Set timewarp projection description
         * @return Reference to self (for method chaining)
         */
        LayerEyeFovDepth& setTimewarpProjDesc(const TimewarpProjectionDescription& desc);
};

/**
@brief Wrapper around `ovrLayerQuad`

@author Jonathan Hale (Squareys)
*/
class MAGNUM_OVRINTEGRATION_EXPORT LayerQuad: public HeadLockableLayer {
    public:
        /** @brief Constructor */
        explicit LayerQuad();

        /**
         * @brief Set color texture
         * @param textureSet    Swap texture set to set as color texture
         * @return Reference to self (for method chaining)
         */
        LayerQuad& setColorTexture(const TextureSwapChain& textureSet);

        /**
         * @brief Set the viewport
         * @param viewport      Viewport to set to
         * @return Reference to self (for method chaining)
         */
        LayerQuad& setViewport(const Range2Di& viewport);

        /**
         * @brief Set position and orientation of the center of the quad
         * @param pose          Center pose of the quad
         * @return Reference to self (for method chaining)
         *
         * Position is specified in meters.
         */
        LayerQuad& setCenterPose(const DualQuaternion& pose);

        /**
         * @brief Set width and heigh of the quad in meters
         * @param size          Size of the quad
         * @return Reference to self (for method chaining)
         */
        LayerQuad& setQuadSize(const Vector2& size);
};

/**
@brief Compositor

Wraps compositing related functions of LibOVR.

## Usage

The compositor handles distortion, chromatic abberation, timewarp and sending
images to a HMD's display.

The compositor may contain a set of layers with different sizes and different
properties. See @ref LayerDirect, @ref LayerEyeFov, @ref LayerEyeFovDepth and
@ref LayerQuad.

Setup of a distortion layer may look as follows:

@code
// setup TextureSwapChains etc
Context context;
Hmd& hmd = // ...
std::unique_ptr<TextureSwapChain> textureSet[2] = // ...
Vector2i textureSize[2] = // ...

// setup compositor layers
LayerEyeFov& layer = Context::get().compositor().addLayerEyeFov();
layer.setFov(hmd.get());
layer.setHighQuality(true);

for(Int eye = 0; eye < 2; ++eye) {
    layer.setColorTexture(eye, *textureSet[eye]);
    layer.setViewport(eye, {{}, textureSize[eye]});
}
@endcode

After that you need to render every frame by first rendering to the texture
sets and then submitting the compositor frame via @ref Compositor::submitFrame().

@code
layer.setRenderPoses(hmd);

Context::get().compositor().submitFrame(hmd);
@endcode

@see @ref Hmd, @ref TextureSwapChain, @ref Context::compositor()
@author Jonathan Hale (Squareys)
*/
class MAGNUM_OVRINTEGRATION_EXPORT Compositor {
    public:
        /** @brief Copying is not allowed */
        Compositor(const Compositor&) = delete;

        /** @brief Moving is not allowed */
        Compositor(Compositor&&) = delete;

        /** @brief Copying is not allowed */
        Compositor& operator=(const Compositor&) = delete;

        /** @brief Moving is not allowed */
        Compositor& operator=(Compositor&&) = delete;

        /**
         * @brief Add a layer of specific type
         *
         * @see @ref addLayerDirect(), @ref addLayerEyeFov(),
         *      @ref addLayerEyeFovDepth(), @ref addLayerQuad()
         */
        Layer& addLayer(LayerType type);

        /**
         * @brief Create a @ref LayerDirect
         *
         * @see @ref addLayer()
         */
        LayerDirect& addLayerDirect();

        /**
         * @brief Create a @ref LayerEyeFov
         *
         * @see @ref addLayer()
         */
        LayerEyeFov& addLayerEyeFov();

        /**
         * @brief Create a @ref LayerEyeFovDepth
         *
         * @see @ref addLayer()
         */
        LayerEyeFovDepth& addLayerEyeFovDepth();

        /**
         * @brief Create a @ref LayerQuad
         *
         * @see @ref addLayer()
         */
        LayerQuad& addLayerQuad();

        /**
         * @brief Submit the frame to the compositor
         * @param hmd       HMD to render to
         * @return Reference to self (for method chaining)
         */
        Compositor& submitFrame(Hmd& hmd);

    private:
        explicit Compositor();

        Layer& addLayer(std::unique_ptr<Layer> layer);

        std::vector<const ovrLayerHeader*> _layers;
        std::vector<std::unique_ptr<Layer>> _wrappedLayers;

        friend class Context;
};

}}

#endif
