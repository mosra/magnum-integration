/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Matrix4.h>

#include "Magnum/EigenIntegration/GeometryIntegration.h"

namespace Magnum { namespace EigenIntegration { namespace Test { namespace {

struct GeometryIntegrationTest: TestSuite::Tester {
    explicit GeometryIntegrationTest();

    void translation2D();
    void translation3D();

    void quaternion();

    void transform2DAffine();
    void transform2DIsometry();
    void transform2DAffineCompact();
    void transform3DProjective();
    void transform3DAffineCompact();
};

GeometryIntegrationTest::GeometryIntegrationTest() {
    addTests({&GeometryIntegrationTest::translation2D,
              &GeometryIntegrationTest::translation3D,

              &GeometryIntegrationTest::quaternion,

              &GeometryIntegrationTest::transform2DAffine,
              &GeometryIntegrationTest::transform2DIsometry,
              &GeometryIntegrationTest::transform2DAffineCompact,
              &GeometryIntegrationTest::transform3DProjective,
              &GeometryIntegrationTest::transform3DAffineCompact});
}

using namespace Math::Literals;

void GeometryIntegrationTest::translation2D() {
    Vector2 a{0.3f, -1.3f};
    Eigen::Translation2f b{0.3f, -1.3f};

    CORRADE_COMPARE(Vector2{b}, a);
    CORRADE_VERIFY(Eigen::Translation2f(a).isApprox(b));

    /* Verify also the cast function, people don't need to use it but should
       be allowed to for consistency */
    CORRADE_VERIFY(cast<Eigen::Translation2f>(a).isApprox(b));
}

void GeometryIntegrationTest::translation3D() {
    Vector3d a{0.3, -1.3, 2.1};
    Eigen::Translation3d b{0.3, -1.3, 2.1};

    CORRADE_COMPARE(Vector3d{b}, a);
    CORRADE_VERIFY(Eigen::Translation3d(a).isApprox(b));

    /* Verify also the cast function, people don't need to use it but should
       be allowed to for consistency */
    CORRADE_VERIFY(cast<Eigen::Translation3d>(a).isApprox(b));
}

void GeometryIntegrationTest::quaternion() {
    Quaternion a{{1.0f, 3.1f, 1.2f}, 0.5f};
    Eigen::Quaternionf b{0.5f, 1.0f, 3.1f, 1.2f};

    CORRADE_COMPARE(Quaternion{b}, a);
    CORRADE_VERIFY(Eigen::Quaternionf(a).isApprox(b));

    /* Explicitly check the parts to avoid having the conversion done wrong
       twice */
    CORRADE_COMPARE(Vector3{Eigen::Vector3f{b.vec()}}, a.vector());

    /* Verify also the cast function, people don't need to use it but should
       be allowed to for consistency */
    CORRADE_VERIFY(cast<Eigen::Quaternionf>(a).isApprox(b));
}

void GeometryIntegrationTest::transform2DAffine() {
    Matrix3 a = Matrix3::translation({-1.5f, 0.3f})*
        Matrix3::rotation(25.0_degf)*
        Matrix3::scaling({1.23f, 2.0f});

    auto b = Eigen::Affine2f::Identity();
    /* Wtf, I would expect the following xforms to be added from the left */
    b.translate(Eigen::Vector2f{-1.5f, 0.3f})
     .rotate(Float(Rad(25.0_degf)))
     .scale(Eigen::Vector2f{1.23f, 2.0f});

    CORRADE_COMPARE(Matrix3(b), a);
    CORRADE_VERIFY(Eigen::Affine2f(a).isApprox(b));

    /* Verify also the cast function, people don't need to use it but should
       be allowed to for consistency */
    CORRADE_VERIFY(cast<Eigen::Affine2f>(a).isApprox(b));
}

void GeometryIntegrationTest::transform2DIsometry() {
    Matrix3d a = Matrix3d::translation({-1.5, 0.3})*
        Matrix3d::rotation(37.0_deg);

    auto b = Eigen::Isometry2d::Identity();
    /* Wtf, I would expect the following xforms to be added from the left */
    b.translate(Eigen::Vector2d{-1.5, 0.3})
     .rotate(Double(Radd(37.0_deg)));

    CORRADE_COMPARE(Matrix3d(b), a);
    CORRADE_VERIFY(Eigen::Isometry2d(a).isApprox(b, 1.0e-7));

    /* Verify also the cast function, people don't need to use it but should
       be allowed to for consistency */
    CORRADE_VERIFY(cast<Eigen::Isometry2d>(a).isApprox(b, 1.0e-7));
}

void GeometryIntegrationTest::transform2DAffineCompact() {
    Matrix3 a = Matrix3::translation({-1.5f, 0.3f})*
        Matrix3::rotation(25.0_degf)*
        Matrix3::scaling({1.23f, 2.0f});
    Matrix3x2 a32{a[0].xy(), a[1].xy(), a[2].xy()};

    auto b = Eigen::AffineCompact2f::Identity();
    /* Wtf, I would expect the following xforms to be added from the left */
    b.translate(Eigen::Vector2f{-1.5f, 0.3f})
     .rotate(Float(Rad(25.0_degf)))
     .scale(Eigen::Vector2f{1.23f, 2.0f});

    CORRADE_COMPARE(Matrix3x2(b), a32);
    CORRADE_VERIFY(Eigen::AffineCompact2f(a32).isApprox(b));

    /* Verify also the cast function, people don't need to use it but should
       be allowed to for consistency */
    CORRADE_VERIFY(cast<Eigen::AffineCompact2f>(a32).isApprox(b));
}

void GeometryIntegrationTest::transform3DProjective() {
    Matrix4 a = Matrix4::translation({-1.5f, 0.3f, 1.4f})*
        Matrix4::rotationY(25.0_degf)*
        Matrix4::scaling({-3.3f, 1.23f, 2.0f});

    auto b = Eigen::Projective3f::Identity();
    /* Wtf, I would expect the following xforms to be added from the left */
    b.translate(Eigen::Vector3f{-1.5f, 0.3f, 1.4f})
     .rotate(Eigen::AngleAxisf{Float(Rad(25.0_degf)), Eigen::Vector3f::UnitY()})
     .scale(Eigen::Vector3f{-3.3f, 1.23f, 2.0f});

    CORRADE_COMPARE(Matrix4(b), a);
    CORRADE_VERIFY(Eigen::Projective3f(a).isApprox(b));

    /* Verify also the cast function, people don't need to use it but should
       be allowed to for consistency */
    CORRADE_VERIFY(cast<Eigen::Projective3f>(a).isApprox(b));
}

void GeometryIntegrationTest::transform3DAffineCompact() {
    Matrix4d a = Matrix4d::translation({-1.5, 0.3, 1.4})*
        Matrix4d::rotationY(25.0_deg)*
        Matrix4d::scaling({-3.3, 1.23, 2.0});
    Matrix4x3d a43{a[0].xyz(), a[1].xyz(), a[2].xyz(), a[3].xyz()};

    auto b = Eigen::AffineCompact3d::Identity();
    /* Wtf, I would expect the following xforms to be added from the left */
    b.translate(Eigen::Vector3d{-1.5, 0.3, 1.4})
     .rotate(Eigen::AngleAxisd{Double(Radd(25.0_deg)), Eigen::Vector3d::UnitY()})
     .scale(Eigen::Vector3d{-3.3, 1.23, 2.0});

    CORRADE_COMPARE(Matrix4x3d(b), a43);
    CORRADE_VERIFY(Eigen::AffineCompact3d(a43).isApprox(b));

    /* Verify also the cast function, people don't need to use it but should
       be allowed to for consistency */
    CORRADE_VERIFY(cast<Eigen::AffineCompact3d>(a43).isApprox(b));
}

}}}}

CORRADE_TEST_MAIN(Magnum::EigenIntegration::Test::GeometryIntegrationTest)
