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
    void dvec();
    void ivec();
    void uvec();
    void mat3();
    void mat4();
    void mat2x2();
    void mat2x3();
    void mat2x4();
    void mat3x2();
    void mat3x4();
    void mat4x2();
    void mat4x3();
    void dmat3();
    void dmat4();
    void dmat2x2();
    void dmat2x3();
    void dmat2x4();
    void dmat3x2();
    void dmat3x4();
    void dmat4x2();
    void dmat4x3();
};

IntegrationTest::IntegrationTest() {
    addTests({&IntegrationTest::vec,
              &IntegrationTest::dvec,
              &IntegrationTest::ivec,
              &IntegrationTest::uvec,
              &IntegrationTest::mat3,
              &IntegrationTest::mat4,
              &IntegrationTest::mat2x2,
              &IntegrationTest::mat2x3,
              &IntegrationTest::mat2x4,
              &IntegrationTest::mat3x2,
              &IntegrationTest::mat3x4,
              &IntegrationTest::mat4x2,
              &IntegrationTest::mat4x3,
              &IntegrationTest::dmat3,
              &IntegrationTest::dmat4,
              &IntegrationTest::dmat2x2,
              &IntegrationTest::dmat2x3,
              &IntegrationTest::dmat2x4,
              &IntegrationTest::dmat3x2,
              &IntegrationTest::dmat3x4,
              &IntegrationTest::dmat4x2,
              &IntegrationTest::dmat4x3});
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

void IntegrationTest::dvec() {
    {
        Vector2d a{1.0, 2.0};
        glm::dvec2 b{1.0, 2.0};

        CORRADE_COMPARE(Vector2d{b}, a);
        CORRADE_VERIFY(glm::dvec2(a) == b);
    } {
        Vector3d a{1.0, 2.0, 3.0};
        glm::dvec3 b{1.0, 2.0, 3.0};

        CORRADE_COMPARE(Vector3d{b}, a);
        CORRADE_VERIFY(glm::dvec3(a) == b);
    } {
        Vector4d a{1.0, 2.0, 3.0, 4.0};
        glm::dvec4 b{1.0, 2.0, 3.0, 4.0};

        CORRADE_COMPARE(Vector4d{b}, a);
        CORRADE_VERIFY(glm::dvec4(a) == b);
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

void IntegrationTest::uvec() {
    {
        Vector2ui a{1, 4294967295};
        glm::uvec2 b{1, 4294967295};

        CORRADE_COMPARE(Vector2ui{b}, a);
        CORRADE_VERIFY(glm::uvec2(a) == b);
    } {
        Vector3ui a{1, 4294967295, 3};
        glm::uvec3 b{1, 4294967295, 3};

        CORRADE_COMPARE(Vector3ui{b}, a);
        CORRADE_VERIFY(glm::uvec3(a) == b);
    } {
        Vector4ui a{1, 2, 3, 4294967295};
        glm::uvec4 b{1, 2, 3, 4294967295};

        CORRADE_COMPARE(Vector4ui{b}, a);
        CORRADE_VERIFY(glm::uvec4(a) == b);
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

void IntegrationTest::mat2x2() {
    Matrix2x2 a{Vector2{3.0f,  5.0f},
                Vector2{4.5f,  4.0f}};
    glm::mat2x2 b{3.0f,  5.0f,
                  4.5f,  4.0f};

    CORRADE_COMPARE(Matrix2x2{b}, a);
    CORRADE_COMPARE(Matrix2x2{b}[1][0], a[1][0]);
    CORRADE_VERIFY(glm::mat2x2(a) == b);
    CORRADE_COMPARE(glm::mat2x2(a)[0][1], b[0][1]);
}

void IntegrationTest::mat2x3() {
    Matrix2x3 a{Vector3{3.0f,  5.0f, 8.0f},
                Vector3{4.5f,  4.0f, 7.0f}};
    glm::mat2x3 b{3.0f,  5.0f, 8.0f,
                  4.5f,  4.0f, 7.0f};

    CORRADE_COMPARE(Matrix2x3{b}, a);
    CORRADE_COMPARE(Matrix2x3{b}[1][0], a[1][0]);
    CORRADE_VERIFY(glm::mat2x3(a) == b);
    CORRADE_COMPARE(glm::mat2x3(a)[0][2], b[0][2]);
}

void IntegrationTest::mat2x4() {
    Matrix2x4 a{Vector4{3.0f,  5.0f, 8.0f, 10.0f},
                Vector4{4.5f,  4.0f, 7.0f, 11.0f}};
    glm::mat2x4 b{3.0f,  5.0f, 8.0f, 10.0f,
                  4.5f,  4.0f, 7.0f, 11.0f};

    CORRADE_COMPARE(Matrix2x4{b}, a);
    CORRADE_COMPARE(Matrix2x4{b}[1][0], a[1][0]);
    CORRADE_VERIFY(glm::mat2x4(a) == b);
    CORRADE_COMPARE(glm::mat2x4(a)[0][3], b[0][3]);
}

void IntegrationTest::mat3x2() {
    Matrix3x2 a{Vector2{3.0f,  5.0f},
                Vector2{4.5f,  4.0f},
                Vector2{7.9f, -1.0f}};
    glm::mat3x2 b{3.0f,  5.0f,
                  4.5f,  4.0f,
                  7.9f, -1.0f};

    CORRADE_COMPARE(Matrix3x2{b}, a);
    CORRADE_COMPARE(Matrix3x2{b}[2][0], a[2][0]);
    CORRADE_VERIFY(glm::mat3x2(a) == b);
    CORRADE_COMPARE(glm::mat3x2(a)[0][1], b[0][1]);
}

void IntegrationTest::mat3x4() {
    Matrix3x4 a{Vector4{3.0f,  5.0f, 8.0f, 10.0f},
                Vector4{4.5f,  4.0f, 7.0f, 11.0f},
                Vector4{7.9f, -1.0f, 8.0f, 12.0f}};
    glm::mat3x4 b{3.0f,  5.0f, 8.0f, 10.0f,
                  4.5f,  4.0f, 7.0f, 11.0f,
                  7.9f, -1.0f, 8.0f, 12.0f};

    CORRADE_COMPARE(Matrix3x4{b}, a);
    CORRADE_COMPARE(Matrix3x4{b}[2][0], a[2][0]);
    CORRADE_VERIFY(glm::mat3x4(a) == b);
    CORRADE_COMPARE(glm::mat3x4(a)[0][3], b[0][3]);
}

void IntegrationTest::mat4x2() {
    Matrix4x2 a{Vector2{3.0f,  5.0f},
                Vector2{4.5f,  4.0f},
                Vector2{7.9f, -1.0f},
                Vector2{8.1f,  0.1f}};
    glm::mat4x2 b{3.0f,  5.0f,
                  4.5f,  4.0f,
                  7.9f, -1.0f,
                  8.1f,  0.1f};

    CORRADE_COMPARE(Matrix4x2{b}, a);
    CORRADE_COMPARE(Matrix4x2{b}[3][0], a[3][0]);
    CORRADE_VERIFY(glm::mat4x2(a) == b);
    CORRADE_COMPARE(glm::mat4x2(a)[0][1], b[0][1]);
}

void IntegrationTest::mat4x3() {
    Matrix4x3 a{Vector3{3.0f,  5.0f, 8.0f},
                Vector3{4.5f,  4.0f, 7.0f},
                Vector3{7.9f, -1.0f, 8.0f},
                Vector3{8.1f,  0.1f, 0.5f}};
    glm::mat4x3 b{3.0f,  5.0f, 8.0f,
                  4.5f,  4.0f, 7.0f,
                  7.9f, -1.0f, 8.0f,
                  8.1f,  0.1f, 0.5f};

    CORRADE_COMPARE(Matrix4x3{b}, a);
    CORRADE_COMPARE(Matrix4x3{b}[3][0], a[3][0]);
    CORRADE_VERIFY(glm::mat4x3(a) == b);
    CORRADE_COMPARE(glm::mat4x3(a)[0][2], b[0][2]);
}

void IntegrationTest::dmat3() {
    Matrix3d a{Vector3d{3.0,  5.0, 8.0},
               Vector3d{4.5,  4.0, 7.0},
               Vector3d{7.9, -1.0, 8.0}};
    glm::dmat3 b{3.0,  5.0, 8.0,
                 4.5,  4.0, 7.0,
                 7.9, -1.0, 8.0};

    CORRADE_COMPARE(Matrix3d{b}, a);
    CORRADE_COMPARE(Matrix3d{b}[2][0], a[2][0]);
    CORRADE_VERIFY(glm::dmat3(a) == b);
    CORRADE_COMPARE(glm::dmat3(a)[0][2], b[0][2]);
}

void IntegrationTest::dmat4() {
    Matrix4d a{Vector4d{3.0,  5.0, 8.0, 10.0},
               Vector4d{4.5,  4.0, 7.0, 11.0},
               Vector4d{7.9, -1.0, 8.0, 12.0},
               Vector4d{8.1,  0.1, 0.5, 13.0}};
    glm::dmat4 b{3.0,  5.0, 8.0, 10.0,
                 4.5,  4.0, 7.0, 11.0,
                 7.9, -1.0, 8.0, 12.0,
                 8.1,  0.1, 0.5, 13.0};

    CORRADE_COMPARE(Matrix4d{b}, a);
    CORRADE_COMPARE(Matrix4d{b}[3][0], a[3][0]);
    CORRADE_VERIFY(glm::dmat4(a) == b);
    CORRADE_COMPARE(glm::dmat4(a)[0][3], b[0][3]);
}

void IntegrationTest::dmat2x2() {
    Matrix2x2 a{Vector2{3.0f,  5.0f},
                Vector2{4.5f,  4.0f}};
    glm::mat2x2 b{3.0f,  5.0f,
                  4.5f,  4.0f};

    CORRADE_COMPARE(Matrix2x2{b}, a);
    CORRADE_COMPARE(Matrix2x2{b}[1][0], a[1][0]);
    CORRADE_VERIFY(glm::mat2x2(a) == b);
    CORRADE_COMPARE(glm::mat2x2(a)[0][1], b[0][1]);
}

void IntegrationTest::dmat2x3() {
    Matrix2x3d a{Vector3d{3.0,  5.0, 8.0},
                 Vector3d{4.5,  4.0, 7.0}};
    glm::dmat2x3 b{3.0,  5.0, 8.0,
                  4.5,  4.0, 7.0};

    CORRADE_COMPARE(Matrix2x3d{b}, a);
    CORRADE_COMPARE(Matrix2x3d{b}[1][0], a[1][0]);
    CORRADE_VERIFY(glm::dmat2x3(a) == b);
    CORRADE_COMPARE(glm::dmat2x3(a)[0][2], b[0][2]);
}

void IntegrationTest::dmat2x4() {
    Matrix2x4d a{Vector4d{3.0,  5.0, 8.0, 10.0},
                 Vector4d{4.5,  4.0, 7.0, 11.0}};
    glm::dmat2x4 b{3.0,  5.0, 8.0, 10.0,
                   4.5,  4.0, 7.0, 11.0};

    CORRADE_COMPARE(Matrix2x4d{b}, a);
    CORRADE_COMPARE(Matrix2x4d{b}[1][0], a[1][0]);
    CORRADE_VERIFY(glm::dmat2x4(a) == b);
    CORRADE_COMPARE(glm::dmat2x4(a)[0][3], b[0][3]);
}

void IntegrationTest::dmat3x2() {
    Matrix3x2d a{Vector2d{3.0,  5.0},
                 Vector2d{4.5,  4.0},
                 Vector2d{7.9, -1.0}};
    glm::dmat3x2 b{3.0,  5.0,
                  4.5,  4.0,
                  7.9, -1.0};

    CORRADE_COMPARE(Matrix3x2d{b}, a);
    CORRADE_COMPARE(Matrix3x2d{b}[2][0], a[2][0]);
    CORRADE_VERIFY(glm::dmat3x2(a) == b);
    CORRADE_COMPARE(glm::dmat3x2(a)[0][1], b[0][1]);
}

void IntegrationTest::dmat3x4() {
    Matrix3x4d a{Vector4d{3.0,  5.0, 8.0, 10.0},
                 Vector4d{4.5,  4.0, 7.0, 11.0},
                 Vector4d{7.9, -1.0, 8.0, 12.0}};
    glm::dmat3x4 b{3.0,  5.0, 8.0, 10.0,
                   4.5,  4.0, 7.0, 11.0,
                   7.9, -1.0, 8.0, 12.0};

    CORRADE_COMPARE(Matrix3x4d{b}, a);
    CORRADE_COMPARE(Matrix3x4d{b}[2][0], a[2][0]);
    CORRADE_VERIFY(glm::dmat3x4(a) == b);
    CORRADE_COMPARE(glm::dmat3x4(a)[0][3], b[0][3]);
}

void IntegrationTest::dmat4x2() {
    Matrix4x2d a{Vector2d{3.0,  5.0},
                 Vector2d{4.5,  4.0},
                 Vector2d{7.9, -1.0},
                 Vector2d{8.1,  0.1}};
    glm::dmat4x2 b{3.0,  5.0,
                   4.5,  4.0,
                   7.9, -1.0,
                   8.1,  0.1};

    CORRADE_COMPARE(Matrix4x2d{b}, a);
    CORRADE_COMPARE(Matrix4x2d{b}[3][0], a[3][0]);
    CORRADE_VERIFY(glm::dmat4x2(a) == b);
    CORRADE_COMPARE(glm::dmat4x2(a)[0][1], b[0][1]);
}

void IntegrationTest::dmat4x3() {
    Matrix4x3d a{Vector3d{3.0,  5.0, 8.0},
                 Vector3d{4.5,  4.0, 7.0},
                 Vector3d{7.9, -1.0, 8.0},
                 Vector3d{8.1,  0.1, 0.5}};
    glm::dmat4x3 b{3.0,  5.0, 8.0,
                   4.5,  4.0, 7.0,
                   7.9, -1.0, 8.0,
                   8.1,  0.1, 0.5};

    CORRADE_COMPARE(Matrix4x3d{b}, a);
    CORRADE_COMPARE(Matrix4x3d{b}[3][0], a[3][0]);
    CORRADE_VERIFY(glm::dmat4x3(a) == b);
    CORRADE_COMPARE(glm::dmat4x3(a)[0][2], b[0][2]);
}

}}}

CORRADE_TEST_MAIN(Magnum::GlmIntegration::Test::IntegrationTest)
