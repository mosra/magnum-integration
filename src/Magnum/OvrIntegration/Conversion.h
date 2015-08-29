#ifndef Magnum_OvrIntegration_Conversion_h
#define Magnum_OvrIntegration_Conversion_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Converters and conversion methods
 *
 * Tools for converting libOVR math and texture types to Magnum types and back.
 *
 * @author Jonathan Hale (Squareys)
 */

#include <Magnum/Magnum.h>
#include <Magnum/Texture.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Math/Vector.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/DualQuaternion.h>
#include <OVR_CAPI.h>

namespace Magnum { namespace Math { namespace Implementation {

/* ovrSizei */
template<> struct VectorConverter<2, Int, ovrSizei> {
    static Vector<2, Int> from(const ovrSizei& other) {
        return {other.w, other.h};
    }

    static ovrSizei to(const Vector<2, Int>& other) {
        return {other[0], other[1]};
    }
};

/* ovrVector2i */
template<> struct VectorConverter<2, Int, ovrVector2i> {
    static Vector<2, Int> from(const ovrVector2i& other) {
        return {other.x, other.y};
    }

    static ovrVector2i to(const Vector<2, Int>& other) {
        return {other[0], other[1]};
    }
};

/* ovrVector2f */
template<> struct VectorConverter<2, Float, ovrVector2f> {
    static Vector<2, Float> from(const ovrVector2f& other) {
        return {other.x, other.y};
    }

    static ovrVector2f to(const Vector<2, Float>& other) {
        return {other[0], other[1]};
    }
};

/* ovrVector3f */
template<> struct VectorConverter<3, Float, ovrVector3f> {
    static Vector<3, Float> from(const ovrVector3f& other) {
        return {other.x, other.y, other.z};
    }

    static ovrVector3f to(const Vector<3, Float>& other) {
        return {other[0], other[1], other[2]};
    }
};

/* ovrMatrix4 -- The oculus SDK stores matrices in row-major form. We transpose
   to get column-major form for Magnum and OpenGL. */
template<> struct RectangularMatrixConverter<4, 4, Float, ovrMatrix4f> {
    static RectangularMatrix<4, 4, Float> from(const ovrMatrix4f& other) {
        return Matrix4<Float>::from(reinterpret_cast<const Float*>(other.M)).transposed();
    }

    static ovrMatrix4f to(const RectangularMatrix<4, 4, Float>& other) {
        return *reinterpret_cast<ovrMatrix4f*>(other.transposed().data());
    }
};

/* ovrQuatf */
template<> struct QuaternionConverter<Float, ovrQuatf> {
    static Quaternion<Float> from(const ovrQuatf& other) {
        return Quaternion<Float>(Vector3<Float>{other.x, other.y, other.z}, other.w);
    }

    static ovrQuatf to(const Quaternion<Float>& other) {
        const Vector3<Float> imaginary = other.vector();
        return {imaginary.x(), imaginary.y(), imaginary.z(), other.scalar()};
    }
};

/* ovrPosef */
template<> struct DualQuaternionConverter<Float, ovrPosef> {
    static DualQuaternion<Float> from(const ovrPosef& other) {
        return DualQuaternion<Float>::translation(Vector3<Float>(other.Position)) * DualQuaternion<Float>{Quaternion<Float>(other.Orientation)};
    }

    static ovrPosef to(const DualQuaternion<Float>& other) {
        return {ovrQuatf(other.rotation()), ovrVector3f(other.translation())};
    }
};

/* ovrRecti */
template<> struct RangeConverter<2, Int, ovrRecti> {
    static Range<2, Int> from(const ovrRecti& other) {
        return Range<2, Int>::fromSize(Vector2i(other.Pos), Vector2i(other.Size));
    }

    static ovrRecti to(const Range<2, Int>& other) {
        return {ovrVector2i(other.min()), ovrSizei(other.size())};
    }
};

}}

namespace OvrIntegration {

/** @brief Wrap an `ovrTexture` as @ref Texture2D */
Texture2D wrap(const ovrTexture& texture);

}}

#endif
