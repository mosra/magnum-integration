#ifndef Magnum_LibOvrIntegration_Hmd_h
#define Magnum_LibOvrIntegration_Hmd_h
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
 * @brief Class @ref Magnum::LibOvrIntegration::Hmd and class @ref Magnum::LibOvrIntegration::SwapTextureSet.
 */

#include <memory>
#include <OVR_CAPI.h>
#include <Magnum/Texture.h>
#include <Magnum/Magnum.h>

#include "Magnum/LibOVRIntegration/visibility.h"
#include "Magnum/LibOVRIntegration/Conversion.h"
#include "Magnum/LibOVRIntegration/LibOvrIntegration.h"

namespace Magnum { namespace LibOvrIntegration {

/**
@brief SwapTextureSet

Contains an array of texture which can be rendered to an hmd display by the
libOVR compositor after applying distortion to it.

@todoc

@see @ref Hmd
*/
class MAGNUM_LIBOVRINTEGRATION_EXPORT SwapTextureSet {
    public:
        /**
         * @brief Constructor.
         * @param hmd Hmd for which this SwapTextureSet is created.
         * @param format Texture format.
         * @param size Size for the textures.
         */
        explicit SwapTextureSet(const Hmd& hmd, TextureFormat format, Vector2i size);
        ~SwapTextureSet();

        /** @brief Currently active texture in the set. */
        Texture2D& getActiveTexture() const;

        /**
         * @brief Increment to use the next texture in the set.
         * @return Reference to self (for method chaining)
         */
        SwapTextureSet& increment() {
            _swapTextureSet->CurrentIndex = (_swapTextureSet->CurrentIndex + 1) % _swapTextureSet->TextureCount;
            return *this;
        }

        /** @brief The underlying ovrSwapTextureSet. */
        ovrSwapTextureSet& getOvrSwapTextureSet() {
            return *_swapTextureSet;
        }

    private:
        const Hmd& _hmd;
        TextureFormat _format;
        Vector2i _size;

        ovrSwapTextureSet* _swapTextureSet;
        Texture2D** _textures;
};

/**
@brief Hmd

Wraps ovrHmd_* methods.

## Usage

Instances of Hmd are created by @ref LibOvrContext.

@code
std::unique_ptr<Hmd> hmd = LibOvrContext::get().initialize().createHmd(0, HmdType::DK2);
hmd->setEnabledCaps({HmdCapability::LowPersistence, HmdCapability::DynamicPrediction});
hmd->configureTracking({HmdTrackingCapability::Orientation,
                        HmdTrackingCapability::MagYawCorrection,
                        HmdTrackingCapability::Position}, {});
// ...
LibOvrContext::get().shutdown();
@endcode

@todoc hmdpose, mirror texture, swaptextureset
*/
class MAGNUM_LIBOVRINTEGRATION_EXPORT Hmd {
    public:

        /** @brief Destructor. */
        ~Hmd();

        /**
         * @brief Enable or disable hmd capabilities
         * @param caps @ref HmdCapability flags to enable.
         * @return Reference to self (for method chaining)
         */
        Hmd& setEnabledCaps(HmdCapabilities caps);

        /**
         * @brief Enable or disable hmd tracking capabilities
         * @param caps @ref HmdTrackingCapability flags to enable.
         * @return Reference to self (for method chaining)
         */
        Hmd& configureTracking(HmdTrackingCapabilities caps, HmdTrackingCapabilities required);

        /**
         * @brief Configure rendering to the rift.
         *
         * Includes setting up hmd to eye offsets internally.
         *
         * @return Reference to self (for method chaining)
         */
        Hmd& configureRendering();

        /**
         * @brief Get preferred size for textures used for rendering to this hmd.
         * @param eye Eye index to get the texture size for.
         * @return Size for textures.
         */
        Vector2i getFovTextureSize(const unsigned int eye);

        /**
         * @brief Create a mirror texture.
         *
         * The libOVR compositor will render a copy of its result to the texture returned
         * by this method.
         *
         * @param format Texture format.
         * @param size Size for the mirror texture.
         * @return Pointer to the created mirror texture. Its destruction is handled by the Hmd.
         */
        Texture2D& createMirrorTexture(TextureFormat format, Vector2i size);

        /**
         * @brief Convenience method to create a @ref SwapTextureSet for this Hmd.
         * @param format Texture format.
         * @param eye Eye index which will be used to get the preferred size for the texture.
         * @return The created @ref SwapTextureSet.
         *
         * @see createSwapTextureSet(format,eye)
         */
        std::unique_ptr<SwapTextureSet> createSwapTextureSet(TextureFormat format, int eye);

        /**
         * @brief Create a @ref SwapTextureSet for this Hmd.
         * @param format Texture format.
         * @param size Size for the textures in the created @ref SwapTextureSet.
         * @return The created @ref SwapTextureSet.
         *
         * @see createSwapTextureSet(format,eye)
         */
        std::unique_ptr<SwapTextureSet> createSwapTextureSet(TextureFormat format, const Vector2i size);

        /**
         * @brief getEyePoses Get the current translation for the eyes from the
         *                    head pose tracked by the hmd.
         * @return A @ref std::unique_ptr to an array of Matrix4 with size 2,
         *         the transformations for each eye.
         */
        std::unique_ptr<DualQuaternion> getEyePoses();

        /**
         * @brief Refresh cached eye poses.
         * @see Use @ref getEyePoses() to access the result.
         * @return Reference to self (for method chaining)
         */
        Hmd& pollEyePoses();

        /** @brief Resolution of the Hmd's display. */
        Vector2i resolution() const {
            return Vector2i(_hmd->Resolution);
        }

        /**
         * @brief Tan of the fov for an eye.
         * @param eye Eye index.
         * @return Vector of eye fovs, x being horizontal and y vertical.
         */
        Vector2 defaultEyeFov(int eye) const {
            const ovrFovPort fov = _hmd->DefaultEyeFov[eye];
            return {fov.RightTan + fov.LeftTan, fov.UpTan + fov.DownTan};
        }

        /** @brief Get the underlying ovrHmd. */
        ovrHmd getOvrHmd() const {
            return _hmd;
        }

        /** @brief Get the ovrViewScale. */
        const ovrViewScaleDesc& getOvrViewScale() const {
            return _viewScale;
        }

        /** @brief Whether this hmd is a debug hmd (true) or connection to a real device (false). */
        bool isDebugHmd() const;

        /**
         * @brief Get a pointer to the most current eye poses as ovrPosef.
         * @return A pointer to an ovrPosef[2] containing the most
         *         recently polled eye poses.
         * @see Hmd::pollEyePoses()
         */
        const ovrPosef* getOvrEyePoses() const {
            return _ovrPoses;
        }

    private:
        explicit Hmd(ovrHmd hmd, HmdStatusFlags flags);

        ovrHmd _hmd;
        ovrPosef _ovrPoses[2];
        ovrVector3f _hmdToEyeViewOffset[2];
        ovrViewScaleDesc _viewScale;

        ovrTrackingState _trackingState;

        ovrTexture* _ovrMirrorTexture;
        std::unique_ptr<Texture2D> _mirrorTexture;

        HmdStatusFlags _flags;

        friend class SwapTextureSet;
        friend class LibOvrContext;
        friend class LayerEyeFov;
};

}}

#endif
