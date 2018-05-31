#ifndef Magnum_GlmIntegration_GtcIntegration_h
#define Magnum_GlmIntegration_GtcIntegration_h
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
@brief Integration of GLM extension types

Provides conversion for the following types:

| Magnum type                           | Equivalent GLM type               |
| ------------------------------------- | --------------------------------- |
| @ref Magnum::Quaternion "Quaternion", @ref Magnum::Quaterniond "Quaterniond" | `glm::quat`, `glm::dquat` |

Types with extra qualifiers (such as `glm::mediump_dquat`) are treated the
same as types with no qualifier. Example usage:

@snippet GlmIntegration.cpp GtcIntegration

See @ref Magnum/GlmIntegration/Integration.h
and @ref Magnum/GlmIntegration/GtxIntegration.h for conversion of other types.
*/

#include <glm/gtc/quaternion.hpp>

#include "Magnum/Math/Quaternion.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum { namespace Math { namespace Implementation {

/* Quaternion */

template<class T, glm::qualifier q> struct QuaternionConverter<T, glm::tquat<T, q>> {
    static Quaternion<T> from(const glm::tquat<T, q>& other) {
        return {{other.x, other.y, other.z}, other.w};
    }

    static glm::tquat<T, q> to(const Quaternion<T>& other) {
        return {other.scalar(), other.vector().x(), other.vector().y(), other.vector().z()};
    }
};

}}}
#endif

#endif
