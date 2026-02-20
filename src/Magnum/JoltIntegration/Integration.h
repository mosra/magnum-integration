#ifndef Magnum_JoltIntegration_h
#define Magnum_JoltIntegration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2026 Igal Alkon <igal@alkontek.com>

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

/**
 * @file
 * @brief Conversion of Jolt math types
*/

#include <Magnum/Math/RectangularMatrix.h>
#include <Magnum/Math/Quaternion.h>

/* Jolt requires <Jolt/Jolt.h> BEFORE any other Jolt header.
We include the full definitions here because the converters return Jolt types by value. */
#include <Jolt/Jolt.h>

#ifndef DOXYGEN_GENERATING_OUTPUT

namespace Magnum { namespace Math { namespace Implementation {

    template<> struct VectorConverter<3, Float, JPH::Vec3> {
        static Vector<3, Float> from(const JPH::Vec3& other) {
            return {other.GetX(), other.GetY(), other.GetZ()};
        }

        static JPH::Vec3 to(const Vector<3, Float>& other) {
            return {other[0], other[1], other[2]};
        }
    };

    template<> struct RectangularMatrixConverter<4, 4, Float, JPH::Mat44> {
        static RectangularMatrix<4, 4, Float> from(const JPH::Mat44& other) {
            RectangularMatrix<4, 4, Float> result;
            for (int row = 0; row < 4; ++row)
                for (int col = 0; col < 4; ++col)
                    result[col][row] = other(row, col);
            return result;
        }

        static JPH::Mat44 to(const RectangularMatrix<4, 4, Float>& other) {
            JPH::Mat44 result{};
            for (int row = 0; row < 4; ++row)
                for (int col = 0; col < 4; ++col)
                    result(row, col) = other[col][row];
            return result;
        }
    };

    template<> struct QuaternionConverter<Float, JPH::Quat> {
        static Quaternion<Float> from(const JPH::Quat& other) {
            return {{other.GetX(), other.GetY(), other.GetZ()}, other.GetW()};
        }

        static JPH::Quat to(const Quaternion<Float>& other) {
            return {other.vector().x(), other.vector().y(), other.vector().z(), other.scalar()};
        }
    };

}}}
#endif

#endif //Magnum_JoltIntegration_h
