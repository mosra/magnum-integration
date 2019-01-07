#ifndef Magnum_ImGuiIntegration_Conversion_h
#define Magnum_ImGuiIntegration_Conversion_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Conversion of ImGui math types
 */

#include <imgui.h>
#include <Magnum/Types.h>
#include <Magnum/Math/Vector.h>

namespace Magnum { namespace Math { namespace Implementation {

/* ImVec2 */
template<> struct VectorConverter<2, Float, ImVec2> {
    static Vector<2, Float> from(const ImVec2& other) {
        return {other.x, other.y};
    }

    static ImVec2 to(const Vector<2, Float>& other) {
        return {other[0], other[1]};
    }
};

/* ImVec4 */
template<> struct VectorConverter<4, Float, ImVec4> {
    static Vector<4, Float> from(const ImVec4& other) {
        return {other.x, other.y, other.z, other.w};
    }

    static ImVec4 to(const Vector<4, Float>& other) {
        return {other[0], other[1], other[2], other[3]};
    }
};

/* ImColor */
template<> struct VectorConverter<4, Float, ImColor> {
    static Vector<4, Float> from(const ImColor& other) {
        return Vector<4, Float>(other.Value);
    }

    static ImColor to(const Vector<4, Float>& other) {
        /* Construct from ImVec4 */
        return ImVec4(other);
    }
};

}}}

#endif
