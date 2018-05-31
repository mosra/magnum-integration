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

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/GlmIntegration/Integration.h"
#include "Magnum/GlmIntegration/GtcIntegration.h"

#define GLM_ENABLE_EXPERIMENTAL /* WTF, GLM! */
#include "Magnum/GlmIntegration/GtxIntegration.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {
{
/* [Integration] */
glm::vec3 a{1.0f, 2.0f, 3.0f};
Vector3 b(a);

glm::mat3 c = glm::mat3(Matrix3::rotation(35.0_degf));
/* [Integration] */
static_cast<void>(b);
static_cast<void>(c);
}

{
/* [GtcIntegration] */
Quaterniond a = Quaterniond::rotation(35.0_deg, Vector3d::xAxis());
glm::dquat b(a);
/* [GtcIntegration] */
static_cast<void>(b);
}

{
/* [GtxIntegration] */
DualQuaternion a = DualQuaternion::translation({1.0f, 2.0f, 3.0f});
glm::dualquat b(a);
/* [GtxIntegration] */
static_cast<void>(b);
}
}
