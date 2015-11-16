#ifndef Magnum_OvrIntegration_Hmd_h
#define Magnum_OvrIntegration_Hmd_h
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
 * @brief Class @ref Magnum::OvrIntegration::Hmd, @ref Magnum::OvrIntegration::SwapTextureSet
 *
 * @author Jonathan Hale (Squareys)
 */

#include <array>
#include <memory>
#include <OVR_CAPI.h>
#include <OVR_CAPI_Keys.h>

#include <Corrade/Containers/Array.h>

#include <Magnum/Texture.h>
#include <Magnum/Magnum.h>

#include "Magnum/OvrIntegration/Conversion.h"
#include "Magnum/OvrIntegration/OvrIntegration.h"
#include "Magnum/OvrIntegration/HmdEnum.h"
#include "Magnum/OvrIntegration/visibility.h"

namespace Magnum { namespace OvrIntegration {

/**
@brief Swap texture set

Contains an array of textures which can be rendered to an HMD by the Oculus SDK
@ref Compositor.
@see @ref Hmd, @ref Layer
*/
class MAGNUM_OVRINTEGRATION_EXPORT SwapTextureSet {
    public:
        /**
         * @brief Constructor
         * @param hmd       HMD for which this swap texture set is created
         * @param format    Texture format
         * @param size      Size for the textures
         */
        explicit SwapTextureSet(const Hmd& hmd, TextureFormat format, const Vector2i& size);
        ~SwapTextureSet();

        /** @brief Currently active texture in the set */
        Texture2D& activeTexture();

        /**
         * @brief Increment to use the next texture in the set
         * @return Reference to self (for method chaining)
         */
        SwapTextureSet& increment() {
            _swapTextureSet->CurrentIndex = (_swapTextureSet->CurrentIndex + 1) % _swapTextureSet->TextureCount;
            return *this;
        }

        /** @brief The underlying `ovrSwapTextureSet` */
        ::ovrSwapTextureSet& ovrSwapTextureSet() const {
            return *_swapTextureSet;
        }

    private:
        const Hmd& _hmd;
        TextureFormat _format;
        Vector2i _size;

        ::ovrSwapTextureSet* _swapTextureSet;
        Containers::Array<Texture2D> _textures;
};

/**
@brief Hmd

Wraps `ovrSession`, `ovrHmdDesc` and methods from the Oculus SDK which directly
affect an HMD and its properties.

## Usage

Instances of @ref Hmd are created by @ref Context.

@code
std::unique_ptr<Hmd> hmd = Context::get().initialize().createHmd();
hmd->configureTracking(HmdTrackingCapability::Orientation |
                       HmdTrackingCapability::MagYawCorrection |
                       HmdTrackingCapability::Position, {});
hmd->configureRendering();

// ...
@endcode

Once the HMD is configured, you can poll and get the head pose.

@code
std::unique_ptr<DualQuaternion> poses = hmd->pollEyePoses().eyePoses();

DualQuaternion leftPose = poses.get()[0];
DualQuaternion rightPose = poses.get()[1];
@endcode

### Rendering to HMD

Rendering to an @ref Hmd is done via the @ref Compositor. It's results are
rendered directly to the Rift. The compositor layers usually require you to
render to a set of textures which are then rendered to the rift with
distortion, chromatic abberation and possibly timewarp.

A setup for such a @ref SwapTextureSet for an eye could look like this:

@code
const Int eye = 0; // left eye
Vector2i textureSize = hmd.fovTextureSize(eye);
std::unique_ptr<SwapTextureSet> textureSet = hmd.createSwapTextureSet(TextureFormat::RGBA, textureSize);

// create the framebuffer which will be used to render to the current texture
// of the texture set later.
Framebuffer framebuffer{{}, textureSize};
framebuffer.mapForDraw(Framebuffer::ColorAttachment(0));

// setup depth attachment
Image2D image(PixelFormat::DepthComponent, PixelType::UnsignedInt, textureSize, nullptr);

Texture2D* depth = new Texture2D();
depth->setMinificationFilter(Sampler::Filter::Linear)
      .setMagnificationFilter(Sampler::Filter::Linear)
      .setWrapping(Sampler::Wrapping::ClampToEdge)
      .setStorage(1, TextureFormat::DepthComponent24, textureSize)
      .subImage(0, {{}, textureSize}, image);

// ...

// render to the SwapTextureSet
textureSet->increment();

// switch to framebuffer and attach textures
framebuffer.bind();
framebuffer.attachTexture(Framebuffer::ColorAttachment(0), _textureSet->activeTexture(), 0)
           .attachTexture(Framebuffer::BufferAttachment::Depth, *depth, 0)
           .clear(FramebufferClear::Color | FramebufferClear::Depth);

// ... render scene

framebuffer.detach(Framebuffer::ColorAttachment(0))
           .detach(Framebuffer::BufferAttachment::Depth);
@endcode

Usually, especially for debugging, you will want to have a *mirror* of the
@ref Compositor result displayed to a window.

@code
Texture2D& mirrorTexture = hmd->createMirrorTexture(TextureFormat::RGBA, resolution);
Framebuffer mirrorFramebuffer{Range2Di::fromSize({}, resolution)};
mirrorFramebuffer.attachTexture(Framebuffer::ColorAttachment(0), mirrorTexture, 0)
                 .mapForRead(Framebuffer::ColorAttachment(0));

// ...

// blit mirror texture to defaultFramebuffer.
const Vector2i size = mirrorTexture->imageSize(0);
Framebuffer::blit(mirrorFramebuffer,
                  defaultFramebuffer,
                  {{0, size.y()}, {size.x(), 0}},
                  {{}, size},
                  FramebufferBlit::Color, FramebufferBlitFilter::Nearest);
@endcode

@see @ref Context, @ref SwapTextureSet, @ref Compositor
*/
class MAGNUM_OVRINTEGRATION_EXPORT Hmd {
    public:
        ~Hmd();

