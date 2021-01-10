#ifndef Magnum_OvrIntegration_Integration_h
#define Magnum_OvrIntegration_Integration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015, 2016 Jonathan Hale <squareys@googlemail.com>

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
@brief Conversion of libOVR math types

Provides conversion for the following types:

| Magnum type                           | Equivalent libOVR type            |
| ------------------------------------- | --------------------------------- |
| @ref Magnum::Vector2i "Vector2i"      | @cpp ovrVector2i @ce, @cpp ovrSizei @ce |
| @ref Magnum::Vector2 "Vector2"        | @cpp ovrVector2f @ce <b></b>      |
| @ref Magnum::Vector3 "Vector3"        | @cpp ovrVector3f @ce <b></b>      |
| @ref Magnum::Matrix4 "Matrix4"        | @cpp ovrMatrix4f @ce <b></b>      |
| @ref Magnum::Quaternion "Quaternion"  | @cpp ovrQuatf @ce <b></b>         |
| @ref Magnum::DualQuaternion "DualQuaternion" | @cpp ovrPosef @ce <b></b>  |
| @ref Magnum::Range2Di "Range2Di"      | @cpp ovrRecti @ce <b></b>         |

Unlike Magnum, OVR SDK stores matrices in row-major order, so these get
transposed during the conversion.

Example usage:

@snippet OvrIntegration.cpp Integration

@see @ref types-thirdparty-integration
*/

#include <Magnum/Math/Range.h>
#include <Magnum/Math/Vector.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/DualQuaternion.h>
#include <OVR_CAPI.h>

/* Don't list (useless) Magnum and Math namespaces without anything else */
#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum { namespace Math { namespace Implementation {

template<> struct VectorConverter<2, Int, ovrSizei> {
    static Vector<2, Int> from(const ovrSizei& other) {
        return {other.w, other.h};
    }

    static ovrSizei to(const Vector<2, Int>& other) {
        return {other[0], other[1]};
    }
};

template<> struct VectorConverter<2, Int, ovrVector2i> {
    static Vector<2, Int> from(const ovrVector2i& other) {
        return {other.x, other.y};
    }

    static ovrVector2i to(const Vector<2, Int>& other) {
        return {other[0], other[1]};
    }
};

template<> struct VectorConverter<2, Float, ovrVector2f> {
    static Vector<2, Float> from(const ovrVector2f& other) {
        return {other.x, other.y};
    }

    static ovrVector2f to(const Vector<2, Float>& other) {
        return {other[0], other[1]};
    }
};

template<> struct VectorConverter<3, Float, ovrVector3f> {
    static Vector<3, Float> from(const ovrVector3f& other) {
        return {other.x, other.y, other.z};
    }

    static ovrVector3f to(const Vector<3, Float>& other) {
        return {other[0], other[1], other[2]};
    }
};

template<> struct RectangularMatrixConverter<4, 4, Float, ovrMatrix4f> {
    static RectangularMatrix<4, 4, Float> from(const ovrMatrix4f& other) {
        return Matrix4<Float>::from(reinterpret_cast<const Float*>(other.M)).transposed();
    }

    static ovrMatrix4f to(const RectangularMatrix<4, 4, Float>& other) {
        return *reinterpret_cast<ovrMatrix4f*>(other.transposed().data());
    }
};

template<> struct QuaternionConverter<Float, ovrQuatf> {
    static Quaternion<Float> from(const ovrQuatf& other) {
        return Quaternion<Float>(Vector3<Float>{other.x, other.y, other.z}, other.w);
    }

    static ovrQuatf to(const Quaternion<Float>& other) {
        const Vector3<Float> imaginary = other.vector();
        return {imaginary.x(), imaginary.y(), imaginary.z(), other.scalar()};
    }
};

template<> struct DualQuaternionConverter<Float, ovrPosef> {
    static DualQuaternion<Float> from(const ovrPosef& other) {
        return DualQuaternion<Float>::translation(Vector3<Float>(other.Position)) * DualQuaternion<Float>{Quaternion<Float>(other.Orientation)};
    }

    static ovrPosef to(const DualQuaternion<Float>& other) {
        return {ovrQuatf(other.rotation()), ovrVector3f(other.translation())};
    }
};

template<> struct RangeConverter<2, Int, ovrRecti> {
    static Range<2, Int> from(const ovrRecti& other) {
        return Range<2, Int>::fromSize(Vector2i(other.Pos), Vector2i(other.Size));
    }

    static ovrRecti to(const Range<2, Int>& other) {
        return {ovrVector2i(other.min()), ovrSizei(other.size())};
    }
};

}}}
#endif

#endif
