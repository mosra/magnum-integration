#ifndef Magnum_OvrIntegration_Hmd_h
#define Magnum_OvrIntegration_Hmd_h
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

/** @file
 * @brief Class @ref Magnum::OvrIntegration::Hmd, @ref Magnum::OvrIntegration::TextureSwapChain
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
#include "Magnum/OvrIntegration/visibility.h"

namespace Magnum { namespace OvrIntegration {

/**
@brief Texture swap chain

Contains an array of textures which can be rendered to an HMD by the Oculus SDK
@ref Compositor.
@see @ref Hmd, @ref Layer
*/
class MAGNUM_OVRINTEGRATION_EXPORT TextureSwapChain {
    public:
        /**
         * @brief Constructor
         * @param hmd       HMD for which this swap texture set is created
         * @param size      Size for the textures
         */
        explicit TextureSwapChain(const Hmd& hmd,  const Vector2i& size);
        ~TextureSwapChain();

        /** @brief Currently active texture in the set */
        Texture2D& activeTexture();

        /**
         * @brief Increment to use the next texture in the set
         * @return Reference to self (for method chaining)
         */
        TextureSwapChain& commit();

        /** @brief The underlying `ovrTextureSwapChain` */
        ::ovrTextureSwapChain ovrTextureSwapChain() const {
            return _textureSwapChain;
        }

    private:
        const Hmd& _hmd;
        Vector2i _size;
        Int _curIndex;

        ::ovrTextureSwapChain _textureSwapChain;
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

A setup for such a @ref TextureSwapChain for an eye could look like this:

@code
const Int eye = 0; // left eye
Vector2i textureSize = hmd.fovTextureSize(eye);
std::unique_ptr<TextureSwapChain> textureSet = hmd.createTextureSwapChain(textureSize);

// create the framebuffer which will be used to render to the current texture
// of the texture set later.
Framebuffer framebuffer{{}, textureSize};
framebuffer.mapForDraw(Framebuffer::ColorAttachment(0));

// setup depth attachment
Texture2D* depth = new Texture2D();
depth->setMinificationFilter(Sampler::Filter::Linear)
      .setMagnificationFilter(Sampler::Filter::Linear)
      .setWrapping(Sampler::Wrapping::ClampToEdge)
      .setStorage(1, TextureFormat::DepthComponent24, textureSize);

// ...

// render to the TextureSwapChain
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

@see @ref Context, @ref TextureSwapChain, @ref Compositor
*/
class MAGNUM_OVRINTEGRATION_EXPORT Hmd {
    public:
        ~Hmd();

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
         * @param size      Size for the mirror texture
         * @return Reference to the created mirror texture. Its destruction is
         *      handled by the @ref Hmd.
         *
         * The libOVR compositor will render a copy of its result to the
         * texture returned by this method.
         */
        Texture2D& createMirrorTexture(const Vector2i& size);

        /**
         * @brief Convenience method to create a @ref TextureSwapChain for this HMD
         * @param eye       Eye index which will be used to get the preferred
         *      size for the texture.
         *
         * @see @ref createTextureSwapChain(const Vector2i&)
         */
        std::unique_ptr<TextureSwapChain> createTextureSwapChain(Int eye);

        /**
         * @brief Create a @ref TextureSwapChain for this HMD
         * @param size      Size for the textures in the created set
         *
         * @see @ref createTextureSwapChain(TextureFormat, Int)
         */
        std::unique_ptr<TextureSwapChain> createTextureSwapChain(const Vector2i& size);

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
         * @brief Re-centers the sensor position and orientation.
         *
         * This resets the (x,y,z) positional components and the yaw orientation
         * component. The Roll and pitch orientation components are always determined by
         * gravity and cannot be redefined. All future tracking will report values
         * relative to this new reference position.
         * If you are using ovrTrackerPoses then you will need to call ovr_GetTrackerPose
         * after this, because the sensor position(s) will change as a result of this.
         *
         * The headset cannot be facing vertically upward or downward but rather must be
         * roughly level otherwise this function will fail with
         * @ref OvrIntegration::Error::InvalidHeadsetOrientation.
         *
         * For more info, see the notes on each ovrTrackingOrigin enumeration to understand how
         * recenter will vary slightly in its behavior based on the current ovrTrackingOrigin setting.
         */
        void recenterTrackingOrigin() const {
            ovr_RecenterTrackingOrigin(_session);
        }

        /**
         * @brief Sets the tracking origin type
         *
         * When the tracking origin is changed, all of the calls that either provide
         * or accept ovrPosef will use the new tracking origin provided.
         */
        void setTrackingOrigin(TrackingOrigin origin) const {
            ovr_SetTrackingOriginType(_session, ovrTrackingOrigin(origin));
        }

        /**
         * @brief Clear @ref SessionStatusFlag::ShouldRecenter.
         *
         * Clears the ShouldRecenter status bit in ovrSessionStatus, allowing further recenter
         * requests to be detected. Since this is automatically done by @ref Hmd::recenterTrackingOrigin(),
         * this is only needs to be called when application is doing its own re-centering.
         */
        void clearShouldRecenterFlag() const {
            ovr_ClearShouldRecenterFlag(_session);
        }

        /** @brief Get the current frame index */
        Long currentFrameIndex() const { return _frameIndex; }

        /**
         * @brief Increment the frame index
         *
         * Returns the previous index value. This method is called by
         * @ref Compositor::submitFrame().
         */
        Long incFrameIndex() {
            return _frameIndex++;
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

        /** @brief Neck to eye distance set in the active Oculus profile */
        std::array<Float, 2> neckToEyeDistance() const {
            std::array<Float, 2> values{{OVR_DEFAULT_NECK_TO_EYE_HORIZONTAL, OVR_DEFAULT_NECK_TO_EYE_VERTICAL}};
            ovr_GetFloatArray(_session, OVR_KEY_NECK_TO_EYE_DISTANCE, values.data(), 2);
            return values;
        }

        /** @brief Eye to node distance set in the active Oculus profile */
        std::array<Float, 2> eyeToNoseDistance() const {
           std::array<Float, 2> values{{0.0f, 0.0f}};
            ovr_GetFloatArray(_session, OVR_KEY_EYE_TO_NOSE_DISTANCE, values.data(), 2);
            return values;
        }


        /** @brief Status of the OVR session */
        SessionStatusFlags sessionStatus() const;

    private:
        explicit Hmd(::ovrSession hmd);

        ::ovrSession _session;
        ::ovrHmdDesc _hmdDesc;
        ovrPosef _ovrPoses[2];
        ovrVector3f _hmdToEyeOffset[2];
        ::ovrViewScaleDesc _viewScale;

        Double _predictedDisplayTime;
        ovrTrackingState _trackingState;

        Long _frameIndex;

        ovrMirrorTexture _ovrMirrorTexture;
        std::unique_ptr<Texture2D> _mirrorTexture;

        HmdStatusFlags _flags;

        friend class TextureSwapChain;
        friend class Context;
};

}}

#endif
