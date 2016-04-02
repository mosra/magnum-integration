#ifndef Magnum_OvrIntegration_HmdEnum_h
#define Magnum_OvrIntegration_HmdEnum_h
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
 * @brief Enum @ref Magnum::OvrIntegration::HmdType, @ref Magnum::OvrIntegration::HmdTrackingCapability, @ref Magnum::OvrIntegration::HmdStatusFlag, enum class @ref Magnum::OvrIntegration::HmdTrackingCapabilities, @ref Magnum::OvrIntegration::HmdStatusFlags
 *
 * @author Jonathan Hale (Squareys)
 */

#include <memory>
#include <Magnum/Texture.h>
#include <Magnum/Magnum.h>
#include <OVR_CAPI.h>

#include "Magnum/OvrIntegration/visibility.h"

namespace Magnum { namespace OvrIntegration {

/**
@brief HMD type

@see @ref Context::createHmd()
*/
enum class HmdType: Int {
    None = ovrHmd_None,             /**< Absence of an HMD type */
    DK1 = ovrHmd_DK1,               /**< Developer Kit 1 */
    DKHD = ovrHmd_DKHD,             /**< HD prototype, aka Crystal Cove. Used by Oculus internally. */
    DK2 = ovrHmd_DK2,               /**< Developer Kit 2 */
    CB = ovrHmd_CB,                 /**< Crescent Bay prototype. Used by Oculus internally. */
    Other = ovrHmd_Other,           /**< Unknown type */
    E3_2015 = ovrHmd_E3_2015,       /**< Hmd demoed at E3 2015. Used by Oculus internally. */
    ES06 = ovrHmd_ES06,             /**< Used by Oculus internally. */
    ES09 = ovrHmd_ES09,             /**< Used by Oculus internally. */
    ES11 = ovrHmd_ES11,             /**< Used by Oculus internally. */
    CV1 = ovrHmd_CV1,               /**< Consumer Version 1 */
};

/**
@brief HMD tracking capability

@see @ref HmdTrackingCapabilities, @ref Hmd::configureTracking()
*/
enum class HmdTrackingCapability: Int {
    /** Supports orientation tracking (IMU) */
    Orientation = ovrTrackingCap_Orientation,

    /** Supports yaw drift correction via a magnetometer or other means */
    MagYawCorrection = ovrTrackingCap_MagYawCorrection,

    /** Supports positional tracking */
    Position = ovrTrackingCap_Position,
};

/**
@brief HMD tracking capabilities

@see @ref Hmd::configureTracking()
*/
typedef Containers::EnumSet<HmdTrackingCapability> HmdTrackingCapabilities;

CORRADE_ENUMSET_OPERATORS(HmdTrackingCapabilities)

/**
@brief Tracker flag

@see @ref TrackerFlags
*/
enum class TrackerFlag: Int {
    /**< The sensor is present, else the sensor is absent or offline */
    Connected = ovrTracker_Connected,

    /**
     * The sensor has a valid pose, else the pose is unavailable.
     * This will only be set if ovrTracker_Connected is set.
     */
    PoseTracked = ovrTracker_PoseTracked
};

/**
@brief Tracker flags
*/
typedef Containers::EnumSet<TrackerFlag> TrackerFlags;

CORRADE_ENUMSET_OPERATORS(TrackerFlags)

@brief Status flag

Flags describing the current status of sensor tracking.
The values must be the same as in enum StatusBits
@see @ref StatusFlags, @ref Hmd::configureTracking()
*/
enum class StatusFlag: Int {
    OrientationTracked = ovrStatus_OrientationTracked, /**< Orientation is currently tracked (connected and in use) */
    PositionTracked = ovrStatus_PositionTracked,       /**< Position is currently tracked (false if out of range) */
};

/** @brief Status flags */
typedef Containers::EnumSet<StatusFlag> StatusFlags;

CORRADE_ENUMSET_OPERATORS(StatusFlags)

/** @brief HMD status flag */
enum class HmdStatusFlag: UnsignedByte {
    /**
     * A mirror texture was created for the hmd and needs to be destroyed on
     * destruction of the HMD
     */
    HasMirrorTexture = 1 << 0,

