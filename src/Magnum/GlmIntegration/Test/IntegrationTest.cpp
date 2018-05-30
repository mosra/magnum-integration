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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/GlmIntegration/Integration.h"

namespace Magnum { namespace GlmIntegration { namespace Test {

struct IntegrationTest: TestSuite::Tester {
    explicit IntegrationTest();

    void vec();
    void ivec();
    void mat3();
    void mat4();
};

IntegrationTest::IntegrationTest() {
    addTests({&IntegrationTest::vec,
              &IntegrationTest::ivec,
              &IntegrationTest::mat3,
              &IntegrationTest::mat4});
}

void IntegrationTest::vec() {
    {
        Vector2 a{1.0f, 2.0f};
        glm::vec2 b{1.0f, 2.0f};

        CORRADE_COMPARE(Vector2{b}, a);
        CORRADE_VERIFY(glm::vec2(a) == b);
    } {
        Vector3 a{1.0f, 2.0f, 3.0f};
        glm::vec3 b{1.0f, 2.0f, 3.0f};

        CORRADE_COMPARE(Vector3{b}, a);
        CORRADE_VERIFY(glm::vec3(a) == b);
    } {
        Vector4 a{1.0f, 2.0f, 3.0f, 4.0f};
        glm::vec4 b{1.0f, 2.0f, 3.0f, 4.0f};

        CORRADE_COMPARE(Vector4{b}, a);
        CORRADE_VERIFY(glm::vec4(a) == b);
    }
}

void IntegrationTest::ivec() {
    {
        Vector2i a{-1, 2};
        glm::ivec2 b{-1, 2};

        CORRADE_COMPARE(Vector2i{b}, a);
        CORRADE_VERIFY(glm::ivec2(a) == b);
    } {
        Vector3i a{1, -2, 3};
        glm::ivec3 b{1, -2, 3};

        CORRADE_COMPARE(Vector3i{b}, a);
        CORRADE_VERIFY(glm::ivec3(a) == b);
    } {
        Vector4i a{1, 2, 3, -4};
        glm::ivec4 b{1, 2, 3, -4};

        CORRADE_COMPARE(Vector4i{b}, a);
        CORRADE_VERIFY(glm::ivec4(a) == b);
    }
}

void IntegrationTest::mat3() {
    Matrix3 a{Vector3{3.0f,  5.0f, 8.0f},
              Vector3{4.5f,  4.0f, 7.0f},
              Vector3{7.9f, -1.0f, 8.0f}};
    glm::mat3 b{3.0f,  5.0f, 8.0f,
                4.5f,  4.0f, 7.0f,
                7.9f, -1.0f, 8.0f};

    CORRADE_COMPARE(Matrix3{b}, a);
    CORRADE_COMPARE(Matrix3{b}[2][0], a[2][0]);
    CORRADE_VERIFY(glm::mat3(a) == b);
    CORRADE_COMPARE(glm::mat3(a)[0][2], b[0][2]);
}

void IntegrationTest::mat4() {
    Matrix4 a{Vector4{3.0f,  5.0f, 8.0f, 10.0f},
              Vector4{4.5f,  4.0f, 7.0f, 11.0f},
              Vector4{7.9f, -1.0f, 8.0f, 12.0f},
              Vector4{8.1f,  0.1f, 0.5f, 13.0f}};
    glm::mat4 b{3.0f,  5.0f, 8.0f, 10.0f,
                4.5f,  4.0f, 7.0f, 11.0f,
                7.9f, -1.0f, 8.0f, 12.0f,
                8.1f,  0.1f, 0.5f, 13.0f};

    CORRADE_COMPARE(Matrix4{b}, a);
    CORRADE_COMPARE(Matrix4{b}[3][0], a[3][0]);
    CORRADE_VERIFY(glm::mat4(a) == b);
    CORRADE_COMPARE(glm::mat4(a)[0][3], b[0][3]);
}

}}}

CORRADE_TEST_MAIN(Magnum::GlmIntegration::Test::IntegrationTest)
