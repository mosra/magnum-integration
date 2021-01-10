/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
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

    #ifdef BT_USE_DOUBLE_PRECISION
    Debug{} << "Using Bullet with BT_USE_DOUBLE_PRECISION enabled";
    #endif
}

void IntegrationTest::vector() {
    Math::Vector3<btScalar> a{btScalar(1.0), btScalar(2.0), btScalar(3.0)};
    btVector3 b{btScalar(1.0), btScalar(2.0), btScalar(3.0)};

    CORRADE_COMPARE(Math::Vector3<btScalar>{b}, a);
    /* Clang can't handle {} (huh?) */
    CORRADE_VERIFY(btVector3(a) == b);
}

void IntegrationTest::matrix3() {
    /* Magnum is column-major */
    const Math::Matrix3x3<btScalar> a{
        Math::Vector3<btScalar>{btScalar( 0.133333333333333), btScalar(0.933333333333333), btScalar(-0.333333333333333)},
        Math::Vector3<btScalar>{btScalar(-0.666666666666667), btScalar(0.333333333333333), btScalar( 0.666666666666667)},
        Math::Vector3<btScalar>{btScalar( 0.733333333333333), btScalar(0.133333333333333), btScalar( 0.666666666666667)}
    };

    /* Bullet is row-major */
    const btMatrix3x3 b{
        btScalar( 0.133333333333333), btScalar(-0.666666666666667), btScalar(0.733333333333333),
        btScalar( 0.933333333333333), btScalar( 0.333333333333333), btScalar(0.133333333333333),
        btScalar(-0.333333333333333), btScalar( 0.666666666666667), btScalar(0.666666666666667)
    };

    CORRADE_COMPARE(Math::Matrix3x3<btScalar>{b}, a);

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
    CORRADE_COMPARE(Math::Quaternion<btScalar>::fromMatrix(a), Math::Quaternion<btScalar>{q});
}

void IntegrationTest::matrix4() {
    const auto rotation = Math::Quaternion<btScalar>{{btScalar(1.0), btScalar(2.0), btScalar(3.0)}, btScalar(4.0)}.normalized();
    constexpr Math::Vector3<btScalar> translation{btScalar(1.0), btScalar(2.0), btScalar(3.0)};

    const auto a = Math::Matrix4<btScalar>::from(rotation.toMatrix(), translation);
    const btTransform b{btQuaternion{rotation}, btVector3{translation}};

    CORRADE_COMPARE(Math::Matrix4<btScalar>{b}, a);

    const btTransform btA = btTransform(a);
    CORRADE_COMPARE(Math::Quaternion<btScalar>{btA.getRotation()}, rotation);
    CORRADE_COMPARE(Math::Vector3<btScalar>{btA.getOrigin()}, translation);
}

void IntegrationTest::quaternion() {
    Math::Quaternion<btScalar> a{{btScalar(1.0), btScalar(2.0), btScalar(3.0)}, btScalar(4.0)};
    btQuaternion b{btScalar(1.0), btScalar(2.0), btScalar(3.0), btScalar(4.0)};

    CORRADE_COMPARE(Math::Quaternion<btScalar>{b}, a);
    CORRADE_VERIFY(btQuaternion{a} == b);
}

}}}}

CORRADE_TEST_MAIN(Magnum::BulletIntegration::Test::IntegrationTest)
