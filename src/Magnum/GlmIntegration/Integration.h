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
