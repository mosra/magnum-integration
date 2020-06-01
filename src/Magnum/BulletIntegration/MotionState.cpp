/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
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

#include "MotionState.h"

#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Functions.h>

#include "Magnum/BulletIntegration/Integration.h"

namespace Magnum { namespace BulletIntegration {

/* The original btMotionState is not dllexported on Windows, so the constructor
   and destructor of this class have to be non-inline in order to avoid the
   need for having btMotionState constructor exported */

/* Doxygen gets confused when using {} to initialize parent object */
MotionState::MotionState(SceneGraph::AbstractBasicObject3D<btScalar>& object, SceneGraph::AbstractBasicTranslationRotation3D<btScalar>& transformation): SceneGraph::AbstractBasicFeature3D<btScalar>(object), _transformation(transformation) {}

MotionState::~MotionState() = default;

void MotionState::getWorldTransform(btTransform& worldTrans) const {
    const Matrix4 transformation = object().transformationMatrix();
    worldTrans.setOrigin(btVector3(transformation.translation()));
    worldTrans.setBasis(btMatrix3x3(transformation.rotationScaling()));
}

void MotionState::setWorldTransform(const btTransform& worldTrans) {
    const Vector3 position = Vector3{worldTrans.getOrigin()};
    const Vector3 axis = Vector3{worldTrans.getRotation().getAxis()};
    const Float rotation = worldTrans.getRotation().getAngle();

    /* Bullet sometimes reports NaNs for all the parameters and nobody is sure
       why: https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=12080. The body
       gets stuck in that state, so print the warning just once. */
    if(Math::isNan(position).any() || Math::isNan(axis).any() || Math::isNan(rotation)) {
        if(!_broken) {
            Warning{} << "BulletIntegration::MotionState: Bullet reported NaN transform for" << this << Debug::nospace << ", ignoring";
            _broken = true;
        }
        return;
    }

    /** @todo Verify that all objects have common parent */
    _transformation.resetTransformation()
        .rotate(Rad{rotation}, axis.normalized())
        .translate(position);
}

}}
