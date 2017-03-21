#ifndef Magnum_BulletIntegration_h
#define Magnum_BulletIntegration_h
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

/** @file
 * @brief Integration of Bullet math classes
 */

#include <LinearMath/btMatrix3x3.h>
#include <Magnum/Math/RectangularMatrix.h>

#include "Magnum/BulletIntegration/visibility.h"

namespace Magnum { namespace Math { namespace Implementation {

template<> struct VectorConverter<3, btScalar, btVector3> {
    static Vector<3, Float> from(const btVector3& other) {
        return {other.x(), other.y(), other.z()};
    }

    static btVector3 to(const Vector<3, Float>& other) {
        return {other[0], other[1], other[2]};
    }
};

template<> struct RectangularMatrixConverter<3, 3, btScalar, btMatrix3x3> {
    static RectangularMatrix<3, 3, Float> from(const btMatrix3x3& other) {
        return {Vector<3, Float>(other[0]),
                Vector<3, Float>(other[1]),
                Vector<3, Float>(other[2])};
    }

    static btMatrix3x3 to(const RectangularMatrix<3, 3, Float>& other) {
        return {other[0][0], other[0][1], other[0][2],
                other[1][0], other[1][1], other[1][2],
                other[2][0], other[2][1], other[2][2]};
    }
};

}}}

#endif
