/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Integration.h"
#include "GtcIntegration.h"

#define GLM_ENABLE_EXPERIMENTAL /* WTF, GLM!! Yes, I'm going to repeat this every time. */
#include "GtxIntegration.h"

#include "Magnum/GlmIntegration/visibility.h"

#include <glm/gtx/string_cast.hpp>

namespace glm {

template<length_t size, class T, glm::qualifier q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const vec<size, T, q>& value) {
    return debug << to_string(value);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, bool, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, bool, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, bool, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, unsigned int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, unsigned int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, unsigned int, highp>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, bool, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, bool, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, bool, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, unsigned int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, unsigned int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, unsigned int, mediump>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, bool, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, bool, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, bool, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<2, unsigned int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<3, unsigned int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const vec<4, unsigned int, lowp>&);
#endif

template<length_t cols, length_t rows, class T, glm::qualifier q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const mat<cols, rows, T, q>& value) {
    return debug << to_string(value);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 2, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 3, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 4, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 2, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 3, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 4, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 2, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 3, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 4, float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 2, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 3, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 4, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 2, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 3, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 4, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 2, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 3, double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 4, double, highp>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 2, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 3, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 4, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 2, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 3, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 4, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 2, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 3, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 4, float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 2, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 3, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 4, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 2, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 3, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 4, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 2, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 3, double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 4, double, mediump>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 2, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 3, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 4, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 2, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 3, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 4, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 2, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 3, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 4, float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 2, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 3, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<2, 4, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 2, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 3, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<3, 4, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 2, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 3, double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const mat<4, 4, double, lowp>&);
#endif

template<class T, glm::qualifier q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tquat<T, q>& value) {
    return debug << to_string(value);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<double, highp>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<double, mediump>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<double, lowp>&);
#endif

template<class T, glm::qualifier q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tdualquat<T, q>& value) {
    return debug << to_string(value);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<double, highp>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<double, mediump>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<double, lowp>&);
#endif

}
