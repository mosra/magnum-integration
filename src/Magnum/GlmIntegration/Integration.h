#ifndef Magnum_GlmIntegration_Integration_h
#define Magnum_GlmIntegration_Integration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2017 sigman78 <sigman78@gmail.com>

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
@brief Conversion of basic GLM vector and matrix types

Provides conversion for the following types:

| Magnum vector type                             | Equivalent GLM type      |
| ---------------------------------------------- | ------------------------ |
| @ref Magnum::Math::BoolVector "Math::BoolVector<2>", @ref Magnum::Math::BoolVector "Math::BoolVector<3>", @ref Magnum::Math::BoolVector "Math::BoolVector<4>" | `glm::bvec2`, `glm::bvec3`, `glm::bvec4` |
| @ref Magnum::Vector2 "Vector2", @ref Magnum::Vector3 "Vector3", @ref Magnum::Color3 "Color3", @ref Magnum::Vector4 "Vector4", @ref Magnum::Color4 "Color4" | `glm::vec2`, `glm::vec3`, `glm::vec4` |
| @ref Magnum::Vector2ui "Vector2ui", @ref Magnum::Vector3ui "Vector3ui", @ref Magnum::Vector4ui "Vector4ui" | `glm::uvec2`, `glm::uvec3`, `glm::uvec4` |
| @ref Magnum::Vector2i "Vector2i", @ref Magnum::Vector3i "Vector3i", @ref Magnum::Vector4i "Vector4i" | `glm::ivec2`, `glm::ivec3`, `glm::ivec4` |
| @ref Magnum::Vector2d "Vector2d", @ref Magnum::Vector3d "Vector3d", @ref Magnum::Vector4d "Vector4d" | `glm::dvec2`, `glm::dvec3`, `glm::dvec4` |

| Magnum matrix type                             | Equivalent GLM type      |
| ---------------------------------------------- | ------------------------ |
| @ref Magnum::Matrix2x2 "Matrix2x2" or @ref Magnum::Matrix2x2d "Matrix2x2d"                                | `glm::mat2` / `glm::mat2x2` or `glm::dmat2` / `glm::dmat2x2` |
| @ref Magnum::Matrix3 "Matrix3" / @ref Magnum::Matrix3x3 "Matrix3x3" or @ref Magnum::Matrix3d "Matrix3d" / @ref Magnum::Matrix3x3d "Matrix3x3d" | `glm::mat3` / `glm::mat3x3` or `glm::dmat3` / `glm::dmat3x3` |
| @ref Magnum::Matrix4 "Matrix4" / @ref Magnum::Matrix4x4 "Matrix4x4" or @ref Magnum::Matrix4d "Matrix4d" / @ref Magnum::Matrix4x4d "Matrix4x4d" | `glm::mat4` / `glm::mat4x4` or `glm::dmat4` / `glm::dmat4x4` |
| @ref Magnum::Matrix2x3 "Matrix2x3" or @ref Magnum::Matrix2x3d "Matrix2x3d" | `glm::mat2x3` or `glm::dmat2x3` |
| @ref Magnum::Matrix3x2 "Matrix3x2" or @ref Magnum::Matrix3x2d "Matrix3x2d" | `glm::mat3x2` or `glm::dmat3x2` |
| @ref Magnum::Matrix2x4 "Matrix2x4" or @ref Magnum::Matrix2x4d "Matrix2x4d" | `glm::mat2x4` or `glm::dmat2x4` |
| @ref Magnum::Matrix4x2 "Matrix4x2" or @ref Magnum::Matrix4x2d "Matrix4x2d" | `glm::mat4x2` or `glm::dmat4x2` |
| @ref Magnum::Matrix3x4 "Matrix3x4" or @ref Magnum::Matrix3x4d "Matrix3x4d" | `glm::mat3x4` or `glm::dmat3x4` |
| @ref Magnum::Matrix4x3 "Matrix4x3" or @ref Magnum::Matrix4x3d "Matrix4x3d" | `glm::mat4x3` or `glm::dmat4x3` |

Types with extra qualifiers (such as `glm::mediump_dmat3x4`) are treated the
same as types with no qualifier. Debug output using @ref Corrade::Utility::Debug
for all types is provided as well. Example usage:

@snippet GlmIntegration.cpp Integration

See @ref Magnum/GlmIntegration/GtcIntegration.h
and @ref Magnum/GlmIntegration/GtxIntegration.h for conversion of more complex
types.
*/

#include <glm/matrix.hpp>

#include "Magnum/Math/RectangularMatrix.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum { namespace Math { namespace Implementation {

/* Bool vectors */

