#ifndef Magnum_OvrIntegration_Session_h
#define Magnum_OvrIntegration_Session_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Class @ref Magnum::OvrIntegration::PoseState, @ref Magnum::OvrIntegration::InputState, @ref Magnum::OvrIntegration::TextureSwapChain, @ref Magnum::OvrIntegration::Session,
 */

#include <array>
#include <memory>
#include <Corrade/Containers/Array.h>
#include <Magnum/Texture.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_Keys.h>

#include "Magnum/OvrIntegration/Conversion.h"
#include "Magnum/OvrIntegration/OvrIntegration.h"
#include "Magnum/OvrIntegration/visibility.h"

namespace Magnum { namespace OvrIntegration {

namespace Implementation {
    enum class HmdStatusFlag: UnsignedByte;
    typedef Corrade::Containers::EnumSet<HmdStatusFlag> HmdStatusFlags;
}

/** @brief A full rigid body pose with first and second derivatives */
class MAGNUM_OVRINTEGRATION_EXPORT PoseState {
    public:

        /**
         * @brief Wrap a `ovrPoseStatef` as @ref PoseState
         * @return `state` as @ref PoseState reference
         */
        static PoseState& wrap(::ovrPoseStatef& state) {
            return reinterpret_cast<PoseState&>(state);
        }
        /** @overload */
        static const PoseState& wrap(const ::ovrPoseStatef& state) {
            return reinterpret_cast<const PoseState&>(state);
        }

        /**
         * @brief Constructor
         *
         * Default initializes underlying `ovrPoseStatef`.
         */
        PoseState(): _state() {}

        /**
         * @brief Constructor with initial `ovrPoseStatef`
         *
         * Initializes underlying `ovrPoseStatef` with given @p state.
         */
        PoseState(const ovrPoseStatef& state): _state(state) {}

        /** @brief Position and orientation */
        DualQuaternion pose() const {
            return DualQuaternion{_state.ThePose};
        }

        /** @brief Angular velocity in radians per second */
        Vector3 angularVelocity() const {
            return Vector3{_state.AngularVelocity};
        }

        /** @brief Velocity in meters per second */
        Vector3 LinearVelocity() const {
            return Vector3{_state.LinearVelocity};
        }

        /** @brief Angular acceleration in radians per second per second */
        Vector3 angularAcceleration() const {
            return Vector3{_state.AngularAcceleration};
        }

        /** @brief Acceleration in meters per second per second */
        Vector3 linearAcceleration() const {
            return Vector3{_state.LinearAcceleration};
        }

        /** @brief Absolute time that this pose refers to (in seconds) */
        Double time() const {
            return _state.TimeInSeconds;
        }

        /** @brief The underlying `ovrPoseStatef` */
        ::ovrPoseStatef& ovrPoseStatef() {
            return _state;
        }

    private:
        ::ovrPoseStatef _state;
};

/**
@brief Input state

Describes the complete controller input state, including Oculus Touch and XBox
gamepad. If multiple inputs are connected and used at the same time, their
inputs are combined.
*/
class MAGNUM_OVRINTEGRATION_EXPORT InputState {
    public:
        /**
         * @brief Constructor
         *
         * Default initializes underlying `ovrInputState`.
         */
        InputState(): _state() {}

        /**
         * @brief Constructor with initial `ovrInputState`
         *
         * Initializes underlying `ovrInputState` with given @p state.
         */
        InputState(const ovrInputState& state): _state(state) {}

        /** @brief Values for buttons described by `ovrButton` */
        Buttons buttons() const;

        /** @brief Touch values for buttons and sensors as described by `ovrTouch` */
        Touches touches() const;

        /**
         * @brief Left and right finger trigger values
         *
         * In the range @f$ [0.0f; 1.0f] @f$
         */
        Float indexTrigger(UnsignedInt hand) const {
            return _state.IndexTrigger[hand];
        }

        /**
         * @brief Left and right hand trigger values
         *
         * In the range @f$ [0.0f; 1.0f] @f$
         */
        Float handTrigger(UnsignedInt hand) const {
            return _state.HandTrigger[hand];
        }

        /**
         * @brief Horizontal and vertical thumbstick axis values
         *
         * In the range @f$ [-1.0f; 1.0f] @f$
         */
        Vector2 thumbstick(UnsignedInt hand) const {
            return Vector2{_state.Thumbstick[hand]};
        }

        /** @brief System time when the controller state was last updated (in seconds) */
        Double time() const {
            return _state.TimeInSeconds;
        }

        /** @brief The type of the controller this state is for */
        ControllerType controllerType() const {
            return static_cast<ControllerType>(_state.ControllerType);
        }

        /** @brief The underlying `ovrInputState` */
        ::ovrInputState& ovrInputState() {
            return _state;
        }

    private:
        ::ovrInputState _state;
};

/**
@brief Texture swap chain

Contains an array of textures which can be rendered to an HMD by the Oculus SDK
@ref Compositor.
@see @ref Session, @ref Layer
*/
class MAGNUM_OVRINTEGRATION_EXPORT TextureSwapChain {
    public:
        /**
         * @brief Constructor
         * @param session   HMD for which this texture swap chain is created
         * @param size      Size for the textures
         */
        explicit TextureSwapChain(const Session& session,  const Vector2i& size);
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
        const Session& _session;
        Vector2i _size;
        Int _curIndex;

