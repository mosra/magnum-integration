#ifndef Magnum_OvrIntegration_Enums_h
#define Magnum_OvrIntegration_Enums_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
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
 * @brief Enum @ref Magnum::OvrIntegration::HmdType, @ref Magnum::OvrIntegration::TrackingOrigin, @ref Magnum::OvrIntegration::TrackerFlag, @ref Magnum::OvrIntegration::Button, @ref Magnum::OvrIntegration::Touch, @ref Magnum::OvrIntegration::StatusFlag, @ref Magnum::OvrIntegration::SessionStatusFlag, @ref Magnum::OvrIntegration::PerformanceHudMode, @ref Magnum::OvrIntegration::DebugHudStereoMode, @ref Magnum::OvrIntegration::LayerHudMode, @ref Magnum::OvrIntegration::ErrorType, enum set @ref Magnum::OvrIntegration::TrackerFlags, @ref Magnum::OvrIntegration::Buttons, @ref Magnum::OvrIntegration::Touches, @ref Magnum::OvrIntegration::StatusFlags, @ref Magnum::OvrIntegration::SessionStatusFlags
 */

#include <Corrade/Containers/EnumSet.h>
#include <Magnum/Magnum.h>
#include <OVR_CAPI.h>

#include "Magnum/OvrIntegration/visibility.h"

namespace Magnum { namespace OvrIntegration {

/**
@brief HMD type

@see @ref Context::createSession()
*/
enum class HmdType: Int {
    /** Absence of a HMD type */
    None = ovrHmd_None,

    /** Developer Kit 1 */
    DK1 = ovrHmd_DK1,

    /** HD prototype, aka Crystal Cove. Used by Oculus internally. */
    DKHD = ovrHmd_DKHD,

    /** Developer Kit 2 */
    DK2 = ovrHmd_DK2,

    /** Crescent Bay prototype. Used by Oculus internally. */
    CB = ovrHmd_CB,

    /** Unknown type */
    Other = ovrHmd_Other,

    /** HMD demoed at E3 2015. Used by Oculus internally. */
    E3_2015 = ovrHmd_E3_2015,

    /** Used by Oculus internally */
    ES06 = ovrHmd_ES06,

    /** Used by Oculus internally */
    ES09 = ovrHmd_ES09,

    /** Used by Oculus internally */
    ES11 = ovrHmd_ES11,

    /** Consumer Version 1 */
    CV1 = ovrHmd_CV1,
};

/** @debugoperatorenum{Magnum::OvrIntegration::HmdType} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, HmdType value);

/**
@brief Tracking origin

@see @ref Session::setTrackingOrigin()
*/
enum class TrackingOrigin: Int {
    /**
     * Tracking system origin reported at eye (HMD) height.
     *
     * Prefer using this origin when your application requires matching user's
     * current physical head pose to a virtual head pose without any regards to
     * the height of the floor. Cockpit-based, or 3rd-person experiences are
     * ideal candidates.
     *
     * When used, all poses are reported as an offset transform from the profile
     * calibrated or recentered HMD pose. It is recommended that apps using this
     * origin type call @ref Session::recenterTrackingOrigin() prior to starting
     * the VR experience, but notify the user before doing so to make sure the
     * user is in a comfortable pose, facing a comfortable direction.
     */
    EyeLevel = ovrTrackingOrigin_EyeLevel,

    /**
     * Tracking system origin reported at floor height.
     *
     * Prefer using this origin when your application requires the physical
     * floor height to match the virtual floor height, such as standing
     * experiences.
     *
     * When used, all poses are reported as an offset transform from the profile
     * calibrated floor pose. Calling @ref Session::recenterTrackingOrigin() will
     * recenter the X & Z axes as well as yaw, but the Y-axis (i.e. height) will
     * continue to be reported using the floor height as the origin for all poses.
     */
    FloorLevel = ovrTrackingOrigin_FloorLevel,
};

/** @debugoperatorenum{Magnum::OvrIntegration::TrackingOrigin} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, TrackingOrigin value);

/**
@brief Tracker flag

@see @ref TrackerFlags
*/
enum class TrackerFlag: Int {
    /** The sensor is present, else the sensor is absent or offline */
    Connected = ovrTracker_Connected,