    /** The HMD was created as a debug HMD (without real hardware) */
    Debug = ovrHmdCap_DebugDevice /* 0x0010 */
};

/** @brief HMD status flags */
typedef Containers::EnumSet<HmdStatusFlag> HmdStatusFlags;

CORRADE_ENUMSET_OPERATORS(HmdStatusFlags)

/** @brief Session status flag */
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

/** @brief Session status flags */
typedef Corrade::Containers::EnumSet<SessionStatusFlag> SessionStatusFlags;

CORRADE_ENUMSET_OPERATORS(SessionStatusFlags)

/**
@brief Performance HUD mode

@see @ref Hmd::setPerformanceHudMode()
*/
enum class PerformanceHudMode: Int {
    Off = ovrPerfHud_Off,                     /**< Turns off the performance HUD */
    PerfSummary = ovrPerfHud_PerfSummary,     /**< Shows performance summary and headroom */
    LatencyTiming = ovrPerfHud_LatencyTiming, /**< Shows latency related timing info */
    AppRenderTiming = ovrPerfHud_AppRenderTiming,  /**< Shows render timing info for application */
    CompRenderTiming = ovrPerfHud_CompRenderTiming,/**< Shows render timing info for OVR compositor */
    VersionInfo = ovrPerfHud_VersionInfo      /**< Shows SDK & HMD version Info */
};

/**
@brief Debug HUD mode

@see @ref Hmd::setDebugHudStereoMode()
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

/**
@brief Layer HUD mode

@see @ref Hmd::setLayerHudMode()
*/
enum class LayerHudMode: Int {
    /** Turns off the layer HUD */
    Off = ovrLayerHud_Off,

    /** Shows info about a specific layer */
    Info = ovrLayerHud_Info,
};

/**
@brief Error type

@see @ref Error, @ref Context::error()
*/
enum class ErrorType: Int {
    /* General errors */
    MemoryAllocationFailure = ovrError_MemoryAllocationFailure, /**< Failure to allocate memory. */
    SocketCreationFailure = ovrError_SocketCreationFailure,     /**< Failure to create a socket. */
    InvalidSession = ovrError_InvalidSession,           /**< Invalid ovrSession parameter provided. */
    Timeout = ovrError_Timeout,                         /**< The operation timed out. */
    NotInitialized = ovrError_NotInitialized,           /**< The system or component has not been initialized. */
    InvalidParameter = ovrError_InvalidParameter,       /**< Invalid parameter provided. See error info or log for details. */
    ServiceError = ovrError_ServiceError,               /**< Generic service error. See error info or log for details. */
    NoHmd = ovrError_NoHmd,                             /**< The given HMD doesn't exist. */

    /* Audio errors. */
    AudioDeviceNotFound =  ovrError_AudioDeviceNotFound,/**< Failure to find the specified audio device. */
    AudioComError = ovrError_AudioComError,             /**< Generic COM error. */

    /* Initialization errors. */
    Initialize = ovrError_Initialize,                   /**< Generic initialization error. */
    LibLoad = ovrError_LibLoad,                         /**< Couldn't load LibOVRRT. */
    LibVersion = ovrError_LibVersion,                   /**< LibOVRRT version incompatibility. */
    ServiceConnection = ovrError_ServiceConnection,     /**< Couldn't connect to the OVR Service. */
    ServiceVersion = ovrError_ServiceVersion,           /**< OVR Service version incompatibility. */
    IncompatibleOs = ovrError_IncompatibleOS,           /**< The operating system version is incompatible. */
    DisplayInit = ovrError_DisplayInit,                 /**< Unable to initialize the HMD display. */
    ServerStart = ovrError_ServerStart,                 /**< Unable to start the server. Is it already running? */
    Reinitialization = ovrError_Reinitialization,       /**< Attempting to re-initialize with a different version. */
    MismatchedAdapters = ovrError_MismatchedAdapters,   /**< Chosen rendering adapters between client and service do not match */
    LeakingResources = ovrError_LeakingResources,       /**< Calling application has leaked resources */
    ClientVersion = ovrError_ClientVersion,             /**< Client version too old to connect to service */
    OutOfDateOs = ovrError_OutOfDateOS,                 /**< The operating system is out of date. */
    OutOfDateGfxDriver = ovrError_OutOfDateGfxDriver,   /**< The graphics driver is out of date. */
    IncompatibleGpu = ovrError_IncompatibleGPU,         /**< The graphics hardware is not supported */
    NoValidVrDisplaySystem = ovrError_NoValidVRDisplaySystem,   /**< No valid VR display system found. */

