/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
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
#include <Corrade/TestSuite/Tester.h>
#include <Magnum/Shapes/Box.h>
#include <Magnum/Shapes/Sphere.h>
#include <Magnum/Shapes/Shape.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include "Magnum/BulletIntegration/ConvertShape.h"
#include "Magnum/BulletIntegration/MotionState.h"

namespace Magnum { namespace BulletIntegration { namespace Test {

typedef SceneGraph::Object<SceneGraph::BasicMatrixTransformation3D<btScalar>> Object3D;

struct ConvertShapeTest: TestSuite::Tester {
    explicit ConvertShapeTest();

    void box();
    void sphere();
};

ConvertShapeTest::ConvertShapeTest() {
    addTests({&ConvertShapeTest::box,
              &ConvertShapeTest::sphere});
}

void ConvertShapeTest::box() {
    Object3D object;
    btBoxShape btBox({0.5f, 1.0f, 1.5f});
    Shapes::Shape<Shapes::Box3D>* box = convertShape(object, btBox);
    CORRADE_COMPARE(box->shape().transformation(), Matrix4::scaling({0.5f, 1.0f, 1.5f}));
}

void ConvertShapeTest::sphere() {
    Object3D object;
    btSphereShape btSphereShape(42.f);
    Shapes::Shape<Shapes::Sphere3D>* sphere = convertShape(object, btSphereShape);
    CORRADE_COMPARE(sphere->shape().radius(), 42.f);
    CORRADE_COMPARE(sphere->shape().position(), Vector3());
}

}}}

CORRADE_TEST_MAIN(Magnum::BulletIntegration::Test::ConvertShapeTest)