    /**
     * The sensor has a valid pose, else the pose is unavailable. This will
     * only be set if @ref TrackerFlag::Connected is set.
     */
    PoseTracked = ovrTracker_PoseTracked
};

/** @debugoperatorenum{Magnum::OvrIntegration::TrackerFlag} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, TrackerFlag value);

/**
@brief Tracker flags
*/
typedef Containers::EnumSet<TrackerFlag> TrackerFlags;

CORRADE_ENUMSET_OPERATORS(TrackerFlags)

/**
@brief Button

The Oculus SDK provides an abstraction of input for the XBox controller,
the Oculus Remote and Oculus Touch. This enum describes all possible buttons
pressable on one of these controllers.

@see @ref Buttons
*/
enum class Button: UnsignedInt {
    A = ovrButton_A,                    /**< A */
    B = ovrButton_B,                    /**< B */
    X = ovrButton_X,                    /**< X */
    Y = ovrButton_Y,                    /**< Y */

    RThumb = ovrButton_RThumb,          /**< Right thumbstick */
    RShoulder = ovrButton_RShoulder,    /**< Right shoulder */
    LThumb = ovrButton_LThumb,          /**< Left thumbstick */
    LShoulder = ovrButton_LShoulder,    /**< Left shoulder */

    Up = ovrButton_Up,                  /**< D pad up */
    Down = ovrButton_Down,              /**< D pad down */
    Left = ovrButton_Left,              /**< D pad left */
    Right = ovrButton_Right,            /**< D pad right */

    Enter = ovrButton_Enter,            /**< Start on XBox controller */
    Back = ovrButton_Back,              /**< Back on Xbox controller */

    /** Volume up. Only supported by Remote. */
    VolUp = ovrButton_VolUp,

    /** Volume down. Only supported by Remote */
    VolDown = ovrButton_VolDown,

    Home = ovrButton_Home,              /**< Home */
};

/** @debugoperatorenum{Magnum::OvrIntegration::Button} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, Button value);

CORRADE_ENUMSET_OPERATORS(Containers::EnumSet<Button>)

/** @brief Buttons */
struct MAGNUM_OVRINTEGRATION_EXPORT Buttons: Containers::EnumSet<Button> {
    /** @brief Bit mask of all buttons on the right Touch controller */
    static constexpr
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Buttons
        #else
        Containers::EnumSet<Button>
        #endif
        RMask = Button::A | Button::B | Button::RThumb | Button::RShoulder;

    /** @brief Bit mask of all buttons on the left Touch controller */
    static constexpr
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Buttons
        #else
        Containers::EnumSet<Button>
        #endif
        LMask = Button::X | Button::Y | Button::LThumb | Button::LShoulder;

    /** @brief Bit mask of buttons used by Oculus Home */
    static constexpr
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Buttons
        #else
        Containers::EnumSet<Button>
        #endif
        PrivateMask = Button::VolUp | Button::VolDown | Button::Home;

    #ifndef CORRADE_MSVC2017_COMPATIBILITY
    using EnumSet::EnumSet;
    #else
    /* MSVC tries to inherit also the private constructor and dies. Grr. */
    constexpr /*implicit*/ Buttons() = default;
    constexpr /*implicit*/ Buttons(Button value): Containers::EnumSet<Button>(value) {}
    explicit Buttons(Containers::NoInitT): Containers::EnumSet<Button>(Containers::NoInit) {}
    #endif
};

/**
@brief Touch

The Oculus SDK provides an abstraction of input for the XBox controller,
the Oculus Remote and Oculus Touch. This enum describes all possible buttons
or poses which can be "touched", therefore, the player is touching the button,
but not pressing it.

@see @ref Touches
*/
enum class Touch: UnsignedInt {
    A = ovrTouch_A, /**< A button touch */
    B = ovrTouch_B, /**< B button touch */

    X = ovrTouch_X, /**< X button touch */
    Y = ovrTouch_Y, /**< Y button touch */

    RThumb = ovrTouch_RThumb, /**< Right thumbstick touch */
    LThumb = ovrTouch_LThumb, /**< Left thumbstick touch */

    RIndexTrigger = ovrTouch_RIndexTrigger, /**< Right index finger touch */
    LIndexTrigger = ovrTouch_LIndexTrigger, /**< Left index finger touch */

    /**
     * Right index finger pose state, derived internally based on distance,
     * proximity to sensors and filtering.
     */
    RIndexPointing = ovrTouch_RIndexPointing,

