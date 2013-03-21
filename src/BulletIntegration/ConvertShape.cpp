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

#include "ConvertShape.h"

#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/BroadphaseCollision/btBroadphaseProxy.h>
#include <Utility/Assert.h>
#include <Physics/Box.h>
#include <Physics/Sphere.h>

#include "Integration.h"

namespace Magnum { namespace BulletIntegration {

Physics::AbstractShape3D* convertShape(const btCollisionShape* shape) {
    int type = shape->getShapeType();

    switch (type) {
        case BOX_SHAPE_PROXYTYPE:
            return convertShape(static_cast<const btBoxShape*>(shape));
            break;
        case SPHERE_SHAPE_PROXYTYPE:
            return convertShape(static_cast<const btSphereShape*>(shape));
            break;
    }
    CORRADE_INTERNAL_ASSERT(false);
}

Physics::Box3D* convertShape(const btBoxShape* box) {
    return new Physics::Box3D(Matrix4::scaling(Vector3(box->getHalfExtentsWithMargin())*2));
}

Physics::Sphere3D* convertShape(const btSphereShape* sphere) {
    return new Physics::Sphere3D({}, sphere->getRadius());
}

}}