        ::ovrTextureSwapChain _textureSwapChain;
        Containers::Array<Texture2D> _textures;
};

/**
@brief Session

Wraps `ovrSession`, `ovrHmdDesc` and methods from the Oculus SDK which directly
affect an HMD and its properties.

@section OvrIntegration-Session-usage Usage

Instances of @ref Session are created by @ref Context.

@code{.cpp}
std::unique_ptr<Session> session = Context::get().initialize().createSession();
session->configureRendering();

// ...
@endcode

Once the HMD is configured, you can poll and get the head pose.

@code{.cpp}
std::unique_ptr<DualQuaternion> poses = session->pollEyePoses().eyePoses();

DualQuaternion leftPose = poses.get()[0];
DualQuaternion rightPose = poses.get()[1];
@endcode

@section OvrIntegration-Session-usage-rendering Rendering to the HMD

Rendering to an HMD is done via the @ref Compositor. It's results are
rendered directly to the Rift. The compositor layers usually require you to
render to a set of textures which are then rendered to the rift with
distortion, chromatic abberation and possibly timewarp.

A setup for such a @ref TextureSwapChain for an eye could look like this:

@code{.cpp}
const Int eye = 0; // left eye
Vector2i textureSize = session.fovTextureSize(eye);
std::unique_ptr<TextureSwapChain> swapChain = session.createTextureSwapChain(textureSize);

// create the framebuffer which will be used to render to the current texture
// of the texture chain later.
Framebuffer framebuffer{{}, textureSize};
framebuffer.mapForDraw(Framebuffer::ColorAttachment(0));

// setup depth attachment
Texture2D* depth = new Texture2D();
depth->setMinificationFilter(Sampler::Filter::Linear)
      .setMagnificationFilter(Sampler::Filter::Linear)
      .setWrapping(Sampler::Wrapping::ClampToEdge)
      .setStorage(1, TextureFormat::DepthComponent24, textureSize);

// ...

// switch to framebuffer and attach textures
framebuffer.bind();
framebuffer.attachTexture(Framebuffer::ColorAttachment(0), _textureChain->activeTexture(), 0)
           .attachTexture(Framebuffer::BufferAttachment::Depth, *depth, 0)
           .clear(FramebufferClear::Color | FramebufferClear::Depth);

// ... render scene

// commit changes to the TextureSwapChain
swapChain->commit();

framebuffer.detach(Framebuffer::ColorAttachment(0))
           .detach(Framebuffer::BufferAttachment::Depth);
@endcode

Usually, especially for debugging, you will want to have a *mirror* of the
@ref Compositor result displayed to a window.

@code{.cpp}
Texture2D& mirrorTexture = session->createMirrorTexture(resolution);
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
class MAGNUM_OVRINTEGRATION_EXPORT Session {
    friend class Context;

    public:
        ~Session();

        /**
         * @brief Configure rendering to the Rift
         * @return Reference to self (for method chaining)
         *
         * Includes setting up HMD to eye offsets internally.
         */
        Session& configureRendering();

        /**
         * @brief Get preferred size for textures used for rendering to this HMD
         * @param eye       Eye index to get the texture size for
         */
        Vector2i fovTextureSize(Int eye);

        /**
         * @brief Create a mirror texture
         * @param size      Size for the mirror texture
         * @return Reference to the created mirror texture. Its destruction is
         *      handled by the @ref Session.
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
         * @see @ref createTextureSwapChain(Int)
         */
        std::unique_ptr<TextureSwapChain> createTextureSwapChain(const Vector2i& size);

        /**
         * @brief Get the current tracked head pose as a PoseState.
         */
        const PoseState& headPoseState() const {
            return PoseState::wrap(_trackingState.HeadPose);
        }

        /**
         * @brief The pose of the origin captured during calibration.
         *
         * Returns the origin of recentered space. As long as @ref Session::recenterTrackingOrigin()
         * has not been called, this method returns an identity transformation.
         */
        DualQuaternion calibratedOrigin() const {
            return DualQuaternion(_trackingState.CalibratedOrigin);
        }

        /**
         * @brief Get transformation for the eyes since last @ref Session::pollEyePoses() call
         *
         * Returns array of two DualQuaternions describing tranformation and
         * orientation of each eye.
         */
        std::array<DualQuaternion, 2> eyePoses() const {
            return std::array<DualQuaternion, 2>{{DualQuaternion(_ovrPoses[0]), DualQuaternion(_ovrPoses[1])}};
        }

        /**
         * @brief Get the transformation of hand trackers since last @ref Session::pollTrackers() call
         *
         * Returns array of two PoseStates describing tranformation and
         * orientation of each hand. The first referring to the left hand,
         * the second referring to the right hand.
         */
        std::array<std::reference_wrapper<const PoseState>, 2> handPoseStates() const {
            return std::array<std::reference_wrapper<const PoseState>, 2>{{
                    PoseState::wrap(_trackingState.HandPoses[0]),
                    PoseState::wrap(_trackingState.HandPoses[1])}};
        }

