/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2017 sigman78 <sigman78@gmail.com>

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

#include "Magnum/Magnum.h"

#include <glm/fwd.hpp>
#if GLM_VERSION < 96
#define GLM_FORCE_RADIANS /* Otherwise 0.9.5 spits a lot of loud messages :/ */
#endif
#include "Magnum/GlmIntegration/GtcIntegration.h"

namespace Magnum { namespace GlmIntegration { namespace Test { namespace {

struct GtcIntegrationTest: TestSuite::Tester {
    explicit GtcIntegrationTest();

    void quat();
    void dquat();

    void debugQuat();
};

GtcIntegrationTest::GtcIntegrationTest() {
    addTests({&GtcIntegrationTest::quat,
              &GtcIntegrationTest::dquat,

              &GtcIntegrationTest::debugQuat});
}

void GtcIntegrationTest::quat() {
    Quaternion a{{1.0f, 2.0f, 3.0f}, 4.0f};
    #if GLM_VERSION*10 + GLM_VERSION_REVISION < 952
    /* Due to initializer list fiasco in < 0.9.5.2, using {} gives a totally
       different result. https://github.com/g-truc/glm/issues/159 */
    glm::quat b(4.0f, 1.0f, 2.0f, 3.0f);
    #else
    glm::quat b{4.0f, 1.0f, 2.0f, 3.0f};
    #endif

    CORRADE_COMPARE(Quaternion{b}, a);
    #if GLM_VERSION < 97
    /* Can't use {} in < 0.9.5.2 because it matches *anything*. Too bad this
       version is in 14.04 LTS. https://github.com/g-truc/glm/issues/159. Also,
       glm::to_string() for quat is since O.9.7. */
    CORRADE_VERIFY(glm::quat(a) == b);
    #else
    CORRADE_COMPARE(glm::quat{a}, b);
    #endif

    CORRADE_COMPARE(Quaternion{b}.vector().z(), b.z);
    /* Can't use {} in < 0.9.5.2 because it matches *anything*. Too bad this
       version is in 14.04 LTS. https://github.com/g-truc/glm/issues/159 */
    CORRADE_COMPARE(glm::quat(a).z, a.vector().z());
}

void GtcIntegrationTest::dquat() {
    Quaterniond a{{1.0, 2.0, 3.0}, 4.0};
    #if GLM_VERSION*10 + GLM_VERSION_REVISION < 952
    /* Due to initializer list fiasco in < 0.9.5.2, using {} gives a totally
       different result. https://github.com/g-truc/glm/issues/159 */
    glm::dquat b(4.0, 1.0, 2.0, 3.0);
    #else
    glm::dquat b{4.0, 1.0, 2.0, 3.0};
    #endif

    CORRADE_COMPARE(Quaterniond{b}, a);
    #if GLM_VERSION < 97
    /* Can't use {} in < 0.9.5.2 because it matches *anything*. Too bad this
       version is in 14.04 LTS. https://github.com/g-truc/glm/issues/159. Also,
       glm::to_string() for quat is since O.9.7. */
    CORRADE_VERIFY(glm::dquat(a) == b);
    #else
    CORRADE_COMPARE(glm::dquat{a}, b);
    #endif

    CORRADE_COMPARE(Quaterniond{b}.vector().z(), b.z);
    /* Can't use {} in < 0.9.5.2 because it matches *anything*. Too bad this
       version is in 14.04 LTS. https://github.com/g-truc/glm/issues/159 */
    CORRADE_COMPARE(glm::dquat(a).z, a.vector().z());
}

void GtcIntegrationTest::debugQuat() {
    #if GLM_VERSION < 97
    CORRADE_SKIP("Not available in GLM < 0.9.7.");
    #else
    std::ostringstream out;
    Debug{&out} << glm::mediump_quat{4.0f, 1.0f, 2.0f, 3.0f};
    #if GLM_VERSION < 990
    CORRADE_COMPARE(out.str(), "quat(1.000000, 2.000000, 3.000000, 4.000000)\n");
    #else
    CORRADE_COMPARE(out.str(), "quat(4.000000, {1.000000, 2.000000, 3.000000})\n");
    #endif
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::GlmIntegration::Test::GtcIntegrationTest)
