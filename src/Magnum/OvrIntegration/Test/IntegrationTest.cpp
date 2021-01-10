/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
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

#include <Corrade/TestSuite/Tester.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/DualQuaternion.h>

#include "Magnum/OvrIntegration/Integration.h"

namespace Magnum { namespace OvrIntegration { namespace Test { namespace {

struct IntegrationTest: TestSuite::Tester {
    explicit IntegrationTest();

    void sizei();
    void vector2i();
    void vector2f();
    void vector3f();
    void recti();
    void matrix4f();
    void quatf();
    void posef();
};

IntegrationTest::IntegrationTest() {
    addTests({&IntegrationTest::sizei,
              &IntegrationTest::vector2i,
              &IntegrationTest::vector2f,
              &IntegrationTest::vector3f,
              &IntegrationTest::recti,
              &IntegrationTest::matrix4f,
              &IntegrationTest::quatf,
              &IntegrationTest::posef});
}

void IntegrationTest::sizei() {
    Vector2i a{1, 2};
    ovrSizei b{1, 2};

    CORRADE_COMPARE(Vector2i(b), a);

    ovrSizei c(a);
    CORRADE_COMPARE(c.w, b.w);
    CORRADE_COMPARE(c.h, b.h);
}

void IntegrationTest::vector2i() {
    Vector2i a{1, 2};
    ovrVector2i b{1, 2};

    CORRADE_COMPARE(Vector2i(b), a);

    ovrVector2i c(a);
    CORRADE_COMPARE(c.x, b.x);
    CORRADE_COMPARE(c.y, b.y);
}

void IntegrationTest::vector2f() {
    Vector2 a{1.0f, 2.0f};
    ovrVector2f b{1.0f, 2.0f};

    CORRADE_COMPARE(Vector2(b), a);

    ovrVector2f c(a);
    CORRADE_COMPARE(c.x, b.x);
    CORRADE_COMPARE(c.y, b.y);
}

void IntegrationTest::vector3f() {
    Vector3 a{1.0f, 2.0f, 3.0f};
    ovrVector3f b{1.0f, 2.0f, 3.0f};

    CORRADE_COMPARE(Vector3(b), a);

    ovrVector3f c(a);
    CORRADE_COMPARE(c.x, b.x);
    CORRADE_COMPARE(c.y, b.y);
    CORRADE_COMPARE(c.z, b.z);
}

void IntegrationTest::recti() {
    Range2Di a{{2, 2}, {42, 42}};
    ovrRecti b{{2, 2}, {40, 40}};

    CORRADE_COMPARE(Range2Di(b), a);

    ovrRecti c(a);
    CORRADE_COMPARE(c.Pos.x, b.Pos.x);
    CORRADE_COMPARE(c.Pos.y, b.Pos.y);
    CORRADE_COMPARE(c.Size.w, b.Size.w);
    CORRADE_COMPARE(c.Size.h, b.Size.h);
}

void IntegrationTest::matrix4f() {
    Matrix4 a{{1.1f, 1.2f, 1.3f, 1.4f},
              {2.1f, 2.2f, 2.3f, 2.4f},
              {3.1f, 3.2f, 3.3f, 3.4f},
              {4.1f, 4.2f, 4.3f, 4.4f}};
    ovrMatrix4f b{{{1.1f, 1.2f, 1.3f, 1.4f},
                   {2.1f, 2.2f, 2.3f, 2.4f},
                   {3.1f, 3.2f, 3.3f, 3.4f},
                   {4.1f, 4.2f, 4.3f, 4.4f}}};

    /* Transposing accomodates for the sdk storing matrices in colum-major form */
    CORRADE_COMPARE(Matrix4(b).transposed(), a);

    ovrMatrix4f c(a.transposed());
    float* pb = &b.M[0][0];
    float* pc = &c.M[0][0];
    for(std::size_t i = 0; i != 16; ++i, ++pb, ++pc)
        CORRADE_COMPARE(*pc, *pb);
}

void IntegrationTest::quatf() {
    Quaternion a{{0.1f, 0.2f, 0.3f}, 1.0f};
    ovrQuatf b{0.1f, 0.2f, 0.3f, 1.0f};

    CORRADE_COMPARE(Quaternion(b), a);

    ovrQuatf c(a);
    CORRADE_COMPARE(c.x, b.x);
    CORRADE_COMPARE(c.y, b.y);
    CORRADE_COMPARE(c.z, b.z);
    CORRADE_COMPARE(c.w, b.w);
}

void IntegrationTest::posef() {
    Quaternion q = Quaternion{{0.1f, 0.2f, 0.3f}, 1.0}.normalized();
    DualQuaternion a = DualQuaternion::translation({1.0f, 2.0f, 3.0f})*DualQuaternion{q};
    ovrPosef b{ovrQuatf(q), {1.0f, 2.0f, 3.0f}};

    CORRADE_COMPARE(DualQuaternion(b), a);

    ovrPosef c(a);
    CORRADE_COMPARE(c.Orientation.x, b.Orientation.x);
    CORRADE_COMPARE(c.Orientation.y, b.Orientation.y);
    CORRADE_COMPARE(c.Orientation.z, b.Orientation.z);
    CORRADE_COMPARE(c.Orientation.w, b.Orientation.w);

    CORRADE_COMPARE(c.Position.x, b.Position.x);
    CORRADE_COMPARE(c.Position.y, b.Position.y);
    CORRADE_COMPARE(c.Position.z, b.Position.z);
}

}}}}

CORRADE_TEST_MAIN(Magnum::OvrIntegration::Test::IntegrationTest)
