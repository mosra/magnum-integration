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
@brief Integration of basic GLM vector and matrix types
*/

#include <glm/matrix.hpp>

#include "Magnum/Math/RectangularMatrix.h"

namespace Magnum { namespace Math { namespace Implementation {

/* Float vectors */

template<> struct VectorConverter<2, Float, glm::vec2> {
    static Vector<2, Float> from(const glm::vec2& other) {
        return {other.x, other.y};
    }

    static glm::vec2 to(const Vector<2, Float>& other) {
        return {other[0], other[1]};
    }
};

template<> struct VectorConverter<3, Float, glm::vec3> {
    static Vector<3, Float> from(const glm::vec3& other) {
        return {other.x, other.y, other.z};
    }

    static glm::vec3 to(const Vector<3, Float>& other) {
        return {other[0], other[1],  other[2]};
    }
};

template<> struct VectorConverter<4, Float, glm::vec4> {
    static Vector<4, Float> from(const glm::vec4& other) {
        return {other.x, other.y, other.z, other.w};
    }

    static glm::vec4 to(const Vector<4, Float>& other) {
        return {other[0], other[1],  other[2], other[3]};
    }
};

/* Int vectors */

template<> struct VectorConverter<2, Int, glm::ivec2> {
    static Vector<2, Int> from(const glm::ivec2& other) {
        return {other.x, other.y};
    }

    static glm::ivec2 to(const Vector<2, Int>& other) {
        return {other[0], other[1]};
    }
};

template<> struct VectorConverter<3, Int, glm::ivec3> {
    static Vector<3, Int> from(const glm::ivec3& other) {
        return {other.x, other.y, other.z};
    }

    static glm::ivec3 to(const Vector<3, Int>& other) {
        return {other[0], other[1],  other[2]};
    }
};

template<> struct VectorConverter<4, Int, glm::ivec4> {
    static Vector<4, Int> from(const glm::ivec4& other) {
        return {other.x, other.y, other.z, other.w};
    }

    static glm::ivec4 to(const Vector<4, Int>& other) {
        return {other[0], other[1],  other[2], other[3]};
    }
};

/* Float matrices */

template<> struct RectangularMatrixConverter<3, 3, Float, glm::mat3> {
    static RectangularMatrix<3, 3, Float> from(const glm::mat3& other) {
        return {Vector<3, Float>(other[0]),
                Vector<3, Float>(other[1]),
                Vector<3, Float>(other[2])};
    }

    static glm::mat3 to(const RectangularMatrix<3, 3, Float>& other) {
        return {other[0][0], other[0][1], other[0][2],
                other[1][0], other[1][1], other[1][2],
                other[2][0], other[2][1], other[2][2]};
    }
};

template<> struct RectangularMatrixConverter<4, 4, Float, glm::mat4> {
    static RectangularMatrix<4, 4, Float> from(const glm::mat4& other) {
        return {Vector<4, Float>(other[0]),
                Vector<4, Float>(other[1]),
                Vector<4, Float>(other[2]),
                Vector<4, Float>(other[3])};
    }

    static glm::mat4 to(const RectangularMatrix<4, 4, Float>& other) {
        return {other[0][0], other[0][1], other[0][2], other[0][3],
                other[1][0], other[1][1], other[1][2], other[1][3],
                other[2][0], other[2][1], other[2][2], other[2][3],
                other[3][0], other[3][1], other[3][2], other[3][3]};
    }
};

}}}

#endif
