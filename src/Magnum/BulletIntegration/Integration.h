#ifndef Magnum_BulletIntegration_h
#define Magnum_BulletIntegration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2013 Jan Dupal <dupal.j@gmail.com>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 Max Schwarz <max.schwarz@ais.uni-bonn.de>

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
@brief Conversion of Bullet math types

Provides conversion for the following types:

| Magnum type                           | Equivalent Bullet type        |
| ------------------------------------- | ----------------------------- |
| @ref Magnum::Vector3 "Vector3"        | @m_class{m-doc-external} [btVector3](https://pybullet.org/Bullet/BulletFull/classbtVector3.html) |
| @ref Magnum::Matrix3x3 "Matrix3x3"    | @m_class{m-doc-external} [btMatrix3x3](https://pybullet.org/Bullet/BulletFull/classbtMatrix3x3.html) |
| @ref Magnum::Matrix4 "Matrix4"        | @m_class{m-doc-external} [btTransform](https://pybullet.org/Bullet/BulletFull/classbtTransform.html) |
| @ref Magnum::Quaternion "Quaternion"  | @m_class{m-doc-external} [btQuaternion](https://pybullet.org/Bullet/BulletFull/classbtQuaternion.html) |

All type conversion is done with @m_class{m-doc-external} [btScalar](https://pybullet.org/Bullet/BulletFull/btScalar_8h.html#a1e5824cfc8adbf5a77f2622132d16018)
as the underlying type. It's typically a @cpp float @ce, but Bullet can be
compiled to use @cpp double @ce instead. In that case, the above types are then
@ref Magnum::Vector3d "Vector3d", @ref Magnum::Quaterniond "Quaterniond" etc.

Unlike Magnum, Bullet stores matrices in row-major order, so these get
transposed during the conversion.

Example usage:

@snippet BulletIntegration.cpp Integration

@see @ref types-thirdparty-integration
*/

#include <LinearMath/btMatrix3x3.h>
#include <LinearMath/btTransform.h>
#include <Magnum/Math/RectangularMatrix.h>
#include <Magnum/Math/Quaternion.h>

#include "Magnum/BulletIntegration/visibility.h"

/* Don't list (useless) Magnum and Math namespaces without anything else */
#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum { namespace Math { namespace Implementation {

template<> struct VectorConverter<3, btScalar, btVector3> {
    static Vector<3, btScalar> from(const btVector3& other) {
        return {other.x(), other.y(), other.z()};
    }

    static btVector3 to(const Vector<3, btScalar>& other) {
        return {other[0], other[1], other[2]};
    }
};

template<> struct RectangularMatrixConverter<3, 3, btScalar, btMatrix3x3> {
    static RectangularMatrix<3, 3, btScalar> from(const btMatrix3x3& other) {
        return {Vector<3, btScalar>(other.getColumn(0)),
                Vector<3, btScalar>(other.getColumn(1)),
                Vector<3, btScalar>(other.getColumn(2))};
    }

    static btMatrix3x3 to(const RectangularMatrix<3, 3, btScalar>& other) {
        return {other[0][0], other[1][0], other[2][0],
                other[0][1], other[1][1], other[2][1],
                other[0][2], other[1][2], other[2][2]};
    }
};

template<> struct RectangularMatrixConverter<4, 4, btScalar, btTransform> {
    static RectangularMatrix<4, 4, btScalar> from(const btTransform& other) {
        RectangularMatrix<4, 4, btScalar> mat;
        other.getOpenGLMatrix(mat.data());
        return mat;
    }

    static btTransform to(const RectangularMatrix<4, 4, btScalar>& other) {
        btTransform transform;
        transform.setFromOpenGLMatrix(other.data());
        return transform;
    }
};

template<> struct QuaternionConverter<btScalar, btQuaternion> {
    static Quaternion<btScalar> from(const btQuaternion& other) {
        return {{other.x(), other.y(), other.z()}, other.w()};
    }

    static btQuaternion to(const Quaternion<btScalar>& other) {
        return {other.vector().x(), other.vector().y(), other.vector().z(), other.scalar()};
    }
};

}}}
#endif

#endif
