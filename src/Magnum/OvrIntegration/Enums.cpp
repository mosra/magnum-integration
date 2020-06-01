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

#include "Enums.h"

#include <Corrade/Utility/Debug.h>

namespace Magnum { namespace OvrIntegration {

constexpr Containers::EnumSet<Button> Buttons::RMask;
constexpr Containers::EnumSet<Button> Buttons::LMask;
constexpr Containers::EnumSet<Button> Buttons::PrivateMask;

constexpr Containers::EnumSet<Touch> Touches::RMask;
constexpr Containers::EnumSet<Touch> Touches::LMask;
constexpr Containers::EnumSet<Touch> Touches::RPoseMask;
constexpr Containers::EnumSet<Touch> Touches::LPoseMask;

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
        _c(ES09)
        _c(ES11)
        _c(CV1)
        #undef _c
    }

    return debug << "OvrIntegration::HmdType::(invalid)";
}

Debug& operator<<(Debug& debug, const TrackingOrigin value) {
    switch(value) {
        #define _c(value) case TrackingOrigin::value: return debug << "OvrIntegration::TrackingOrigin::" #value;
        _c(EyeLevel)
        _c(FloorLevel)
        #undef _c
    }

    return debug << "OvrIntegration::TrackingOrigin::(invalid)";
}

Debug& operator<<(Debug& debug, const TrackerFlag value) {
    switch(value) {
        #define _c(value) case TrackerFlag::value: return debug << "OvrIntegration::TrackerFlag::" #value;
        _c(Connected)
        _c(PoseTracked)
        #undef _c
    }

    return debug << "OvrIntegration::TrackerFlag::(invalid)";
}

Debug& operator<<(Debug& debug, const Button value) {
    switch(value) {
        #define _c(value) case Button::value: return debug << "OvrIntegration::Button::" #value;
        _c(A)
        _c(B)
        _c(X)
        _c(Y)
        _c(RThumb)
        _c(RShoulder)
        _c(LThumb)
        _c(LShoulder)
        _c(Up)
        _c(Down)
        _c(Left)
        _c(Right)
        _c(Enter)
        _c(Back)
        _c(VolUp)
        _c(VolDown)
        _c(Home)
        #undef _c
    }

    return debug << "OvrIntegration::Button::(invalid)";
}

Debug& operator<<(Debug& debug, const Touch value) {
    switch(value) {
        #define _c(value) case Touch::value: return debug << "OvrIntegration::Touch::" #value;
        _c(A)
        _c(B)
        _c(X)
        _c(Y)
        _c(RThumb)
        _c(LThumb)
        _c(RIndexTrigger)
        _c(LIndexTrigger)
        _c(RIndexPointing)
        _c(RThumbUp)
        _c(LIndexPointing)
        _c(LThumbUp)
        #undef _c
    }

    return debug << "OvrIntegration::Touch::(invalid)";
}

Debug& operator<<(Debug& debug, const ControllerType value) {
    switch(value) {
        #define _c(value) case ControllerType::value: return debug << "OvrIntegration::ControllerType::" #value;
        _c(None)
        _c(LTouch)
        _c(RTouch)
        _c(Touch)
        _c(Remote)
        _c(XBox)
        _c(Active)
        #undef _c
    }

    return debug << "OvrIntegration::ControllerType::(invalid)";
}

Debug& operator<<(Debug& debug, const StatusFlag value) {
    switch(value) {
        #define _c(value) case StatusFlag::value: return debug << "OvrIntegration::StatusFlag::" #value;
        _c(OrientationTracked)
        _c(PositionTracked)
        #undef _c
    }

    return debug << "OvrIntegration::StatusFlag::(invalid)";
}

Debug& operator<<(Debug& debug, const SessionStatusFlag value) {
    switch(value) {
        #define _c(value) case SessionStatusFlag::value: return debug << "OvrIntegration::SessionStatusFlag::" #value;
        _c(IsVisible)
        _c(HmdPresent)
        _c(HmdMounted)
        _c(DisplayLost)
        _c(ShouldQuit)
        _c(ShouldRecenter)
        #undef _c
    }

    return debug << "OvrIntegration::SessionStatusFlag::(invalid)";
}

Debug& operator<<(Debug& debug, const MirrorOption value) {
    switch(value) {
        #define _c(value) case MirrorOption::value: return debug << "OvrIntegration::MirrorOption::" #value;
        _c(Default)
        _c(PostDistortion)
        _c(LeftEyeOnly)
        _c(RightEyeOnly)
        _c(IncludeGuardian)
        _c(IncludeNotifications)
        _c(IncludeSystemGui)
        #undef _c
    }

    return debug << "OvrIntegration::MirrorOption::(invalid)";
}

Debug& operator<<(Debug& debug, const PerformanceHudMode value) {
    switch(value) {
        #define _c(value) case PerformanceHudMode::value: return debug << "OvrIntegration::PerformanceHudMode::" #value;
        _c(Off)
        _c(PerfSummary)
        _c(LatencyTiming)
        _c(AppRenderTiming)
        _c(CompRenderTiming)
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
        _c(InvalidSession)
        _c(Timeout)
        _c(NotInitialized)
        _c(InvalidParameter)
        _c(ServiceError)
        _c(NoHmd)
        _c(Unsupported)
        _c(DeviceUnavailable)
        _c(InvalidHeadsetOrientation)
        _c(ClientSkippedDestroy)
        _c(ClientSkippedShutdown)
        _c(AudioDeviceNotFound)
        _c(AudioComError)
        _c(Initialize)
        _c(LibLoad)
        _c(LibVersion)
        _c(ServiceConnection)
        _c(ServiceVersion)
        _c(IncompatibleOs)
        _c(DisplayInit)
        _c(ServerStart)
        _c(Reinitialization)
        _c(MismatchedAdapters)
        _c(LeakingResources)
        _c(ClientVersion)
        _c(OutOfDateOs)
        _c(OutOfDateGfxDriver)
        _c(IncompatibleGpu)
        _c(NoValidVrDisplaySystem)
        _c(Obsolete)
        _c(DisabledOrDefaultAdapter)
        _c(HybridGraphicsNotSupported)
        _c(DisplayManagerInit)
        _c(TrackerDriverInit)
        _c(DisplayLost)
        _c(TextureSwapChainFull)
        _c(TextureSwapChainInvalid)
        _c(RuntimeException)
        #undef _c
    }

    return debug << "OvrIntegration::ErrorType::(invalid)";
}

}}
