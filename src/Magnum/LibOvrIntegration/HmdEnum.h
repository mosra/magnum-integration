#ifndef Magnum_LibOvrIntegration_HmdEnum_h
#define Magnum_LibOvrIntegration_HmdEnum_h
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
 * @brief Enum @ref Magnum::LibOvrIntegration::HmdType, @ref Magnum::LibOvrIntegration::HmdCapability, @ref Magnum::LibOvrIntegration::HmdTrackingCapability, @ref Magnum::LibOvrIntegration::HmdStatusFlag, enum class @ref Magnum::LibOvrIntegration::HmdCapabilities, @ref Magnum::LibOvrIntegration::HmdTrackingCapabilities, @ref Magnum::LibOvrIntegration::HmdStatusFlags
 *
 * @author Jonathan Hale (Squareys)
 */

#include <memory>
#include <Magnum/Texture.h>
#include <Magnum/Magnum.h>
#include <OVR_CAPI.h>

#include "Magnum/LibOvrIntegration/visibility.h"

namespace Magnum { namespace LibOvrIntegration {

/**
@brief HMD type

@see @ref Context::createHmd()
*/
enum class HmdType: UnsignedByte {
    None = ovrHmd_None,             /**< Absence of an HMD type */
    DK1 = ovrHmd_DK1,               /**< Developer Kit 1 */
    DKHD = ovrHmd_DKHD,             /**< HD prototype, aka Crystal Cove */
    DK2 = ovrHmd_DK2,               /**< Developer Kit 2 */
    BlackStar = ovrHmd_BlackStar,   /**< Black Star Prototype */
    CB = ovrHmd_CB,                 /**< Crescent Bay prototype */
    Other = ovrHmd_Other            /**< Unknown type */
};

/**
@brief HMD capability

@see @ref HmdCapabilities, @ref Hmd::setEnabledCaps()
*/
enum class HmdCapability: UnsignedInt {
    /**
     * Toggles low persistence mode on or off.
     *
     * This setting reduces eye-tracking based motion blur. Eye-tracking based
     * motion blur is caused by the viewer's focal point moving more pixels
     * than have refreshed in the same period of time.
     *
     * The disadvantage of this setting is that this reduces the average
     * brightness of the display and causes some users to perceive flicker.
     *
     * @note There is no performance cost for this option. Oculus recommends
     *      exposing it to the user as an optional setting.
     */
    LowPersistence = ovrHmdCap_LowPersistence,

    /** Adjusts prediction dynamically based on internally measured latency */
    DynamicPrediction = ovrHmdCap_DynamicPrediction,

    /** Supports rendering without VSync for debugging */
    NoVSync = ovrHmdCap_NoVSync
};

/**
@brief HMD capabilities

@see @ref Hmd::setEnabledCaps()
*/
typedef Containers::EnumSet<HmdCapability> HmdCapabilities;

CORRADE_ENUMSET_OPERATORS(HmdCapabilities)

/**
@brief HMD tracking capability

@see @ref HmdTrackingCapabilities, @ref Hmd::configureTracking()
*/
enum class HmdTrackingCapability: UnsignedInt {
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

}}

#endif