        /**
         * @brief Enable or disable HMD tracking capabilities
         * @return Reference to self (for method chaining)
         */
        Hmd& configureTracking(HmdTrackingCapabilities caps, HmdTrackingCapabilities required);

        /**
         * @brief Configure rendering to the Rift
         * @return Reference to self (for method chaining)
         *
         * Includes setting up HMD to eye offsets internally.
         */
        Hmd& configureRendering();

        /**
         * @brief Get preferred size for textures used for rendering to this HMD
         * @param eye       Eye index to get the texture size for
         */
        Vector2i fovTextureSize(Int eye);

        /**
         * @brief Create a mirror texture
         * @param format    Texture format
         * @param size      Size for the mirror texture
         * @return Reference to the created mirror texture. Its destruction is
         *      handled by the @ref Hmd.
         *
         * The libOVR compositor will render a copy of its result to the
         * texture returned by this method.
         */
        Texture2D& createMirrorTexture(TextureFormat format, const Vector2i& size);

        /**
         * @brief Convenience method to create a @ref SwapTextureSet for this HMD
         * @param format    Texture format
         * @param eye       Eye index which will be used to get the preferred
         *      size for the texture.
         *
         * @see @ref createSwapTextureSet(TextureFormat, const Vector2i&)
         */
        std::unique_ptr<SwapTextureSet> createSwapTextureSet(TextureFormat format, Int eye);

        /**
         * @brief Create a @ref SwapTextureSet for this HMD
         * @param format    Texture format
         * @param size      Size for the textures in the created set
         *
         * @see @ref createSwapTextureSet(TextureFormat, Int)
         */
        std::unique_ptr<SwapTextureSet> createSwapTextureSet(TextureFormat format, const Vector2i& size);

        /**
         * @brief Get the current translation for the eyes from the head pose tracked by the HMD
         *
         * Returns array of two DualQuaternions describing tranformation and orientation of each eye.
         */
        std::array<DualQuaternion, 2> eyePoses() const {
            return std::array<DualQuaternion, 2>{{DualQuaternion(_ovrPoses[0]), DualQuaternion(_ovrPoses[1])}};
        }