    /**
     * Right thumb pose state, derived internally based on distance,
     * proximity to sensors and filtering.
     */
    RThumbUp = ovrTouch_RThumbUp,

    LIndexPointing = ovrTouch_LIndexPointing, /**< Left index finger pose state */
    LThumbUp = ovrTouch_LThumbUp, /**< Left thumb pose state */
};

/** @debugoperatorenum{Magnum::OvrIntegration::Touch} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, Touch value);

CORRADE_ENUMSET_OPERATORS(Containers::EnumSet<Touch>)

/** @brief Touches */
struct MAGNUM_OVRINTEGRATION_EXPORT Touches: Containers::EnumSet<Touch> {
    /** @brief Bit mask of all the button touches on the right controller */
    static constexpr
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Touches
        #else
        Containers::EnumSet<Touch>
        #endif
        RMask = Touch::A | Touch::B | Touch::RThumb | Touch::RIndexTrigger;

    /** @brief Bit mask of all the button touches on the left controller */
    static constexpr
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Touches
        #else
        Containers::EnumSet<Touch>
        #endif
        LMask = Touch::X | Touch::Y | Touch::LThumb | Touch::LIndexTrigger;

    /** @brief Bit mask of all right controller poses */
    static constexpr
       #ifdef DOXYGEN_GENERATING_OUTPUT
       Touches
       #else
       Containers::EnumSet<Touch>
       #endif
       RPoseMask = Touch::RIndexPointing | Touch::RThumbUp;

    /** @brief Bit mask of all left controller poses */
    static constexpr
        #ifdef DOXYGEN_GENERATING_OUTPUT
        Touches
        #else
        Containers::EnumSet<Touch>
        #endif
        LPoseMask = Touch::LIndexPointing | Touch::LThumbUp;

    #ifndef CORRADE_MSVC2017_COMPATIBILITY
    using EnumSet::EnumSet;
    #else
    /* MSVC tries to inherit also the private constructor and dies. Grr. */
    constexpr /*implicit*/ Touches() = default;
    constexpr /*implicit*/ Touches(Touch value): Containers::EnumSet<Touch>(value) {}
    explicit Touches(Containers::NoInitT): Containers::EnumSet<Touch>(Containers::NoInit) {}
    #endif
};

/**
@brief Controller type

@see @ref InputState::controllerType(), @ref Session::pollController()
*/
enum class ControllerType: Int {
    None = ovrControllerType_None,      /**< No controllers */
    LTouch = ovrControllerType_LTouch,  /**< Left Touch controller */
    RTouch = ovrControllerType_RTouch,  /**< Right Touch controller */
    Touch = ovrControllerType_Touch,    /**< Left and right Touch controllers */
    Remote = ovrControllerType_Remote,  /**< Oculus Remote */
    XBox   = ovrControllerType_XBox,    /**< XBox controller */

    /** Operate on or query whichever controller is active */
    Active   = ovrControllerType_Active,
};

/** @debugoperatorenum{Magnum::OvrIntegration::ControllerType} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, ControllerType value);

/**
@brief Status flag

Flags describing the current status of sensor tracking.
@see @ref StatusFlags, @ref Session::trackingState()
*/
enum class StatusFlag: Int {
    /** Orientation is currently tracked (connected and in use) */
    OrientationTracked = ovrStatus_OrientationTracked,

    /** Position is currently tracked (false if out of range) */
    PositionTracked = ovrStatus_PositionTracked,
};

/** @debugoperatorenum{Magnum::OvrIntegration::StatusFlag} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, StatusFlag value);

/**
@brief Status flags

@see @ref Session::trackingState()
*/
typedef Containers::EnumSet<StatusFlag> StatusFlags;

CORRADE_ENUMSET_OPERATORS(StatusFlags)

/**
@brief Session status flag

@see @ref SessionStatusFlags, @ref Session::status()
*/
enum class SessionStatusFlag: UnsignedByte {
    /** Set when the process has VR focus and thus is visible in the HMD */
    IsVisible = 0,

    /** Set when an HMD is present */
    HmdPresent = 1,

    /** Set when the HMD is on the user's head */
    HmdMounted = 2,

    /** Set when the session is in a display-lost state. See ovr_SubmitFrame. */
    DisplayLost = 3,

    /** Set when the application should initiate shutdown. */
    ShouldQuit = 4,

