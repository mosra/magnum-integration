#ifndef Magnum_BulletIntegration_ConvertShape_h
#define Magnum_BulletIntegration_ConvertShape_h
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

/** @file
 * @brief Function @ref Magnum::BulletIntegration::convertShape()
 */

#include <Magnum/Shapes/Shapes.h>
#include <Magnum/SceneGraph/SceneGraph.h>
#include <LinearMath/btScalar.h>

#include "Magnum/BulletIntegration/visibility.h"

class btCollisionShape;
class btBoxShape;
class btSphereShape;

namespace Magnum { namespace BulletIntegration {

/**
@brief Convert generic Bullet collision shape to Magnum shape
@param object   Object the shape belongs to
@param shape    Shape to convert
@param shapes   Optional shape group to add the shape to

Returns @cpp nullptr @ce if given shape is not supported.
*/
Shapes::AbstractShape3D MAGNUM_BULLETINTEGRATION_EXPORT * convertShape(SceneGraph::AbstractBasicObject3D<btScalar>& object, const btCollisionShape& shape, Shapes::ShapeGroup3D* shapes = nullptr);

/**
@brief Convert box Bullet collision shape to Magnum shape

See @ref convertShape(SceneGraph::AbstractBasicObject3D<btScalar>&, const btCollisionShape&, Shapes::ShapeGroup3D*)
for more information.
*/
Shapes::Shape<Shapes::Box3D> MAGNUM_BULLETINTEGRATION_EXPORT * convertShape(SceneGraph::AbstractBasicObject3D<btScalar>& object, const btBoxShape& box, Shapes::ShapeGroup3D* shapes = nullptr);

/**
@brief Convert sphere Bullet collision shape to Magnum shape

See @ref convertShape(SceneGraph::AbstractBasicObject3D<btScalar>&, const btCollisionShape&, Shapes::ShapeGroup3D*)
for more information.
*/
Shapes::Shape<Shapes::Sphere3D> MAGNUM_BULLETINTEGRATION_EXPORT * convertShape(SceneGraph::AbstractBasicObject3D<btScalar>& object, const btSphereShape& sphere, Shapes::ShapeGroup3D* shapes = nullptr);

}}

#endif