        /**
         * @brief Refresh cached eye poses
         * @return Reference to self (for method chaining)
         *
         * Use @ref eyePoses() to access the result.
         */
        Hmd& pollEyePoses();

        /** @brief Resolution of the HMD's display */
        Vector2i resolution() const {
            return Vector2i(_hmdDesc.Resolution);
        }

        /**
         * @brief Tan of the FoV for an eye
         * @param eye   Eye index
         *
         * Returns vector of eye FoVs, x being horizontal and y vertical.
         */
        Vector2 defaultEyeFov(Int eye) const {
            const ovrFovPort& fov = _hmdDesc.DefaultEyeFov[eye];
            return {fov.RightTan + fov.LeftTan, fov.UpTan + fov.DownTan};
        }

        /**
         * @brief Get the projection matrix
         * @param eye       The eye index
         * @param near      Distance to near frustum plane
         * @param far       Distance to far frustum plane
         * @return The projection matrix for eye
         *
         * Get the projection matrix for an eye index for which should be used
         * for prespective rendering to this HMD.
         * @see @ref orthoSubProjectionMatrix()
         */
        Matrix4 projectionMatrix(Int eye, Float near, Float far) const;

        /**
         * @brief Get a projection matrix for projection to an orthogonal plane.
         * @param eye       The eye index
         * @param proj      Projection matrix, usually created by
         *      @ref Hmd::projectionMatrix()
         * @param scale     Scale for the 2D plane
         * @param distance  Distance of the plane from hmd position
         * @return The projection matrix for eye
         *
         * Get a projection matrix which can be used for projection onto a 2D
         * plane orthogonal to the hmds view/screen with distance from hmds
         * position.
         * @see @ref Hmd::projectionMatrix()
         */
        Matrix4 orthoSubProjectionMatrix(Int eye, const Matrix4& proj, const Vector2& scale, Float distance) const;

        /** @brief Get the underlying `ovrHmd` */
        ::ovrHmd ovrHmd() const { return _session; }

        /** @brief Get the underlying `ovrSession` */
        ::ovrSession ovrSession() const { return _session; }

        /** @brief Get the underlying `ovrHmdDesc` */
        ::ovrHmdDesc ovrHmdDesc() const { return _hmdDesc; }

        /** @brief Get the `ovrViewScale` */
        const ::ovrViewScaleDesc& ovrViewScaleDesc() const { return _viewScale; }

        /** @brief Whether this HMD is a connection to a virtual or real device */
        bool isDebugHmd() const;

        /**
         * @brief Get a pointer to the most current eye poses as `ovrPosef`
         *
         * Returns pointer to an `ovrPosef[2]` containing the most recently
         * polled eye poses.
         * @see @ref pollEyePoses()
         */
        const ovrPosef* ovrEyePoses() const { return _ovrPoses; }

        /**
         * @brief Reset yaw and postion to current pose
         *
         * The roll and pitch orientation components are always determined by
         * gravity and cannot be redefined. All future tracking will report
         * values relative to this new reference position.
         */
        void recenterPose() const {
            ovr_RecenterPose(_session);
        }

        /** @brief Get the current frame index */
        Long currentFrameIndex() const { return _frameIndex; }

        /**
         * @brief Increment the frame index
         *
         * Returns the new index value. This method is called by
         * @ref Compositor::submitFrame().
         */
        Long incFrameIndex() {
            return ++_frameIndex;
        }

        /**
         * @brief Set performance HUD mode
         *
         * Performance HUD enables the HMD user to see information critical to
         * the real-time operation of the VR application such as latency timing
         * and CPU & GPU performance metrics.
         */
        void setPerformanceHudMode(PerformanceHudMode mode) const;

        /**
         * @brief Set debug HUD Setero Mode
         *
         * Debug HUD is provided to help developers gauge and debug the fidelity of their app's
         * stereo rendering characteristics. Using the provided quad and crosshair guides,
         * the developer can verify various aspects such as VR tracking units (e.g. meters),
         * stereo camera-parallax properties (e.g. making sure objects at infinity are rendered
         * with the proper separation), measuring VR geometry sizes and distances and more.
         */
        void setDebugHudStereoMode(DebugHudStereoMode mode) const;

