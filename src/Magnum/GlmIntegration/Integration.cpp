/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Utility/DebugStl.h>

#include <glm/fwd.hpp>
#if GLM_VERSION < 96
#define GLM_FORCE_RADIANS /* Otherwise 0.9.5 spits a lot of loud messages :/ */
#endif
#include "Magnum/GlmIntegration/GtcIntegration.h"

#if GLM_VERSION >= 990
#define GLM_ENABLE_EXPERIMENTAL /* WTF, GLM!! Yes, I'm going to repeat this every time. */
#endif
#include "Magnum/GlmIntegration/GtxIntegration.h"

#include "Magnum/GlmIntegration/visibility.h"

#include <glm/gtx/string_cast.hpp>

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace glm {
#if GLM_VERSION < 96
/* All types were in glm::detail in 0.9.5, wrap the following as well in order
   to make ADL work properly. */
namespace detail {
#endif

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tvec2<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tvec3<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tvec4<T, q>& value) {
    return debug << to_string(value);
}

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<bool, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<bool, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<bool, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<unsigned int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<unsigned int, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<unsigned int, highp>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<bool, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<bool, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<bool, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<unsigned int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<unsigned int, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<unsigned int, mediump>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<bool, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<bool, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<bool, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec2<unsigned int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec3<unsigned int, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tvec4<unsigned int, lowp>&);

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tmat2x2<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tmat2x3<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tmat2x4<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tmat3x2<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tmat3x3<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tmat3x4<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tmat4x2<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tmat4x3<T, q>& value) {
    return debug << to_string(value);
}

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tmat4x4<T, q>& value) {
    return debug << to_string(value);
}

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x2<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x3<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x4<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x2<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x3<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x4<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x2<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x3<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x4<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x2<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x3<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x4<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x2<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x3<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x4<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x2<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x3<double, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x4<double, highp>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x2<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x3<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x4<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x2<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x3<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x4<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x2<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x3<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x4<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x2<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x3<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x4<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x2<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x3<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x4<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x2<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x3<double, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x4<double, mediump>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x2<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x3<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x4<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x2<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x3<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x4<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x2<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x3<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x4<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x2<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x3<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat2x4<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x2<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x3<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat3x4<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x2<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x3<double, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tmat4x4<double, lowp>&);

#if GLM_VERSION >= 97
template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tquat<T, q>& value) {
    return debug << to_string(value);
}

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<double, highp>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<double, mediump>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tquat<double, lowp>&);

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tdualquat<T, q>& value) {
    return debug << to_string(value);
}

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<float, highp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<double, highp>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<float, mediump>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<double, mediump>&);

template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<float, lowp>&);
template MAGNUM_GLMINTEGRATION_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const tdualquat<double, lowp>&);
#endif

#if GLM_VERSION < 96
} /* Close the detail namespace */
#endif
}
#endif