        /**
         * @brief Refresh cached tracking state
         * @return Reference to self (for method chaining)
         *
         * Use @ref eyePoses() to access the result.
         */
        Session& pollTrackers();

        /**
         * @brief Refresh cached eye poses
         * @return Reference to self (for method chaining)
         *
         * Use @ref eyePoses() to access the result. Calls @ref pollTrackers().
         * Call @ref pollTrackers() directly, if you do not need to calculate
         * the eye poses.
         */
        Session& pollEyePoses();

        /**
         * @brief Get input state of given controller type
         * @param types Controller type to get the input state of
         * @param state Receives the resulting input state
         * @return Reference to self (for method chaining)
         */
        Session& pollController(ControllerType types, InputState& state);

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
         *      @ref Session::projectionMatrix()
         * @param scale     Scale for the 2D plane
         * @param distance  Distance of the plane from HMD position
         * @return The projection matrix for eye
         *
         * Get a projection matrix which can be used for projection onto a 2D
         * plane orthogonal to the HMDs view/screen with distance from HMDs
         * position.
         * @see @ref Session::projectionMatrix()
         */
        Matrix4 orthoSubProjectionMatrix(Int eye, const Matrix4& proj, const Vector2& scale, Float distance) const;

        /** @brief Get the underlying `ovrSession` */
        ::ovrSession ovrSession() const { return _session; }

        /** @brief Get the underlying `ovrHmdDesc` */
        ::ovrHmdDesc ovrHmdDesc() const { return _hmdDesc; }

        /** @brief Get the underlying `ovrViewScale` */
        const ::ovrViewScaleDesc& ovrViewScaleDesc() const { return _viewScale; }

        /** @brief World scale (in meters per unit) */
        Float worldScale() const { return _viewScale.HmdSpaceToWorldScaleInMeters; }

        /**
         * @brief Set world scale
         * @param scale world scale (in meters per unit)
         * @return Reference to self (for method chaining)
         */
        Session& setWorldScale(Float scale) {
            _viewScale.HmdSpaceToWorldScaleInMeters = scale;
            return *this;
        }

        /** @brief Whether this HMD is a connection to a virtual or real device */
        bool isDebugHmd() const;

        /**
         * @brief Get a pointer to the most current eye poses as `ovrPosef`
         *
         * Returns pointer to an `ovrPosef[2]` containing the most recently
         * polled eye poses.
         * @see @ref pollEyePoses()
         */
        const Containers::StaticArrayView<2, const ovrPosef> ovrEyePoses() const {
            return _ovrPoses;
        }

        /**
         * @brief ovrHandPoseStates
         *
         * The most recent calculated pose for each hand when hand controller
         * tracking is present. The first element refers to the left
         * hand and the second to the right hand. These values
         * can be combined with the result of @ref pollController() with
         * @ref ControllerType::Touch for complete hand controller information.
         */
        const Containers::StaticArrayView<2, const ovrPoseStatef> ovrHandPoseStates() const {
            return _trackingState.HandPoses;
        }

        /**
         * @brief Re-centers the sensor position and orientation.
         *
         * This resets the (x,y,z) positional components and the yaw
         * orientation component. The Roll and pitch orientation components are
         * always determined by gravity and cannot be redefined. All future
         * tracking will report values relative to this new reference position.
         *
         * The headset cannot be facing vertically upward or downward but
         * rather must be roughly level otherwise this function will fail with
         * @ref OvrIntegration::Error::InvalidHeadsetOrientation.
         *
         * For more info, see the notes on each @ref TrackingOrigin enumeration
         * to understand how recenter will vary slightly in its behavior based
         * on the current tracking origin setting, see
         * @ref setTrackingOrigin().
         */
        void recenterTrackingOrigin() const {
            ovr_RecenterTrackingOrigin(_session);
        }

        /**
         * @brief Sets the tracking origin type
         *
         * When the tracking origin is changed, all of the calls that either
         * provide or accept ovrPosef will use the new tracking origin provided.
         */
        void setTrackingOrigin(TrackingOrigin origin) const {
            ovr_SetTrackingOriginType(_session, ovrTrackingOrigin(origin));
        }

        /**
         * @brief Clear @ref SessionStatusFlag::ShouldRecenter.
         *
         * Clears the ShouldRecenter status bit in the session status, allowing
         * further recenter requests to be detected. Since this is
         * automatically done by @ref Session::recenterTrackingOrigin(), this
         * is only needs to be called when application is doing its own
         * re-centering.
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
         * Debug HUD is provided to help developers gauge and debug the
         * fidelity of their app's stereo rendering characteristics. Using the
         * provided quad and crosshair guides, the developer can verify various
         * aspects such as VR tracking units (e.g. meters), stereo
         * camera-parallax properties (e.g. making sure objects at infinity are
         * rendered with the proper separation), measuring VR geometry sizes
         * and distances and more.
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
        SessionStatusFlags status() const;

    private:
        explicit Session(::ovrSession session);

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

        Implementation::HmdStatusFlags _flags;
};

}}

#endif
