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

#include <btBulletDynamicsCommon.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include "Magnum/BulletIntegration/DebugDraw.h"
#include "Magnum/BulletIntegration/MotionState.h"

using namespace Magnum;

int main() {
#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
{
Matrix4 projection, transformation;
/* [DebugDraw-usage] */
btDynamicsWorld* btWorld;
BulletIntegration::DebugDraw debugDraw;

debugDraw.setMode(BulletIntegration::DebugDraw::Mode::DrawWireframe|
                  BulletIntegration::DebugDraw::Mode::DrawConstraints);
btWorld->setDebugDrawer(&debugDraw);
/* [DebugDraw-usage] */

/* [DebugDraw-usage-per-frame] */
debugDraw.setTransformationProjectionMatrix(projection*transformation);
btWorld->debugDrawWorld();
/* [DebugDraw-usage-per-frame] */
}
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#ifndef BT_USE_DOUBLE_PRECISION
{
/* [Integration] */
btVector3 a{20.0f, 50.0f, -1.0f};
Vector3 b(a);

using namespace Math::Literals;
auto c = btQuaternion(Quaternion::rotation(15.0_degf, Vector3::xAxis()));
/* [Integration] */
static_cast<void>(b);
static_cast<void>(c);
}
#endif

#ifndef BT_USE_DOUBLE_PRECISION
{
#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
/* [MotionState-usage] */
btDynamicsWorld* btWorld;
SceneGraph::Object<SceneGraph::MatrixTransformation3D> object;

auto motionState = new BulletIntegration::MotionState{object};
auto collisionShape = new btBoxShape{{0.5f, 0.5f, 0.5f}};
auto rigidBody = new btRigidBody{20.0f, &motionState->btMotionState(), collisionShape};
btWorld->addRigidBody(rigidBody);
/* [MotionState-usage] */

/* [MotionState-kinematic] */
rigidBody->setCollisionFlags(rigidBody->getCollisionFlags()|
    btCollisionObject::CF_KINEMATIC_OBJECT);
/* [MotionState-kinematic] */

/* [MotionState-update] */
rigidBody->setWorldTransform(btTransform(object.transformationMatrix()));
/* [MotionState-update] */

/* [MotionState-usage-after] */
rigidBody->setMotionState(&motionState->btMotionState());
/* [MotionState-usage-after] */
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
}
#endif
}
