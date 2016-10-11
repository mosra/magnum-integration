/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2013 Jan Dupal <dupal.j@gmail.com>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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

#include <Magnum/Magnum.h>
#include "Magnum/BulletIntegration/Integration.h"
#include "Magnum/BulletIntegration/DebugDraw.h"

namespace Magnum { namespace BulletIntegration { namespace Test {

typedef Math::Vector<3, btScalar> Vector3;
typedef Math::RectangularMatrix<3, 3, btScalar> Matrix3;

struct IntegrationTest: TestSuite::Tester {
    explicit IntegrationTest();

    void vector();
    void matrix3();
    void matrix4();
    void debugDrawMode();
};

IntegrationTest::IntegrationTest() {
    addTests({&IntegrationTest::vector,
              &IntegrationTest::matrix3,
              &IntegrationTest::matrix4,
              &IntegrationTest::debugDrawMode});
}

void IntegrationTest::vector() {
    Vector3 a{1.0f, 2.0f, 3.0f};
    btVector3 b{1.0f, 2.0f, 3.0f};

    CORRADE_COMPARE(Vector3{b}, a);
    /* Clang can't handle {} (huh?) */
    CORRADE_VERIFY(btVector3(a) == b);
}

void IntegrationTest::matrix3() {
    constexpr Matrix3 a{Vector3{3.0f,  5.0f, 8.0f},
                        Vector3{4.5f,  4.0f, 7.0f},
                        Vector3{7.9f, -1.0f, 8.0f}};
    const btMatrix3x3 b{3.0f,  5.0f, 8.0f,
                        4.5f,  4.0f, 7.0f,
                        7.9f, -1.0f, 8.0f};

    CORRADE_COMPARE(Matrix3{b}, a);

    /* Comparing directly fails on floating-point inaccuracies, need to use
       fuzzy compare */
    const btMatrix3x3 btA = btMatrix3x3(a);
    const btScalar* pa = &btA[0][0];
    const btScalar* pb = &b[0][0];
    for(std::size_t i = 0; i < 9; ++i, ++pb, ++pa)
        CORRADE_COMPARE(*pa, *pb);
}

void IntegrationTest::matrix4() {
    const Quaternion rotation = Quaternion{{1.0f, 2.0f, 3.0f}, 4.0f}.normalized();
    constexpr Vector3 translation{1.0f, 2.0f, 3.0f};

    const Matrix4 a = Matrix4::from(rotation.toMatrix(), translation);
    const btTransform b{btQuaternion{rotation}, btVector3{translation}};

    CORRADE_COMPARE(Matrix4{b}, a);

    const btTransform btA = btTransform(a);
    CORRADE_COMPARE(Quaternion{btA.getRotation()}, rotation);
    CORRADE_COMPARE(Vector3{btA.getOrigin()}, translation);
}

void IntegrationTest::debugDrawMode() {
    std::ostringstream out;

    Debug(&out) << DebugDraw::DebugMode::DrawAabb << DebugDraw::DebugMode(0xbaadcafe);
    CORRADE_COMPARE(out.str(), "BulletIntegration::DebugDraw::DebugMode::DrawAabb BulletIntegration::DebugDraw::DebugMode(0xbaadcafe)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::BulletIntegration::Test::IntegrationTest)
