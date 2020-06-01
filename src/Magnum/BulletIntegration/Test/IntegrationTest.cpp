/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2013 Jan Dupal <dupal.j@gmail.com>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 Max Schwarz <max.schwarz@ais.uni-bonn.de>

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
#include <Magnum/Math/Matrix4.h>

#include "Magnum/BulletIntegration/Integration.h"

namespace Magnum { namespace BulletIntegration { namespace Test { namespace {

typedef Math::Vector<3, btScalar> Vector3;
typedef Math::RectangularMatrix<3, 3, btScalar> Matrix3;

struct IntegrationTest: TestSuite::Tester {
    explicit IntegrationTest();

    void vector();
    void matrix3();
    void matrix4();
    void quaternion();
};

IntegrationTest::IntegrationTest() {
    addTests({&IntegrationTest::vector,
              &IntegrationTest::matrix3,
              &IntegrationTest::matrix4,
              &IntegrationTest::quaternion});
}

void IntegrationTest::vector() {
    Vector3 a{1.0f, 2.0f, 3.0f};
    btVector3 b{1.0f, 2.0f, 3.0f};

    CORRADE_COMPARE(Vector3{b}, a);
    /* Clang can't handle {} (huh?) */
    CORRADE_VERIFY(btVector3(a) == b);
}

void IntegrationTest::matrix3() {
    /* Magnum is column-major */
    const Matrix3 a{Vector3{ 0.133333f, 0.933333f, -0.333333f},
                    Vector3{-0.666667f, 0.333333f,  0.666667f},
                    Vector3{ 0.733333f, 0.133333f,  0.666667f}};

    /* Bullet is row-major */
    const btMatrix3x3 b{ 0.133333f, -0.666667f, 0.733333f,
                         0.933333f,  0.333333f, 0.133333f,
                        -0.333333f,  0.666667f, 0.666667f};

    CORRADE_COMPARE(Matrix3{b}, a);

    /* Comparing directly fails on floating-point inaccuracies, need to use
       fuzzy compare */
    const btMatrix3x3 btA = btMatrix3x3(a);
    const btScalar* pa = &btA[0][0];
    const btScalar* pb = &b[0][0];
    for(std::size_t i = 0; i < 9; ++i, ++pb, ++pa)
        CORRADE_COMPARE(*pa, *pb);

    /* To be extra sure, verify that conversion to quaternion gives the same
       result */
    btQuaternion q;
    b.getRotation(q);
    CORRADE_COMPARE(Quaternion::fromMatrix(a), Quaternion(q));
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

void IntegrationTest::quaternion() {
    Quaternion a{{1.0f, 2.0f, 3.0f}, 4.0f};
    btQuaternion b{1.0f, 2.0f, 3.0f, 4.0f};

    CORRADE_COMPARE(Quaternion{b}, a);
    CORRADE_VERIFY(btQuaternion{a} == b);
}

}}}}

CORRADE_TEST_MAIN(Magnum::BulletIntegration::Test::IntegrationTest)
