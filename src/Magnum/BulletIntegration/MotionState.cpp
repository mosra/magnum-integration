/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include <Magnum/Math/Matrix4.h>

#include "Magnum/BulletIntegration/Integration.h"

namespace Magnum { namespace BulletIntegration {

void MotionState::getWorldTransform(btTransform& worldTrans) const {
    const Matrix4 transformation = object().transformationMatrix();
    worldTrans.setOrigin(btVector3(transformation.translation()));
    worldTrans.setBasis(btMatrix3x3(transformation.rotationScaling()));
}

void MotionState::setWorldTransform(const btTransform& worldTrans) {
    btVector3 bPosition = worldTrans.getOrigin();
    btVector3 bAxis = worldTrans.getRotation().getAxis();
    Rad bRotation(worldTrans.getRotation().getAngle());

    /** @todo Verify that all objects have common parent */
    transformation.resetTransformation()
        .rotate(bRotation, Vector3(bAxis).normalized())
        .translate(Vector3(bPosition));
}

}}
