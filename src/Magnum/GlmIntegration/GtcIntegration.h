#ifndef Magnum_GlmIntegration_GtcIntegration_h
#define Magnum_GlmIntegration_GtcIntegration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
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

Provides conversion for the following types. See
@ref Magnum/GlmIntegration/Integration.h and
@ref Magnum/GlmIntegration/GtxIntegration.h for conversion of other types.

| Magnum type                           | Equivalent GLM type               |
| ------------------------------------- | --------------------------------- |
| @ref Magnum::Quaternion "Quaternion", @ref Magnum::Quaterniond "Quaterniond" | `glm::quat`, `glm::dquat` |

Types with extra qualifiers (such as `glm::mediump_dquat`) are treated the
same as types with no qualifier. Debug output using @ref Corrade::Utility::Debug
for all types is provided as well on GLM >= 0.9.7. Example usage:

@snippet GlmIntegration.cpp GtcIntegration

@see @ref types-thirdparty-integration
*/

#include <glm/gtc/quaternion.hpp>

#include "Magnum/Math/Quaternion.h"

/* Don't list (useless) Magnum and Math namespaces without anything else */
#ifndef DOXYGEN_GENERATING_OUTPUT
#if GLM_VERSION < 96 /* Was just two decimals in the old days, now it's 3 */
namespace glm {
    /* The types were moved from glm::detail to glm in 0.9.6. Can't be bothered
       so I'm just making aliases here. */
    template<class T, glm::precision q> using tquat = detail::tquat<T, q>;
}
#endif

namespace Magnum { namespace Math { namespace Implementation {

/* Quaternion */

template<class T,
    #if GLM_VERSION < 990
    glm::precision
    #else
    glm::qualifier /* thanks, GLM */
    #endif
q> struct QuaternionConverter<T, glm::tquat<T, q>> {
    static Quaternion<T> from(const glm::tquat<T, q>& other) {
        return {{other.x, other.y, other.z}, other.w};
    }

    static glm::tquat<T, q> to(const Quaternion<T>& other) {
        #if GLM_VERSION*10 + GLM_VERSION_REVISION < 952
        /* Due to initializer list fiasco in 0.9.5.1, using {} gives a totally
           different result. https://github.com/g-truc/glm/issues/159 */
        return glm::tquat<T, q>(other.scalar(), other.vector().x(), other.vector().y(), other.vector().z());
        #else
        return {other.scalar(), other.vector().x(), other.vector().y(), other.vector().z()};
        #endif
    }
};

}}}
#endif

#if !defined(CORRADE_NO_DEBUG) && (defined(DOXYGEN_GENERATING_OUTPUT) || GLM_VERSION >= 97)
namespace glm {
    /**
     * @brief Debug output operator for GLM quaternion types
     *
     * Uses `glm::to_string()` internally. Available since GLM 0.9.7.
     */
    template<class T,
        #if GLM_VERSION < 990
        glm::precision
        #else
        glm::qualifier /* thanks, GLM */
        #endif
    q> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const tquat<T, q>& value);
}
#endif

#endif
