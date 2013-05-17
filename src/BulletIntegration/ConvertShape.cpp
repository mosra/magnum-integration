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
#include <Shapes/Box.h>
#include <Shapes/Shape.h>
#include <Shapes/Sphere.h>

#include "Integration.h"

namespace Magnum { namespace BulletIntegration {

Shapes::AbstractShape3D* convertShape(SceneGraph::AbstractObject3D<btScalar>* object, const btCollisionShape* shape, Shapes::ShapeGroup3D* shapes) {
    int type = shape->getShapeType();

    switch (type) {
        case BOX_SHAPE_PROXYTYPE:
            return convertShape(object, static_cast<const btBoxShape*>(shape), shapes);
            break;
        case SPHERE_SHAPE_PROXYTYPE:
            return convertShape(object, static_cast<const btSphereShape*>(shape), shapes);
            break;
    }

    Error() << "BulletIntegration::convertShape(): shape type" << type << "is not supported";
    return nullptr;
}

Shapes::Shape<Shapes::Box3D>* convertShape(SceneGraph::AbstractObject3D<btScalar>* object, const btBoxShape* box, Shapes::ShapeGroup3D* shapes) {
    return new Shapes::Shape<Shapes::Box3D>(object, Matrix4::scaling(Vector3(box->getHalfExtentsWithMargin())), shapes);
}

Shapes::Shape<Shapes::Sphere3D>* convertShape(SceneGraph::AbstractObject3D<btScalar>* object, const btSphereShape* sphere, Shapes::ShapeGroup3D* shapes) {
    return new Shapes::Shape<Shapes::Sphere3D>(object, {{}, sphere->getRadius()}, shapes);
}

}}
