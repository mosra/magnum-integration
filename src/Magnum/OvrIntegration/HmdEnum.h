#ifndef Magnum_OvrIntegration_HmdEnum_h
#define Magnum_OvrIntegration_HmdEnum_h
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
 * @brief Enum @ref Magnum::OvrIntegration::HmdType, @ref Magnum::OvrIntegration::HmdCapability, @ref Magnum::OvrIntegration::HmdTrackingCapability, @ref Magnum::OvrIntegration::HmdStatusFlag, enum class @ref Magnum::OvrIntegration::HmdCapabilities, @ref Magnum::OvrIntegration::HmdTrackingCapabilities, @ref Magnum::OvrIntegration::HmdStatusFlags
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
    ES06 = ovrHmd_ES06              /**< Used by Oculus internally. */
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
@brief Ovr status flag

Flags describing the current status of sensor tracking.
The values must be the same as in enum StatusBits
@see @ref OvrStatusFlags, @ref Hmd::configureTracking()
*/
enum class StatusFlag: Int {
    OrientationTracked = ovrStatus_OrientationTracked, /**< Orientation is currently tracked (connected and in use) */
    PositionTracked = ovrStatus_PositionTracked,       /**< Position is currently tracked (false if out of range) */
    CameraPoseTracked = ovrStatus_CameraPoseTracked,   /**< Camera pose is currently tracked */
    PositionConnected = ovrStatus_PositionConnected,   /**< Position tracking hardware is connected */
    HmdConnected = ovrStatus_HmdConnected              /**< HMD Display is available and connected */
};

/** @brief HMD status flags */
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
    Debug = 1 << 1
};

/** @brief HMD status flags */
typedef Containers::EnumSet<HmdStatusFlag> HmdStatusFlags;

CORRADE_ENUMSET_OPERATORS(HmdStatusFlags)

/**
@brief Performance HUD mode

@see @ref Hmd::setPerformanceHudMode()
*/
enum class PerformanceHudMode: Int {
    Off = ovrPerfHud_Off,                     /**< Turns off the performance HUD */
    LatencyTiming = ovrPerfHud_LatencyTiming, /**< Shows latency related timing info */
    RenderTiming = ovrPerfHud_RenderTiming,   /**< Unknown type */
    PerfHeadroom = ovrPerfHud_PerfHeadroom,   /**< Shows available performance headroom in a "consumer-friendly" way */
    VersionInfo = ovrPerfHud_VersionInfo      /**< Shows SDK Version Info */
};

/**
@brief Debug HUD mode

Debug HUD is provided to help developers gauge and debug the fidelity of their app's
stereo rendering characteristics. Using the provided quad and crosshair guides,
the developer can verify various aspects such as VR tracking units (e.g. meters),
stereo camera-parallax properties (e.g. making sure objects at infinity are rendered
with the proper separation), measuring VR geometry sizes and distances and more.
@see @ref Hmd::setDebugHudMode()
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

}}

#endif