    /** Set when UX has requested re-centering. Must call ovr_ClearShouldRecenterFlag or ovr_RecenterTrackingOrigin. */
    ShouldRecenter = 5,
};

/** @debugoperatorenum{Magnum::OvrIntegration::SessionStatusFlag} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, SessionStatusFlag value);

/**
@brief Session status flags

@see @ref Session::status()
*/
typedef Containers::EnumSet<SessionStatusFlag> SessionStatusFlags;

CORRADE_ENUMSET_OPERATORS(SessionStatusFlags)

/**
@brief Mirror option

@see @ref Session::createMirrorTexture()
*/
enum class MirrorOption: UnsignedInt {
    Default = ovrMirrorOption_Default,                  /**< Default */
    PostDistortion = ovrMirrorOption_PostDistortion,    /**< Post distortion */
    LeftEyeOnly = ovrMirrorOption_LeftEyeOnly,          /**< Left eye only */
    RightEyeOnly = ovrMirrorOption_RightEyeOnly,        /**< Right eye only */
    IncludeGuardian = ovrMirrorOption_IncludeGuardian,  /**< Include Guardian */
    IncludeNotifications = ovrMirrorOption_IncludeNotifications,  /**< Include Notifications */
    IncludeSystemGui = ovrMirrorOption_IncludeSystemGui /**< Include system GUI */
};

/** @debugoperatorenum{Magnum::OvrIntegration::MirrorOption} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, MirrorOption value);

/**
@brief Tracker flags
*/
typedef Containers::EnumSet<MirrorOption> MirrorOptions;

CORRADE_ENUMSET_OPERATORS(MirrorOptions)

/**
@brief Performance HUD mode

@see @ref Session::setPerformanceHudMode()
*/
enum class PerformanceHudMode: Int {
    Off = ovrPerfHud_Off,                     /**< Turns off the performance HUD */
    PerfSummary = ovrPerfHud_PerfSummary,     /**< Shows performance summary and headroom */
    LatencyTiming = ovrPerfHud_LatencyTiming, /**< Shows latency related timing info */
    AppRenderTiming = ovrPerfHud_AppRenderTiming,  /**< Shows render timing info for application */
    CompRenderTiming = ovrPerfHud_CompRenderTiming,/**< Shows render timing info for OVR compositor */
    VersionInfo = ovrPerfHud_VersionInfo      /**< Shows SDK & HMD version Info */
};

/** @debugoperatorenum{Magnum::OvrIntegration::PerformanceHudMode} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, PerformanceHudMode value);

/**
@brief Debug HUD mode

@see @ref Session::setDebugHudStereoMode()
*/
enum class DebugHudStereoMode: Int {
    /** Turns off the Stereo Debug HUD */
    Off = ovrDebugHudStereo_Off,

    /** Renders Quad in world for Stereo Debugging */
    Quad = ovrDebugHudStereo_Quad,

    /** Renders Quad+crosshair in world for Stereo Debugging */
    QuadWithCrosshair = ovrDebugHudStereo_QuadWithCrosshair,

    /** Renders screen-space crosshair at infinity for Stereo Debugging */
    CrosshairAtInfinity = ovrDebugHudStereo_CrosshairAtInfinity
};

/** @debugoperatorenum{Magnum::OvrIntegration::DebugHudStereoMode} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, DebugHudStereoMode value);

/**
@brief Layer HUD mode

@see @ref Session::setLayerHudMode()
*/
enum class LayerHudMode: Int {
    /** Turns off the layer HUD */
    Off = ovrLayerHud_Off,

    /** Shows info about a specific layer */
    Info = ovrLayerHud_Info,
};

/** @debugoperatorenum{Magnum::OvrIntegration::LayerHudMode} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, LayerHudMode value);

/**
@brief Error type

@see @ref Error, @ref Context::error()
*/
enum class ErrorType: Int {
    /* General errors */

    /** Failure to allocate memory */
    MemoryAllocationFailure = ovrError_MemoryAllocationFailure,

    /** Invalid ovrSession parameter provided */
    InvalidSession = ovrError_InvalidSession,

    /** The operation timed out */
    Timeout = ovrError_Timeout,

    /** The system or component has not been initialized */
    NotInitialized = ovrError_NotInitialized,

    /** Invalid parameter provided. See error info or log for details */
    InvalidParameter = ovrError_InvalidParameter,

    /** Generic service error. See error info or log for details */
    ServiceError = ovrError_ServiceError,

