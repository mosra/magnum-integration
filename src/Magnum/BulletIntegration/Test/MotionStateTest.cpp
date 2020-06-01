/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
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

/* For some reason, Bullet installs the exact same header file in two places
   -- in root and in BulletDynamics/btBulletDynamicsCommon.h. The one from root
   is present in the source tree and the other isn't, so prefer it to be able
   to compile against that as well (that's what the emscripten-ports version
   is, in fact). */
#include <btBulletDynamicsCommon.h>

#include <Corrade/TestSuite/Tester.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>

#include "Magnum/BulletIntegration/Integration.h"
#include "Magnum/BulletIntegration/MotionState.h"

namespace Magnum { namespace BulletIntegration { namespace Test { namespace {

using namespace Math::Literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

struct MotionStateTest: TestSuite::Tester {
    explicit MotionStateTest();

    void test();
};

MotionStateTest::MotionStateTest() {
    addTests({&MotionStateTest::test});
}

void MotionStateTest::test() {
    /* Setup Bullet Physics */
    btDefaultCollisionConfiguration collisionConfig;
    btCollisionDispatcher dispatcher{&collisionConfig};
    btDbvtBroadphase broadphase;
    btDiscreteDynamicsWorld btWorld{&dispatcher, &broadphase, nullptr, &collisionConfig};
    btWorld.setGravity(btVector3{0.0f, 0.0f, 0.0f});

    /* Setup scene graph */
    Scene3D scene;
    Object3D object{&scene};

    Matrix4 transformation = Matrix4::translation({1.0f, 2.0f, 3.0f})*Matrix4::rotationX(45.0_degf);

    /* Setup rigid body which will receive the motion state */
    MotionState motionState{object};
    btSphereShape collisionShape{0.0f};
    btRigidBody rigidBody(1.0f, &motionState.btMotionState(), &collisionShape);
    btWorld.addRigidBody(&rigidBody);

    /* Rigid body should have no transformation initially */
    CORRADE_COMPARE(Matrix4{rigidBody.getCenterOfMassTransform()}, Matrix4{});

    /* Reset transformation of rigid body transformation */
    rigidBody.setWorldTransform(btTransform{transformation});

    /* Motion state will get updated through btWorld */
    btWorld.stepSimulation(1.0f);

    CORRADE_COMPARE(object.transformationMatrix(), transformation);
}

}}}}

CORRADE_TEST_MAIN(Magnum::BulletIntegration::Test::MotionStateTest)
