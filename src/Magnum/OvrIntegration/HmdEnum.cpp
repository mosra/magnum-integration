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

#include "Magnum/OvrIntegration/HmdEnum.h"

namespace Magnum { namespace OvrIntegration {

Debug& operator<<(Debug& debug, const HmdType value) {
    switch(value) {
        #define _c(value) case HmdType::value: return debug << "OvrIntegration::HmdType::" #value;
        _c(None)
        _c(DK1)
        _c(DKHD)
        _c(DK2)
        _c(CB)
        _c(Other)
        _c(E3_2015)
        _c(ES06)
        #undef _c
    }

    return debug << "OvrIntegration::HmdType::(invalid)";
}

Debug& operator<<(Debug& debug, const HmdTrackingCapability value) {
    switch(value) {
        #define _c(value) case HmdTrackingCapability::value: return debug << "OvrIntegration::HmdTrackingCapability::" #value;
        _c(Orientation)
        _c(MagYawCorrection)
        _c(Position)
        #undef _c
    }

    return debug << "OvrIntegration::HmdTrackingCapability::(invalid)";
}

Debug& operator<<(Debug& debug, const StatusFlag value) {
    switch(value) {
        #define _c(value) case StatusFlag::value: return debug << "OvrIntegration::StatusFlag::" #value;
        _c(OrientationTracked)
        _c(PositionTracked)
        _c(CameraPoseTracked)
        _c(PositionConnected)
        _c(HmdConnected)
        #undef _c
    }

    return debug << "OvrIntegration::StatusFlag::(invalid)";
}

Debug& operator<<(Debug& debug, const SessionStatusFlag value) {
    switch(value) {
        #define _c(value) case SessionStatusFlag::value: return debug << "OvrIntegration::SessionStatusFlag::" #value;
        _c(HasVrFocus)
        _c(HmdPresent)
        #undef _c
    }

    return debug << "OvrIntegration::SessionStatusFlag::(invalid)";
}

Debug& operator<<(Debug& debug, const PerformanceHudMode value) {
    switch(value) {
        #define _c(value) case PerformanceHudMode::value: return debug << "OvrIntegration::PerformanceHudMode::" #value;
        _c(Off)
        _c(LatencyTiming)
        _c(RenderTiming)
        _c(PerfHeadroom)
        _c(VersionInfo)
        #undef _c
    }

    return debug << "OvrIntegration::PerformanceHudMode::(invalid)";
}

Debug& operator<<(Debug& debug, const DebugHudStereoMode value) {
    switch(value) {
        #define _c(value) case DebugHudStereoMode::value: return debug << "OvrIntegration::DebugHudStereoMode::" #value;
        _c(Off)
        _c(Quad)
        _c(QuadWithCrosshair)
        _c(CrosshairAtInfinity)
        #undef _c
    }

    return debug << "OvrIntegration::DebugHudStereoMode::(invalid)";
}

Debug& operator<<(Debug& debug, const LayerHudMode value) {
    switch(value) {
        #define _c(value) case LayerHudMode::value: return debug << "OvrIntegration::LayerHudMode::" #value;
        _c(Off)
        _c(Info)
        #undef _c
    }

    return debug << "OvrIntegration::LayerHudMode::(invalid)";
}

Debug& operator<<(Debug& debug, const ErrorType value) {
    switch(value) {
        #define _c(value) case ErrorType::value: return debug << "OvrIntegration::ErrorType::" #value;
        _c(MemoryAllocationFailure)
        _c(SocketCreationFailure)
        _c(InvalidSession)
        _c(Timeout)
        _c(NotInitialized)
        _c(InvalidParameter)
        _c(ServiceError)
        _c(NoHmd)
        _c(AudioReservedBegin)
        _c(AudioReservedEnd)
        _c(Initialize)
        _c(LibLoad)
        _c(LibVersion)
        _c(ServiceConnection)
        _c(ServiceVersion)
        _c(IncompatibleOS)
        _c(DisplayInit)
        _c(ServerStart)
        _c(Reinitialization)
        _c(MismatchedAdapters)
        _c(LeakingResources)
        _c(ClientVersion)
        _c(InvalidBundleAdjustment)
        _c(USBBandwidth)
        _c(USBEnumeratedSpeed)
        _c(ImageSensorCommError)
        _c(GeneralTrackerFailure)
        _c(ExcessiveFrameTruncation)
        _c(ExcessiveFrameSkipping)
        _c(SyncDisconnected)
        _c(TrackerMemoryReadFailure)
        _c(TrackerMemoryWriteFailure)
        _c(TrackerFrameTimeout)
        _c(TrackerTruncatedFrame)
        _c(HMDFirmwareMismatch)
        _c(TrackerFirmwareMismatch)
        _c(BootloaderDeviceDetected)
        _c(TrackerCalibrationError)
        _c(ControllerFirmwareMismatch)
        _c(Incomplete)
        _c(Abandoned)
        _c(DisplayLost)
        #undef _c
    }

    return debug << "OvrIntegration::ErrorType::(invalid)";
}

}}