    /** The given HMD doesn't exist */
    NoHmd = ovrError_NoHmd,

    /** Function call is not supported on this hardware/software */
    Unsupported = ovrError_Unsupported,

    /** Specified device type isn't available */
    DeviceUnavailable = ovrError_DeviceUnavailable,

    /**
     * The headset was in an invalid orientation for the requested operation
     * (e.g. vertically oriented during @ref Session::recenterTrackingOrigin())
     */
    InvalidHeadsetOrientation = ovrError_InvalidHeadsetOrientation,

    /**
     * The client failed to destroy @ref Session on an active session  before
     * destroying @ref Context. Or the client crashed.
     */
    ClientSkippedDestroy = ovrError_ClientSkippedDestroy,

    /** The client failed to destroy @ref Context or the client crashed */
    ClientSkippedShutdown = ovrError_ClientSkippedShutdown,

    /* Audio errors */

    /** Failure to find the specified audio device */
    AudioDeviceNotFound =  ovrError_AudioDeviceNotFound,

    /** Generic COM error */
    AudioComError = ovrError_AudioComError,

    /* Initialization errors */

    /** Generic initialization error */
    Initialize = ovrError_Initialize,

    /** Couldn't load LibOVRRT */
    LibLoad = ovrError_LibLoad,

    /** LibOVRRT version incompatibility */
    LibVersion = ovrError_LibVersion,

    /** Couldn't connect to the OVR Service */
    ServiceConnection = ovrError_ServiceConnection,

    /** OVR Service version incompatibility */
    ServiceVersion = ovrError_ServiceVersion,

    /** The operating system version is incompatible */
    IncompatibleOs = ovrError_IncompatibleOS,

    /** Unable to initialize the HMD display */
    DisplayInit = ovrError_DisplayInit,

    /** Unable to start the server. Is it already running? */
    ServerStart = ovrError_ServerStart,

    /** Attempting to re-initialize with a different version */
    Reinitialization = ovrError_Reinitialization,

    /** Chosen rendering adapters between client and service do not match */
    MismatchedAdapters = ovrError_MismatchedAdapters,

    /** Calling application has leaked resources */
    LeakingResources = ovrError_LeakingResources,

    /** Client version too old to connect to service */
    ClientVersion = ovrError_ClientVersion,

    /** The operating system is out of date */
    OutOfDateOs = ovrError_OutOfDateOS,

    /** The graphics driver is out of date */
    OutOfDateGfxDriver = ovrError_OutOfDateGfxDriver,

    /** The graphics hardware is not supported */
    IncompatibleGpu = ovrError_IncompatibleGPU,

    /** No valid VR display system found */
    NoValidVrDisplaySystem = ovrError_NoValidVRDisplaySystem,

    /** Feature or API is obsolete and no longer supported */
    Obsolete = ovrError_Obsolete,

    /**
     * No supported VR display system found, but disabled or driverless adapter
     * found
     */
    DisabledOrDefaultAdapter = ovrError_DisabledOrDefaultAdapter,

    /**
     * The system is using hybrid graphics (Optimus, etc...), which is not
     * supported
     */
    HybridGraphicsNotSupported = ovrError_HybridGraphicsNotSupported,

    /** Initialization of the display manager failed. */
    DisplayManagerInit = ovrError_DisplayManagerInit,

    /** Failed to get the interface for an attached tracker */
    TrackerDriverInit = ovrError_TrackerDriverInit,

    /* Rendering errors */

    /**
     * In the event of a system-wide graphics reset or cable unplug this is
     * returned to the app.
     */
    DisplayLost = ovrError_DisplayLost,

    /**
     * @ref TextureSwapChain::commit() was called too many times on a texture
     * swapchain without calling submit to use the chain
     */
    TextureSwapChainFull = ovrError_TextureSwapChainFull,

    /**
     * The @ref TextureSwapChain is in an incomplete or inconsistent state.
     * Ensure @ref TextureSwapChain::commit() was called at least once first.
     */
    TextureSwapChainInvalid = ovrError_TextureSwapChainInvalid,

    /* Fatal errors */

    /**
     * A runtime exception occurred. The application is required to shutdown
     * LibOVR and re-initialize it before this error state will be cleared.
     */
    RuntimeException = ovrError_RuntimeException,

};

/** @debugoperatorenum{Magnum::OvrIntegration::ErrorType} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, ErrorType value);

}}

#endif
