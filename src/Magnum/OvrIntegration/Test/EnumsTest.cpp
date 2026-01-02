/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/DualQuaternion.h>

#include "Magnum/OvrIntegration/Session.h"
#include "Magnum/OvrIntegration/Enums.h"
#include "Magnum/OvrIntegration/Context.h"

namespace Magnum { namespace OvrIntegration { namespace Test { namespace {

struct EnumTest: TestSuite::Tester {
    explicit EnumTest();

    void hmdType();
    void trackingOrigin();
    void trackerFlag();
    void button();
    void touch();
    void controllerType();
    void statusFlag();
    void mirrorOption();
    void performanceHudMode();
    void debugHudStereoMode();
    void layerHudMode();
    void errorType();
    void detectResult();
    void sessionStatusFlag();
};

EnumTest::EnumTest() {
    addTests({&EnumTest::hmdType,
              &EnumTest::trackingOrigin,
              &EnumTest::trackerFlag,
              &EnumTest::button,
              &EnumTest::touch,
              &EnumTest::controllerType,
              &EnumTest::statusFlag,
              &EnumTest::mirrorOption,
              &EnumTest::performanceHudMode,
              &EnumTest::debugHudStereoMode,
              &EnumTest::layerHudMode,
              &EnumTest::errorType,
              &EnumTest::detectResult,
              &EnumTest::sessionStatusFlag});
}

void EnumTest::hmdType() {
    Containers::String out;
    Debug(&out) << HmdType::DK2;
    CORRADE_COMPARE(out, "OvrIntegration::HmdType::DK2\n");

    out = {};
    Debug(&out) << HmdType(-1);
    CORRADE_COMPARE(out, "OvrIntegration::HmdType::(invalid)\n");
}

void EnumTest::trackingOrigin() {
    Containers::String out;
    Debug(&out) << TrackingOrigin::EyeLevel;
    CORRADE_COMPARE(out, "OvrIntegration::TrackingOrigin::EyeLevel\n");

    out = {};
    Debug(&out) << TrackingOrigin(-1);
    CORRADE_COMPARE(out, "OvrIntegration::TrackingOrigin::(invalid)\n");
}

void EnumTest::trackerFlag() {
    Containers::String out;
    Debug(&out) << TrackerFlag::Connected;
    CORRADE_COMPARE(out, "OvrIntegration::TrackerFlag::Connected\n");

    out = {};
    Debug(&out) << TrackerFlag(-1);
    CORRADE_COMPARE(out, "OvrIntegration::TrackerFlag::(invalid)\n");
}

void EnumTest::button() {
    Containers::String out;
    Debug(&out) << Button::A;
    CORRADE_COMPARE(out, "OvrIntegration::Button::A\n");

    out = {};
    Debug(&out) << Button(-1);
    CORRADE_COMPARE(out, "OvrIntegration::Button::(invalid)\n");

    CORRADE_VERIFY((Buttons{Button::RShoulder} & Buttons::RMask) != Buttons{});
    CORRADE_VERIFY((Buttons{Button::LShoulder} & Buttons::LMask) != Buttons{});
    CORRADE_VERIFY((Buttons{Button::Home} & Buttons::PrivateMask) != Buttons{});
}

void EnumTest::touch() {
    Containers::String out;
    Debug(&out) << Touch::RIndexPointing;
    CORRADE_COMPARE(out, "OvrIntegration::Touch::RIndexPointing\n");

    out = {};
    Debug(&out) << Touch(-1);
    CORRADE_COMPARE(out, "OvrIntegration::Touch::(invalid)\n");

    CORRADE_VERIFY((Touches{Touch::RThumb} & Touches::RMask) != Touches{});
    CORRADE_VERIFY((Touches{Touch::LThumb} & Touches::LMask) != Touches{});
    CORRADE_VERIFY((Touches{Touch::RThumbUp} & Touches::RPoseMask) != Touches{});
    CORRADE_VERIFY((Touches{Touch::LThumbUp} & Touches::LPoseMask) != Touches{});
}

void EnumTest::controllerType() {
    Containers::String out;
    Debug(&out) << ControllerType::Remote;
    CORRADE_COMPARE(out, "OvrIntegration::ControllerType::Remote\n");

    out = {};
    Debug(&out) << ControllerType(-2);
    CORRADE_COMPARE(out, "OvrIntegration::ControllerType::(invalid)\n");
}

void EnumTest::statusFlag() {
    Containers::String out;
    Debug(&out) << StatusFlag::OrientationTracked;
    CORRADE_COMPARE(out, "OvrIntegration::StatusFlag::OrientationTracked\n");

    out = {};
    Debug(&out) << StatusFlag(-1);
    CORRADE_COMPARE(out, "OvrIntegration::StatusFlag::(invalid)\n");
}

void EnumTest::mirrorOption() {
    Containers::String out;
    Debug(&out) << MirrorOption::IncludeGuardian;
    CORRADE_COMPARE(out, "OvrIntegration::MirrorOption::IncludeGuardian\n");

    out = {};
    Debug(&out) << MirrorOption(-1);
    CORRADE_COMPARE(out, "OvrIntegration::MirrorOption::(invalid)\n");
}

void EnumTest::performanceHudMode() {
    Containers::String out;
    Debug(&out) << PerformanceHudMode::AppRenderTiming;
    CORRADE_COMPARE(out, "OvrIntegration::PerformanceHudMode::AppRenderTiming\n");

    out = {};
    Debug(&out) << PerformanceHudMode(-1);
    CORRADE_COMPARE(out, "OvrIntegration::PerformanceHudMode::(invalid)\n");
}

void EnumTest::debugHudStereoMode() {
    Containers::String out;
    Debug(&out) << DebugHudStereoMode::Quad;
    CORRADE_COMPARE(out, "OvrIntegration::DebugHudStereoMode::Quad\n");

    out = {};
    Debug(&out) << DebugHudStereoMode(-1);
    CORRADE_COMPARE(out, "OvrIntegration::DebugHudStereoMode::(invalid)\n");
}

void EnumTest::layerHudMode() {
    Containers::String out;
    Debug(&out) << LayerHudMode::Off;
    CORRADE_COMPARE(out, "OvrIntegration::LayerHudMode::Off\n");

    out = {};
    Debug(&out) << LayerHudMode(-1);
    CORRADE_COMPARE(out, "OvrIntegration::LayerHudMode::(invalid)\n");
}

void EnumTest::errorType() {
    Containers::String out;
    Debug(&out) << ErrorType::LeakingResources;
    CORRADE_COMPARE(out, "OvrIntegration::ErrorType::LeakingResources\n");

    out = {};
    Debug(&out) << ErrorType(-1);
    CORRADE_COMPARE(out, "OvrIntegration::ErrorType::(invalid)\n");
}

void EnumTest::detectResult() {
    Containers::String out;
    Debug(&out) << DetectResult::HmdConnected;
    CORRADE_COMPARE(out, "OvrIntegration::DetectResult::HmdConnected\n");

    out = {};
    Debug(&out) << DetectResult(-1);
    CORRADE_COMPARE(out, "OvrIntegration::DetectResult::(invalid)\n");
}

void EnumTest::sessionStatusFlag() {
    Containers::String out;
    Debug(&out) << SessionStatusFlag::HmdPresent;
    CORRADE_COMPARE(out, "OvrIntegration::SessionStatusFlag::HmdPresent\n");

    out = {};
    Debug(&out) << SessionStatusFlag(-1);
    CORRADE_COMPARE(out, "OvrIntegration::SessionStatusFlag::(invalid)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::OvrIntegration::Test::EnumTest)
