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
    void errorType();
    void ovrDetectResult();
};

EnumTest::EnumTest() {
    addTests({&EnumTest::hmdType,
              &EnumTest::hmdTrackingCapability,
              &EnumTest::statusFlag,
              &EnumTest::performanceHudMode,
              &EnumTest::debugHudStereoMode,
              &EnumTest::errorType,
              &EnumTest::ovrDetectResult});
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
    Debug(&out) << StatusFlag::HmdConnected;
    CORRADE_COMPARE(out.str(), "OvrIntegration::StatusFlag::HmdConnected\n");

    out.str("");
    Debug(&out) << StatusFlag(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::StatusFlag::(invalid)\n");
}

void EnumTest::performanceHudMode() {
    std::ostringstream out;
    Debug(&out) << PerformanceHudMode::PerfHeadroom;
    CORRADE_COMPARE(out.str(), "OvrIntegration::PerformanceHudMode::PerfHeadroom\n");

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

void EnumTest::errorType() {
    std::ostringstream out;
    Debug(&out) << ErrorType::LeakingResources;
    CORRADE_COMPARE(out.str(), "OvrIntegration::ErrorType::LeakingResources\n");

    out.str("");
    Debug(&out) << ErrorType(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::ErrorType::(invalid)\n");
}

void EnumTest::ovrDetectResult() {
    std::ostringstream out;
    Debug(&out) << OvrDetectResult::HmdConnected;
    CORRADE_COMPARE(out.str(), "OvrIntegration::OvrDetectResult::HmdConnected\n");

    out.str("");
    Debug(&out) << OvrDetectResult(-1);
    CORRADE_COMPARE(out.str(), "OvrIntegration::OvrDetectResult::(invalid)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::OvrIntegration::Test::EnumTest)
