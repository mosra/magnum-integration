/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "MotionState.h"

#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Matrix4.h>

#include "Magnum/BulletIntegration/Integration.h"


namespace Magnum { namespace BulletIntegration {

using namespace Math::Literals;

void MotionState::getWorldTransform(btTransform& worldTrans) const {
    /* Bullet transforms are always world space */
    worldTrans = btTransform{object().absoluteTransformationMatrix()};
}

void MotionState::setWorldTransform(const btTransform& worldTrans) {
    auto parent = object().parent();
    if(parent) {
        /* Get local transform Bullet world transform by removing parent transform */
        const Matrix4 parentTransform = parent->absoluteTransformationMatrix();

        CORRADE_ASSERT(parentTransform.isRigidTransformation(), "MotionState::setWorldTransform(): parent of object with MotionState cannot have scaling", );
        const Quaternion parentRotation = Quaternion::fromMatrix(parentTransform.rotationScaling());

        const Quaternion rotation = (parentRotation.inverted()*Quaternion{worldTrans.getRotation()})
                                    .normalized(); /* Avoid floating point drift */
        const Vector3 translation = Vector3{worldTrans.getOrigin()} - parentTransform.translation();

        /* Apply calculated transformation to object */
        _transformation.resetTransformation();

        const Rad angle = rotation.angle();
        if(angle != 0.0_radf) {
            /* If angle is 0, the axis would be a NaN vector */
            _transformation.rotate(angle, rotation.axis());
        }

        _transformation.translate(translation);
    } else {
        const Quaternion rotation = Quaternion{worldTrans.getRotation()};
        _transformation.resetTransformation()
                       .rotate(rotation.angle(), rotation.axis())
                       .translate(Vector3{worldTrans.getOrigin()});
    }
}

}}
