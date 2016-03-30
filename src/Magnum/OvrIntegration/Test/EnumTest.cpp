/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
#include <sstream>

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/DualQuaternion.h"

#include "Magnum/OvrIntegration/Hmd.h"
#include "Magnum/OvrIntegration/HmdEnum.h"
#include "Magnum/OvrIntegration/Context.h"

namespace Magnum { namespace OvrIntegration { namespace Test {

struct EnumTest: TestSuite::Tester {
    explicit EnumTest();

    void hmdType();
    void hmdTrackingCapability();
    void statusFlag();
    void performanceHudMode();
    void debugHudStereoMode();
    void layerHudMode();
    void errorType();
    void detectResult();
    void sessionStatusFlag();
};

EnumTest::EnumTest() {
    addTests({&EnumTest::hmdType,
              &EnumTest::hmdTrackingCapability,
              &EnumTest::statusFlag,
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

void EnumTest::hmdTrackingCapability() {
    std::ostringstream out;
    Debug(&out) << HmdTrackingCapability::Position;
    CORRADE_COMPARE(out.str(), "OvrIntegration::HmdTrackingCapability::Position\n");

    out.str("");
    Debug(&out) << HmdTrackingCapability(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::HmdTrackingCapability::(invalid)\n");
}

void EnumTest::statusFlag() {
    std::ostringstream out;
    Debug(&out) << StatusFlag::OrientationTracked;
    CORRADE_COMPARE(out.str(), "OvrIntegration::StatusFlag::OrientationTracked\n");

    out.str("");
    Debug(&out) << StatusFlag(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::StatusFlag::(invalid)\n");
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

}}}

CORRADE_TEST_MAIN(Magnum::OvrIntegration::Test::EnumTest)
