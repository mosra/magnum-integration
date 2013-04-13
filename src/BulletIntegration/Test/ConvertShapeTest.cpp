/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2013 Jan Dupal <dupal.j@gmail.com>

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

#include <btBulletDynamicsCommon.h>
#include <TestSuite/Tester.h>
#include <Physics/Box.h>
#include <Physics/Sphere.h>

#include "BulletIntegration/ConvertShape.h"

namespace Magnum { namespace BulletIntegration { namespace Test {

class ConvertShapeTest: public Corrade::TestSuite::Tester {
    public:
        ConvertShapeTest();

        void box();
        void sphere();
};

ConvertShapeTest::ConvertShapeTest() {
    addTests({&ConvertShapeTest::box,
              &ConvertShapeTest::sphere});
}

void ConvertShapeTest::box() {
    btBoxShape btBox({0.5f, 1.0f, 1.5f});
    Physics::Box3D* box = convertShape(&btBox);
    CORRADE_COMPARE(box->transformation(), Matrix4::scaling({0.5f, 1.0f, 1.5f}));
}

void ConvertShapeTest::sphere() {
    btSphereShape btSphereShape(42.f);
    Physics::Sphere3D* sphere = convertShape(&btSphereShape);
    CORRADE_COMPARE(sphere->radius(), 42.f);
    CORRADE_COMPARE(sphere->position(), Vector3());
}

}}}

CORRADE_TEST_MAIN(Magnum::BulletIntegration::Test::ConvertShapeTest)
