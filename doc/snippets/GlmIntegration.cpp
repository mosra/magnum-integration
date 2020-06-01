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

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/GlmIntegration/Integration.h"

#if GLM_VERSION < 96
#define GLM_FORCE_RADIANS /* Otherwise 0.9.5 spits a lot of loud messages :/ */
#endif
#include "Magnum/GlmIntegration/GtcIntegration.h"

#if GLM_VERSION >= 990
#define GLM_ENABLE_EXPERIMENTAL /* WTF, GLM! */
#endif
#include "Magnum/GlmIntegration/GtxIntegration.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {
{
/* The include is already above, so doing it again here should be harmless */
/* [namespace] */
#include <Magnum/GlmIntegration/Integration.h>

glm::vec3 a{1.0f, 2.0f, 3.0f};
Vector3 b(a);

auto c = Matrix4::rotation(35.0_degf, Vector3(a));
/* [namespace] */
static_cast<void>(c);
}

{
/* [Integration] */
glm::vec3 a{1.0f, 2.0f, 3.0f};
Vector3 b(a);

glm::mat3 c = glm::mat3(Matrix3::rotation(35.0_degf));

Debug{} << glm::lowp_ivec3{1, 42, -3}; // prints ivec3(1, 42, -3)
/* [Integration] */
static_cast<void>(b);
static_cast<void>(c);
}

#if GLM_VERSION >= 97
{
/* [GtcIntegration] */
Quaterniond a = Quaterniond::rotation(35.0_deg, Vector3d::xAxis());
glm::dquat b(a);

Debug{} << glm::mediump_quat{4.0f, 1.0f, 2.0f, 3.0f};
    // prints quat(4.000000, {1.000000, 2.000000, 3.000000})
/* [GtcIntegration] */
static_cast<void>(b);
}

{
/* [GtxIntegration] */
DualQuaternion a = DualQuaternion::translation({1.0f, 2.0f, 3.0f});
glm::dualquat b(a);

Debug{} << glm::highp_ddualquat{{4.0, 1.0, 2.0, 3.0},
                                {8.0, 5.0, 6.0, 7.0}};
    // prints ddualquat((4.000000, {1.000000, 2.000000, 3.000000}),
    //                  (8.000000, {5.000000, 6.000000, 7.000000}))
/* [GtxIntegration] */
static_cast<void>(b);
}
#endif
}
