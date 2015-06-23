#ifndef Magnum_LibOvrIntegration_Compositor_h
#define Magnum_LibOvrIntegration_Compositor_h
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

/** @file
 * @brief Class @ref Magnum::LibOvrIntegration::Compositor, @ref Magnum::LibOvrIntegration::Layer, @ref Magnum::LibOvrIntegration::LayerDirect, @ref Magnum::LibOvrIntegration::LayerEyeFov, @ref Magnum::LibOvrIntegration::LayerEyeFovDepth, @ref Magnum::LibOvrIntegration::LayerQuad, @ref Magnum::LibOvrIntegration::TimewarpProjectionDescription, enum @ref Magnum::LibOvrIntegration::LayerType
 *
 * @author Jonathan Hale (Squareys)
 */

#include <memory>
#include <vector>
#include <Magnum/Magnum.h>
#include <OVR_CAPI.h>

#include "Magnum/LibOvrIntegration/visibility.h"
#include "Magnum/LibOvrIntegration/LibOvrIntegration.h"

namespace Magnum { namespace LibOvrIntegration {

/**
@brief Layer type

@see @ref Layer, @ref Compositor::addLayer()
*/
enum class LayerType: UnsignedByte {
    /**
     * Described by `ovrLayerEyeFov`.
     * @see @ref LayerEyeFov, @ref Compositor::addLayerEyeFov()
     */
    EyeFov = ovrLayerType_EyeFov,

    /**
     * Described by `ovrLayerEyeFovDepth`.
     * @see @ref LayerEyeFovDepth, @ref Compositor::addLayerEyeFovDepth()
     */
    EyeFovDepth = ovrLayerType_EyeFovDepth,

    /**
     * Described by `ovrLayerQuad`.
     * @see @ref LayerQuad, @ref Compositor::addLayerQuadInWorld()
     */
    QuadInWorld = ovrLayerType_QuadInWorld,

    /**
     * Described by `ovrLayerQuad`. Displayed in front of your face, moving
     * with the head.
     * @see @ref LayerQuad, @ref Compositor::addLayerQuadHeadLocked()
     */
    QuadHeadLocked = ovrLayerType_QuadHeadLocked,

    /**
     * Described by `ovrLayerDirect`. Passthrough for debugging and custom
     * rendering.
     * @see @ref LayerDirect, @ref Compositor::addLayerDirect()
     */
    Direct = ovrLayerType_Direct

};

/**
@brief Wrapper around `ovrLayerHeader`

If you need to be able to change layer specific data, use one of the layer
classes instead: @ref LayerDirect, @ref LayerEyeFov, @ref LayerEyeFov or
@ref LayerQuad.

@author Jonathan Hale (Squareys)
*/
class MAGNUM_LIBOVRINTEGRATION_EXPORT Layer {
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
@brief Wrapper around `ovrLayerDirect`

@author Jonathan Hale (Squareys)
*/
class MAGNUM_LIBOVRINTEGRATION_EXPORT LayerDirect: public Layer {
    public:
        /** @brief Constructor */
        explicit LayerDirect();

        /**
         * @brief Set color texture
         * @param eye           Index of the eye the color texture is set for
         * @param textureSet    Swap texture set to set as color texture
         * @return Reference to self (for method chaining)
         */
        LayerDirect& setColorTexture(Int eye, const SwapTextureSet& textureSet);

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
class MAGNUM_LIBOVRINTEGRATION_EXPORT LayerEyeFov: public Layer {
    public:
        /** @brief Constructor */
        explicit LayerEyeFov();

        /**
         * @brief Set color texture
         * @param eye           Index of the eye the color texture is set for
         * @param textureSet    Swap texture set to set as color texture
         * @return Reference to self (for method chaining)
         */
        LayerEyeFov& setColorTexture(Int eye, const SwapTextureSet& textureSet);

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
class MAGNUM_LIBOVRINTEGRATION_EXPORT TimewarpProjectionDescription {
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
class MAGNUM_LIBOVRINTEGRATION_EXPORT LayerEyeFovDepth: public Layer {
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
class MAGNUM_LIBOVRINTEGRATION_EXPORT LayerQuad: public Layer {
    public:
        /** @brief Constructor */
        explicit LayerQuad(bool headLocked = false);

        /**
         * @brief Set color texture
         * @param textureSet    Swap texture set to set as color texture
         * @return Reference to self (for method chaining)
         */
        LayerQuad& setColorTexture(const SwapTextureSet& textureSet);

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
// setup SwapTextureSets etc
Context context;
Hmd& hmd = // ...
std::unique_ptr<SwapTextureSet> textureSet[2] = // ...
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

@see @ref Hmd, @ref SwapTextureSet, @ref Context::compositor()
@author Jonathan Hale (Squareys)
*/
class MAGNUM_LIBOVRINTEGRATION_EXPORT Compositor {
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
         *      @ref addLayerEyeFovDepth(), @ref addLayerQuadHeadLocked(),
         *      @ref addLayerQuadInWorld()
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
         * @brief Create a @ref LayerQuad with @ref LayerType::QuadHeadLocked
         *
         * @see @ref addLayer()
         */
        LayerQuad& addLayerQuadHeadLocked();

        /**
         * @brief Create a @ref LayerQuad with @ref LayerType::QuadInWorld
         *
         * @see @ref addLayer()
         */
        LayerQuad& addLayerQuadInWorld();

        /**
         * @brief Submit the frame to the compositor
         * @param hmd       HMD to render to.
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
