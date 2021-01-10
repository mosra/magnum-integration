/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#if GLM_VERSION >= 990
#define GLM_ENABLE_EXPERIMENTAL /* WTF, GLM! */
#endif
#include "Magnum/GlmIntegration/GtxIntegration.h"

namespace Magnum { namespace GlmIntegration { namespace Test { namespace {

struct GtxIntegrationTest: TestSuite::Tester {
    explicit GtxIntegrationTest();

    void dualquat();
    void ddualquat();

    void debugDualQuat();
};

GtxIntegrationTest::GtxIntegrationTest() {
    addTests({&GtxIntegrationTest::dualquat,
              &GtxIntegrationTest::ddualquat,

              &GtxIntegrationTest::debugDualQuat});
}

void GtxIntegrationTest::dualquat() {
    DualQuaternion a{{{1.0f, 2.0f, 3.0f}, 4.0f},
                     {{5.0f, 6.0f, 7.0f}, 8.0f}};
    #if GLM_VERSION*10 + GLM_VERSION_REVISION < 952
    /* In < 0.9.5.2 it is ambiguous (vec3 vs quat as second parameter) due
       to initializer list fiasco: https://github.com/g-truc/glm/issues/159.
       Also, due to the same thing, using {} gives a totally different result
       compared to (). */
    glm::dualquat b{glm::quat(4.0f, 1.0f, 2.0f, 3.0f),
                    glm::quat(8.0f, 5.0f, 6.0f, 7.0f)};
    #else
    glm::dualquat b{{4.0f, 1.0f, 2.0f, 3.0f},
                    {8.0f, 5.0f, 6.0f, 7.0f}};
    #endif

    CORRADE_COMPARE(DualQuaternion{b}, a);
    #if GLM_VERSION < 97
    /* glm::to_string() for quat is since O.9.7 */
    CORRADE_VERIFY(glm::dualquat{a} == b);
    #else
    CORRADE_COMPARE(glm::dualquat{a}, b);
    #endif

    CORRADE_COMPARE(DualQuaternion{b}.dual().vector().z(), b.dual.z);
    CORRADE_COMPARE(glm::dualquat{a}.dual.z, a.dual().vector().z());
}

void GtxIntegrationTest::ddualquat() {
    DualQuaterniond a{{{1.0, 2.0, 3.0}, 4.0},
                      {{5.0, 6.0, 7.0}, 8.0}};
    #if GLM_VERSION*10 + GLM_VERSION_REVISION < 952
    /* In < 0.9.5.2 it is ambiguous (vec3 vs quat as second parameter) due
       to initializer list fiasco: https://github.com/g-truc/glm/issues/159.
       Also, due to the same thing, using {} gives a totally different result
       compared to (). */
    glm::ddualquat b{glm::dquat(4.0, 1.0, 2.0, 3.0),
                     glm::dquat(8.0, 5.0, 6.0, 7.0)};
    #else
    glm::ddualquat b{{4.0, 1.0, 2.0, 3.0},
                     {8.0, 5.0, 6.0, 7.0}};
    #endif

    CORRADE_COMPARE(DualQuaterniond{b}, a);
    #if GLM_VERSION < 97
    /* glm::to_string() for quat is since O.9.7 */
    CORRADE_VERIFY(glm::ddualquat{a} == b);
    #else
    CORRADE_COMPARE(glm::ddualquat{a}, b);
    #endif

    CORRADE_COMPARE(DualQuaterniond{b}.dual().vector().z(), b.dual.z);
    CORRADE_COMPARE(glm::ddualquat{a}.dual.z, a.dual().vector().z());
}

void GtxIntegrationTest::debugDualQuat() {
    #if GLM_VERSION < 97
    CORRADE_SKIP("Not available in GLM < 0.9.7.");
    #else
    #if GLM_VERSION <= 990
    /* https://github.com/g-truc/glm/pull/773 */
    CORRADE_SKIP("Segfaults in 0.9.9.0 due to a GLM bug.");
    #endif

    std::ostringstream out;
    Debug{&out} << glm::highp_ddualquat{{4.0, 1.0, 2.0, 3.0},
                                        {8.0, 5.0, 6.0, 7.0}};
    CORRADE_COMPARE(out.str(), "ddualquat((4.000000, {1.000000, 2.000000, 3.000000}), (8.000000, {5.000000, 6.000000, 7.000000}))\n");
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::GlmIntegration::Test::GtxIntegrationTest)
