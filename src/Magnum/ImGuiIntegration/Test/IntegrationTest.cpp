/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
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

#include <Corrade/TestSuite/Tester.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Color.h>

#include "Magnum/ImGuiIntegration/Integration.h"

namespace Magnum { namespace ImGuiIntegration { namespace Test { namespace {

struct IntegrationTest: TestSuite::Tester {
    explicit IntegrationTest();

    void vector2();
    void vector4();
    void color();
    void colorLiterals();
};

IntegrationTest::IntegrationTest() {
    addTests({&IntegrationTest::vector2,
              &IntegrationTest::vector4,
              &IntegrationTest::color,
              &IntegrationTest::colorLiterals});
}

void IntegrationTest::vector2() {
    ImVec2 imVec2{1.1f, 1.2f};
    Vector2 vec2(1.1f, 1.2f);

    CORRADE_COMPARE(Vector2(imVec2), vec2);

    ImVec2 c(vec2);
    CORRADE_COMPARE(c.x, vec2.x());
    CORRADE_COMPARE(c.y, vec2.y());
}

void IntegrationTest::vector4() {
    ImVec4 imVec4{2.1f, 2.2f, 2.3f, 2.4f};
    Vector4 vec4(2.1f, 2.2f, 2.3f, 2.4f);

    CORRADE_COMPARE(Vector4(imVec4), vec4);

    ImVec4 c(vec4);
    CORRADE_COMPARE(c.x, vec4.x());
    CORRADE_COMPARE(c.y, vec4.y());
    CORRADE_COMPARE(c.z, vec4.z());
    CORRADE_COMPARE(c.w, vec4.w());
}

void IntegrationTest::color() {
    ImColor imColor{0.1f, 0.2f, 0.3f, 0.4f};
    Color4 color(0.1f, 0.2f, 0.3f, 0.4f);

    CORRADE_COMPARE(Color4(imColor), color);

    ImColor c(color);
    CORRADE_COMPARE(c.Value.x, color.x());
    CORRADE_COMPARE(c.Value.y, color.y());
    CORRADE_COMPARE(c.Value.z, color.z());
    CORRADE_COMPARE(c.Value.w, color.w());
}

void IntegrationTest::colorLiterals() {
    using namespace Math::Literals;

    ImColor color3 = ImColor(0xff3366_rgbf);
    ImColor color4 = ImColor(0xff336688_rgbaf);

    CORRADE_COMPARE(Color4(color3), 0xff3366ff_rgbaf);
    CORRADE_COMPARE(Color4(color4), 0xff336688_rgbaf);
}

}}}}

CORRADE_TEST_MAIN(Magnum::ImGuiIntegration::Test::IntegrationTest)