template<glm::qualifier q> struct BoolVectorConverter<2, glm::vec<2, bool, q>> {
    static BoolVector<2> from(const glm::vec<2, bool, q>& other) {
        return (other.x << 0)|(other.y << 1);
    }

    static glm::bvec2 to(const BoolVector<2>& other) {
        return {other[0], other[1]};
    }
};

template<glm::qualifier q> struct BoolVectorConverter<3, glm::vec<3, bool, q>> {
    static BoolVector<3> from(const glm::vec<3, bool, q>& other) {
        return (other.x << 0)|(other.y << 1)|(other.z << 2);
    }

    static glm::bvec3 to(const BoolVector<3>& other) {
        return {other[0], other[1], other[2]};
    }
};

template<glm::qualifier q> struct BoolVectorConverter<4, glm::vec<4, bool, q>> {
    static BoolVector<4> from(const glm::vec<4, bool, q>& other) {
        return (other.x << 0)|(other.y << 1)|(other.z << 2)|(other.w << 3);
    }

    static glm::bvec4 to(const BoolVector<4>& other) {
        return {other[0], other[1], other[2], other[3]};
    }
};

/* Vectors */

template<class T, glm::qualifier q> struct VectorConverter<2, T, glm::vec<2, T, q>> {
    static Vector<2, T> from(const glm::vec<2, T, q>& other) {
        return {other.x, other.y};
    }

    static glm::vec<2, T, q> to(const Vector<2, T>& other) {
        return {other[0], other[1]};
    }
};

template<class T, glm::qualifier q> struct VectorConverter<3, T, glm::vec<3, T, q>> {
    static Vector<3, T> from(const glm::vec<3, T, q>& other) {
        return {other.x, other.y, other.z};
    }

    static glm::vec<3, T, q> to(const Vector<3, T>& other) {
        return {other[0], other[1],  other[2]};
    }
};

template<class T, glm::qualifier q> struct VectorConverter<4, T, glm::vec<4, T, q>> {
    static Vector<4, T> from(const glm::vec<4, T, q>& other) {
        return {other.x, other.y, other.z, other.w};
    }

    static glm::vec<4, T, q> to(const Vector<4, T>& other) {
        return {other[0], other[1],  other[2], other[3]};
    }
};

/* Matrices */

template<std::size_t rows, class T, glm::qualifier q> struct RectangularMatrixConverter<2, rows, T, glm::mat<2, rows, T, q>> {
    static RectangularMatrix<2, rows, T> from(const glm::mat<2, rows, T, q>& other) {
        return {Vector<rows, T>(other[0]),
                Vector<rows, T>(other[1])};
    }

    static glm::mat<2, rows, T, q> to(const RectangularMatrix<2, rows, T>& other) {
        return {glm::vec<rows, T, q>(other[0]),
                glm::vec<rows, T, q>(other[1])};
    }
};

template<std::size_t rows, class T, glm::qualifier q> struct RectangularMatrixConverter<3, rows, T, glm::mat<3, rows, T, q>> {
    static RectangularMatrix<3, rows, T> from(const glm::mat<3, rows, T, q>& other) {
        return {Vector<rows, T>(other[0]),
                Vector<rows, T>(other[1]),
                Vector<rows, T>(other[2])};
    }

    static glm::mat<3, rows, T, q> to(const RectangularMatrix<3, rows, T>& other) {
        return {glm::vec<rows, T, q>(other[0]),
                glm::vec<rows, T, q>(other[1]),
                glm::vec<rows, T, q>(other[2])};
    }
};

template<std::size_t rows, class T, glm::qualifier q> struct RectangularMatrixConverter<4, rows, T, glm::mat<4, rows, T, q>> {
    static RectangularMatrix<4, rows, T> from(const glm::mat<4, rows, T, q>& other) {
        return {Vector<rows, T>(other[0]),
                Vector<rows, T>(other[1]),
                Vector<rows, T>(other[2]),
                Vector<rows, T>(other[3])};
    }

    static glm::mat<4, rows, T, q> to(const RectangularMatrix<4, rows, T>& other) {
        return {glm::vec<rows, T, q>(other[0]),
                glm::vec<rows, T, q>(other[1]),
                glm::vec<rows, T, q>(other[2]),
                glm::vec<rows, T, q>(other[3])};
    }
};

}}}
#endif

namespace glm {
    /**
     * @brief Debug output operator for GLM vector types
     *
     * Uses `glm::to_string()` internally.
     */
    template<length_t size, class T, glm::qualifier q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const vec<size, T, q>& value);

    /**
     * @brief Debug output operator for GLM matrix types
     *
     * Uses `glm::to_string()` internally.
     */
    template<length_t cols, length_t rows, class T, glm::qualifier q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const mat<cols, rows, T, q>& value);
}

#endif
