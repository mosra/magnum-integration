#ifndef Magnum_EigenIntegration_GeometryIntegration_h
#define Magnum_EigenIntegration_GeometryIntegration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2012, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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
@brief Conversion of Eigen geometry types
@m_since_{integration,2019,10}

Provides conversion for the following types. See
@ref Magnum/EigenIntegration/Integration.h for conversion of basic matrix and
vector types.

| Magnum type                           | Equivalent Eigen type             |
| ------------------------------------- | --------------------------------- |
| @ref Magnum::Math::Vector "Math::Vector<size, T>" | @m_class{m-doc-external} [Eigen::Translation<T, size>](https://eigen.tuxfamily.org/dox/classEigen_1_1Translation.html) |
| @ref Magnum::Math::Quaternion "Math::Quaternion<T>" | @m_class{m-doc-external} [Eigen::Quaternion<T>](https://eigen.tuxfamily.org/dox/classEigen_1_1Quaternion.html) |
| @ref Magnum::Math::Matrix3x2<T> "Math::Matrix3x2<T>" | @m_class{m-doc-external} [Eigen::Transform<T, 2, AffineCompact>](https://eigen.tuxfamily.org/dox/classEigen_1_1Transform.html) |
| @ref Magnum::Math::Matrix3<T> "Math::Matrix3<T>" | @m_class{m-doc-external} [Eigen::Transform<T, 2, Affine>](https://eigen.tuxfamily.org/dox/classEigen_1_1Transform.html), \n @m_class{m-doc-external} [Eigen::Transform<T, 2, Projective>](https://eigen.tuxfamily.org/dox/classEigen_1_1Transform.html), \n @m_class{m-doc-external} [Eigen::Transform<T, 2, Isometry>](https://eigen.tuxfamily.org/dox/classEigen_1_1Transform.html) |
| @ref Magnum::Math::Matrix4x3<T> "Math::Matrix4x3<T>" | @m_class{m-doc-external} [Eigen::Transform<T, 3, AffineCompact>](https://eigen.tuxfamily.org/dox/classEigen_1_1Transform.html) |
| @ref Magnum::Math::Matrix4<T> "Math::Matrix4<T>" | @m_class{m-doc-external} [Eigen::Transform<T, 3, Affine>](https://eigen.tuxfamily.org/dox/classEigen_1_1Transform.html), \n @m_class{m-doc-external} [Eigen::Transform<T, 3, Projective>](https://eigen.tuxfamily.org/dox/classEigen_1_1Transform.html), \n @m_class{m-doc-external} [Eigen::Transform<T, 3, Isometry>](https://eigen.tuxfamily.org/dox/classEigen_1_1Transform.html) |

Example usage:

@snippet EigenIntegration.cpp GeometryIntegration

@see @ref types-thirdparty-integration

*/

#include <Eigen/Geometry>
#include <Magnum/Math/Quaternion.h>

#include "Magnum/EigenIntegration/Integration.h"

namespace Magnum { namespace Math { namespace Implementation {

template<std::size_t size, class T> struct VectorConverter<size, T, Eigen::Translation<T, int(size)>> {
    static Vector<size, T> from(const Eigen::Translation<T, size>& other) {
        return Vector<size, T>(other.vector());
    }

    static Eigen::Translation<T, size> to(const Vector<size, T>& other) {
        /* Can't use explicit conversion because of the all-catching implicit
           constructor of Eigen::Matrix */
        return Eigen::Translation<T, size>(VectorConverter<size, T, Eigen::Matrix<T, size, 1>>::to(other));
    }
};

template<std::size_t size, class T, int mode> struct RectangularMatrixConverter<size, size, T, Eigen::Transform<T, int(size - 1), mode>> {
    static_assert(mode == Eigen::Affine || mode == Eigen::Projective || mode == Eigen::Isometry,
        "only Affine, Projective and Isometry transform supported");

    static RectangularMatrix<size, size, T> from(const Eigen::Transform<T, int(size - 1), mode>& other) {
        return RectangularMatrix<size, size, T>(other.matrix());
    }

    static Eigen::Transform<T, int(size - 1), mode> to(const RectangularMatrix<size, size, T>& other) {
        /* Can't use explicit conversion because of the all-catching implicit
           constructor of Eigen::Matrix */
        return Eigen::Transform<T, int(size - 1), mode>(RectangularMatrixConverter<size, size, T, Eigen::Matrix<T, size, size>>::to(other));
    }
};

/* Can't have templated on size like RectangularMatrixConverter<size + 1, size,
   T, Eigen::Transform<T, int(size), Eigen::AffineCompact>> because error:
   template argument ‘(size + 1)’ involves template parameter(s). ¯\_(ツ)_/¯
   So duplicating for 2D and 3D here. */
template<class T> struct RectangularMatrixConverter<3, 2, T, Eigen::Transform<T, 2, Eigen::AffineCompact>> {
    static RectangularMatrix<3, 2, T> from(const Eigen::Transform<T, 2, Eigen::AffineCompact>& other) {
        return RectangularMatrix<3, 2, T>(other.matrix());
    }

    static Eigen::Transform<T, 2, Eigen::AffineCompact> to(const RectangularMatrix<3, 2, T>& other) {
        /* Can't use explicit conversion because of the all-catching implicit
           constructor of Eigen::Matrix */
        return Eigen::Transform<T, 2, Eigen::AffineCompact>(RectangularMatrixConverter<3, 2, T, Eigen::Matrix<T, 2, 3>>::to(other));
    }
};
template<class T> struct RectangularMatrixConverter<4, 3, T, Eigen::Transform<T, 3, Eigen::AffineCompact>> {
    static RectangularMatrix<4, 3, T> from(const Eigen::Transform<T, 3, Eigen::AffineCompact>& other) {
        return RectangularMatrix<4, 3, T>(other.matrix());
    }

    static Eigen::Transform<T, 3, Eigen::AffineCompact> to(const RectangularMatrix<4, 3, T>& other) {
        /* Can't use explicit conversion because of the all-catching implicit
           constructor of Eigen::Matrix */
        return Eigen::Transform<T, 3, Eigen::AffineCompact>(RectangularMatrixConverter<4, 3, T, Eigen::Matrix<T, 3, 4>>::to(other));
    }
};

template<class T> struct QuaternionConverter<T, Eigen::Quaternion<T>> {
    static Quaternion<T> from(const Eigen::Quaternion<T>& other) {
        return {{other.x(), other.y(), other.z()}, other.w()};
    }

    static Eigen::Quaternion<T> to(const Quaternion<T>& other) {
        /* Eigen's quaternion constructor is ordered w x y z */
        return {other.scalar(), other.vector().x(), other.vector().y(), other.vector().z()};
    }
};

}}


namespace EigenIntegration {

/**
@brief Convert a Magnum type to Eigen type
@m_since{2019,10}

Provided only for consistency with
@ref cast(const Math::RectangularMatrix<cols, rows, T>&),
@ref cast(const Math::BoolVector<size>&) and
@ref cast(const Math::Vector<size, T>&) --- conversion from
@ref Math::Quaternion to @m_class{m-doc-external} [Eigen::Quaternion<T>](https://eigen.tuxfamily.org/dox/classEigen_1_1Quaternion.html)
can be done directly via an explicit conversion, unlike with matrices and
vectors there's no need for a helper function.
*/
template<class To, class T> inline To cast(const Math::Quaternion<T>& from) {
    return To(from);
}

}

}

#endif