        /**
         * @brief Set layer HUD mode
         *
         * Layer HUD enables the HMD user to see information about a layer.
         */
        void setLayerHudMode(LayerHudMode mode) const;

        /** @brief Tracking state */
        StatusFlags trackingState() const {
            return {StatusFlag(_trackingState.StatusFlags)};
        }

        /** @brief Name of the active Oculus profile */
        std::string user() const {
            return ovr_GetString(_session, OVR_KEY_USER, "");
        }

        /** @brief Name set in the active Oculus profile */
        std::string playerName() const {
            return ovr_GetString(_session, OVR_KEY_NAME, "");
        }

        /** @brief Gender set in the active Oculus profile */
        std::string playerGender() const {
            return ovr_GetString(_session, OVR_KEY_GENDER, OVR_DEFAULT_GENDER);
        }

        /** @brief Player height set in the active Oculus profile */
        Float playerHeight() const {
            return ovr_GetFloat(_session, OVR_KEY_EYE_HEIGHT, OVR_DEFAULT_PLAYER_HEIGHT);
        }

        /** @brief Eye height set in the active Oculus profile */
        Float eyeHeight() const {
            return ovr_GetFloat(_session, OVR_KEY_EYE_HEIGHT, OVR_DEFAULT_EYE_HEIGHT);
        }

        /** @brief Interpupillar distance set in the active Oculus profile */
        Float ipd() const {
            return ovr_GetFloat(_session, OVR_KEY_IPD, OVR_DEFAULT_IPD);
        }

        /** @brief Neck to eye distance set in the active Oculus profile */
        std::array<Float, 2> neckToEyeDistance() const {
            std::array<Float, 2> values{{OVR_DEFAULT_NECK_TO_EYE_HORIZONTAL, OVR_DEFAULT_NECK_TO_EYE_VERTICAL}};
            ovr_GetFloatArray(_session, OVR_KEY_NECK_TO_EYE_DISTANCE, values.data(), 2);
            return values;
        }

        /** @brief State of the eye relief dial set in the active Oculus profile */
        Int eyeReliefDial() const {
            return ovr_GetInt(_session, OVR_KEY_EYE_RELIEF_DIAL, OVR_DEFAULT_EYE_RELIEF_DIAL);
        }

        /** @brief Eye to node distance set in the active Oculus profile */
        std::array<Float, 2> eyeToNoseDistance() const {
           std::array<Float, 2> values{{0.0f, 0.0f}};
            ovr_GetFloatArray(_session, OVR_KEY_EYE_TO_NOSE_DISTANCE, values.data(), 2);
            return values;
        }

        /** @brief Maximal eye to plate distance set in the active Oculus profile */
        std::array<Float, 2> maxEyeToPlateDistance() const {
            std::array<Float, 2> values{{0.0f, 0.0f}};
            ovr_GetFloatArray(_session, OVR_KEY_MAX_EYE_TO_PLATE_DISTANCE, values.data(), 2);
            return values;
        }

        /** @brief Eye cup set in the active Oculus profile */
        std::string eyeCup() const {
            return ovr_GetString(_session, OVR_KEY_EYE_CUP, "");
        }

        /** @brief Status of the OVR session */
        SessionStatusFlags sessionStatus() const;

    private:
        explicit Hmd(::ovrSession hmd);

        ::ovrSession _session;
        ::ovrHmdDesc _hmdDesc;
        ovrPosef _ovrPoses[2];
        ovrVector3f _hmdToEyeViewOffset[2];
        ::ovrViewScaleDesc _viewScale;

        Double _predictedDisplayTime;
        ovrTrackingState _trackingState;

        Long _frameIndex;

        ovrTexture* _ovrMirrorTexture;
        std::unique_ptr<Texture2D> _mirrorTexture;

        HmdStatusFlags _flags;

        friend class SwapTextureSet;
        friend class Context;
};

}}

#endif
