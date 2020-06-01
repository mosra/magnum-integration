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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>
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
    std::ostringstream out;
    Debug(&out) << HmdType::DK2;
    CORRADE_COMPARE(out.str(), "OvrIntegration::HmdType::DK2\n");

    out.str("");
    Debug(&out) << HmdType(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::HmdType::(invalid)\n");
}

void EnumTest::trackingOrigin() {
    std::ostringstream out;
    Debug(&out) << TrackingOrigin::EyeLevel;
    CORRADE_COMPARE(out.str(), "OvrIntegration::TrackingOrigin::EyeLevel\n");

    out.str("");
    Debug(&out) << TrackingOrigin(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::TrackingOrigin::(invalid)\n");
}

void EnumTest::trackerFlag() {
    std::ostringstream out;
    Debug(&out) << TrackerFlag::Connected;
    CORRADE_COMPARE(out.str(), "OvrIntegration::TrackerFlag::Connected\n");

    out.str("");
    Debug(&out) << TrackerFlag(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::TrackerFlag::(invalid)\n");
}

void EnumTest::button() {
    std::ostringstream out;
    Debug(&out) << Button::A;
    CORRADE_COMPARE(out.str(), "OvrIntegration::Button::A\n");

    out.str("");
    Debug(&out) << Button(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::Button::(invalid)\n");

    CORRADE_VERIFY((Buttons{Button::RShoulder} & Buttons::RMask) != Buttons{});
    CORRADE_VERIFY((Buttons{Button::LShoulder} & Buttons::LMask) != Buttons{});
    CORRADE_VERIFY((Buttons{Button::Home} & Buttons::PrivateMask) != Buttons{});
}

void EnumTest::touch() {
    std::ostringstream out;
    Debug(&out) << Touch::RIndexPointing;
    CORRADE_COMPARE(out.str(), "OvrIntegration::Touch::RIndexPointing\n");

    out.str("");
    Debug(&out) << Touch(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::Touch::(invalid)\n");

    CORRADE_VERIFY((Touches{Touch::RThumb} & Touches::RMask) != Touches{});
    CORRADE_VERIFY((Touches{Touch::LThumb} & Touches::LMask) != Touches{});
    CORRADE_VERIFY((Touches{Touch::RThumbUp} & Touches::RPoseMask) != Touches{});
    CORRADE_VERIFY((Touches{Touch::LThumbUp} & Touches::LPoseMask) != Touches{});
}

void EnumTest::controllerType() {
    std::ostringstream out;
    Debug(&out) << ControllerType::Remote;
    CORRADE_COMPARE(out.str(), "OvrIntegration::ControllerType::Remote\n");

    out.str("");
    Debug(&out) << ControllerType(-2);
    CORRADE_COMPARE(out.str(), "OvrIntegration::ControllerType::(invalid)\n");
}

void EnumTest::statusFlag() {
    std::ostringstream out;
    Debug(&out) << StatusFlag::OrientationTracked;
    CORRADE_COMPARE(out.str(), "OvrIntegration::StatusFlag::OrientationTracked\n");

    out.str("");
    Debug(&out) << StatusFlag(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::StatusFlag::(invalid)\n");
}

void EnumTest::mirrorOption() {
    std::ostringstream out;
    Debug(&out) << MirrorOption::IncludeGuardian;
    CORRADE_COMPARE(out.str(), "OvrIntegration::MirrorOption::IncludeGuardian\n");

    out.str("");
    Debug(&out) << MirrorOption(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::MirrorOption::(invalid)\n");
}

void EnumTest::performanceHudMode() {
    std::ostringstream out;
    Debug(&out) << PerformanceHudMode::AppRenderTiming;
    CORRADE_COMPARE(out.str(), "OvrIntegration::PerformanceHudMode::AppRenderTiming\n");

    out.str("");
    Debug(&out) << PerformanceHudMode(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::PerformanceHudMode::(invalid)\n");
}

void EnumTest::debugHudStereoMode() {
    std::ostringstream out;
    Debug(&out) << DebugHudStereoMode::Quad;
    CORRADE_COMPARE(out.str(), "OvrIntegration::DebugHudStereoMode::Quad\n");

    out.str("");
    Debug(&out) << DebugHudStereoMode(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::DebugHudStereoMode::(invalid)\n");
}

void EnumTest::layerHudMode() {
    std::ostringstream out;
    Debug(&out) << LayerHudMode::Off;
    CORRADE_COMPARE(out.str(), "OvrIntegration::LayerHudMode::Off\n");

    out.str("");
    Debug(&out) << LayerHudMode(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::LayerHudMode::(invalid)\n");
}

void EnumTest::errorType() {
    std::ostringstream out;
    Debug(&out) << ErrorType::LeakingResources;
    CORRADE_COMPARE(out.str(), "OvrIntegration::ErrorType::LeakingResources\n");

    out.str("");
    Debug(&out) << ErrorType(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::ErrorType::(invalid)\n");
}

void EnumTest::detectResult() {
    std::ostringstream out;
    Debug(&out) << DetectResult::HmdConnected;
    CORRADE_COMPARE(out.str(), "OvrIntegration::DetectResult::HmdConnected\n");

    out.str("");
    Debug(&out) << DetectResult(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::DetectResult::(invalid)\n");
}

void EnumTest::sessionStatusFlag() {
    std::ostringstream out;
    Debug(&out) << SessionStatusFlag::HmdPresent;
    CORRADE_COMPARE(out.str(), "OvrIntegration::SessionStatusFlag::HmdPresent\n");

    out.str("");
    Debug(&out) << SessionStatusFlag(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::SessionStatusFlag::(invalid)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::OvrIntegration::Test::EnumTest)