    /* Hardware Errors */
    InvalidBundleAdjustment = ovrError_InvalidBundleAdjustment,     /**< Headset has no bundle adjustment data. */
    UsbBandwidth = ovrError_USBBandwidth,                           /**< The USB hub cannot handle the camera frame bandwidth. */
    UsbEnumeratedSpeed = ovrError_USBEnumeratedSpeed,               /**< The USB camera is not enumerating at the correct device speed. */
    ImageSensorCommError = ovrError_ImageSensorCommError,           /**< Unable to communicate with the image sensor. */
    GeneralTrackerFailure = ovrError_GeneralTrackerFailure,         /**< We use this to report various tracker issues that don't fit in an easily classifiable bucket. */
    ExcessiveFrameTruncation = ovrError_ExcessiveFrameTruncation,   /**< A more than acceptable number of frames are coming back truncated. */
    ExcessiveFrameSkipping = ovrError_ExcessiveFrameSkipping,       /**< A more than acceptable number of frames have been skipped. */
    SyncDisconnected = ovrError_SyncDisconnected,                   /**< The tracker is not receiving the sync signal (cable disconnected?) */
    TrackerMemoryReadFailure = ovrError_TrackerMemoryReadFailure,   /**< Failed to read memory from the tracker */
    TrackerMemoryWriteFailure = ovrError_TrackerMemoryWriteFailure, /**< Failed to write memory from the tracker */
    TrackerFrameTimeout = ovrError_TrackerFrameTimeout,             /**< Timed out waiting for a camera frame */
    TrackerTruncatedFrame = ovrError_TrackerTruncatedFrame,         /**< Truncated frame returned from tracker */
    HmdFirmwareMismatch = ovrError_HMDFirmwareMismatch,             /**< The HMD Firmware is out of date and is unacceptable. */
    TrackerFirmwareMismatch = ovrError_TrackerFirmwareMismatch,     /**< The Tracker Firmware is out of date and is unacceptable. */
    BootloaderDeviceDetected = ovrError_BootloaderDeviceDetected,   /**< A bootloader HMD is detected by the service */
    TrackerCalibrationError = ovrError_TrackerCalibrationError,     /**< The tracker calibration is missing or incorrect */
    ControllerFirmwareMismatch = ovrError_ControllerFirmwareMismatch,/**< The controller firmware is out of date and is unacceptable */

    /* Synchronization Errors */
    Incomplete = ovrError_Incomplete,   /**< Requested async work not yet complete. */
    Abandoned = ovrError_Abandoned,     /**< Requested async work was abandoned and result is incomplete. */

    /* Rendering Errors */
    DisplayLost = ovrError_DisplayLost, /**< In the event of a system-wide graphics reset or cable unplug this is returned to the app */

    /* Fatal errors */
    /**
     * A runtime exception occurred. The application is required to shutdown
     * LibOVR and re-initialize it before this error state will be cleared.
     */
    RuntimeException = ovrError_RuntimeException,
};

/** @debugoperatorenum{Magnum::OvrIntegration::HmdType} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, HmdType value);

/** @debugoperatorenum{Magnum::OvrIntegration::HmdTrackingCapability} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, HmdTrackingCapability value);

/** @debugoperatorenum{Magnum::OvrIntegration::StatusFlag} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, StatusFlag value);

/** @debugoperatorenum{Magnum::OvrIntegration::SessionStatusFlag} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, SessionStatusFlag value);

/** @debugoperatorenum{Magnum::OvrIntegration::PerformanceHudMode} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, PerformanceHudMode value);

/** @debugoperatorenum{Magnum::OvrIntegration::DebugHudStereoMode} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, DebugHudStereoMode value);

/** @debugoperatorenum{Magnum::OvrIntegration::LayerHudMode} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, LayerHudMode value);

/** @debugoperatorenum{Magnum::OvrIntegration::ErrorType} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, ErrorType value);

}}

#endif
