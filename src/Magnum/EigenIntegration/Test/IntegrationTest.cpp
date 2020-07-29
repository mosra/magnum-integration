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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector4.h>

#include "Magnum/EigenIntegration/Integration.h"

/* Eigen types don't have operator==, so I can't use them in CORRADE_COMPARE().
   I could use CORRADE_VERIFY(actual.isApprox(expected)), but that would not
   print the actual values on errors -- and since Eigen has ostream operator<<
   overloads, it'd be nice to use them. That's why this atrocity -- basically
   a copy of the TestSuite::Comparator, but for a custom pseudo type and using
   isApprox() instead of ==. */
namespace {
    template<class> class EigenType;
}

namespace Corrade { namespace TestSuite {

template<class T> class Comparator<EigenType<T>> {
    public:
        ComparisonStatusFlags operator()(const T& actual, const T& expected) {
            if(actual.isApprox(expected)) return {};

            actualValue = &actual;
            expectedValue = &expected;
            return ComparisonStatusFlag::Failed;
        }

        void printMessage(ComparisonStatusFlags, Utility::Debug& out, const std::string& actual, const std::string& expected) const {
            CORRADE_INTERNAL_ASSERT(actualValue && expectedValue);
            out << "Values" << actual << "and" << expected << "are not the same, actual is\n       "
            << *actualValue << Utility::Debug::newline << "        but expected\n       " << *expectedValue;
        }

    private:
        const T* actualValue{};
        const T* expectedValue{};
};

}}

namespace Magnum { namespace EigenIntegration { namespace Test { namespace {

struct IntegrationTest: TestSuite::Tester {
    explicit IntegrationTest();

    void boolVector();

    void vectorArray();
    void vectorMatrix();

    void matrixArray();
    void matrixMatrix();

    void stridedArrayView();
    void matrixXfBlock();
    void matrixXfMap();
    void matrixXf();
    void matrixXfTranspose();
    void vectorXf();
    void vectorXfBlock();
};

IntegrationTest::IntegrationTest() {
    addTests({&IntegrationTest::boolVector,

              &IntegrationTest::vectorArray,
              &IntegrationTest::vectorMatrix,

              &IntegrationTest::matrixArray,
              &IntegrationTest::matrixMatrix,

              &IntegrationTest::stridedArrayView,
              &IntegrationTest::matrixXf,
              &IntegrationTest::matrixXfBlock,
              &IntegrationTest::matrixXfMap,
              &IntegrationTest::matrixXfTranspose,
              &IntegrationTest::vectorXf,
              &IntegrationTest::vectorXfBlock});
}

void IntegrationTest::boolVector() {
    /** @todo does Eigen have typedefs for this? */
    Math::BoolVector<4> a{0xa};
    Eigen::Array<bool, 4, 1> b;
    b << false, true, false, true;
    CORRADE_COMPARE(Math::BoolVector<4>{b}, a);

    #if defined(__GNUC__) && !defined(__clang__)
    /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
       here. We don't care one bit, so silence that. Clang doesn't have this
       warning enabled in UseCorrade.cmake. */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
    #endif
    Eigen::Ref<Eigen::Array<bool, 4, 1>> bref{b};
    Eigen::Ref<const Eigen::Array<bool, 4, 1>> cbref{b};
    #if defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
    CORRADE_COMPARE(Math::BoolVector<4>{bref}, a);
    CORRADE_COMPARE(Math::BoolVector<4>{cbref}, a);

    CORRADE_COMPARE_AS((cast<Eigen::Array<bool, 4, 1>>(a)), b, EigenType);
}

void IntegrationTest::vectorArray() {
    Vector4 a{1.5f, 0.3f, -1.1f, 0.6f};
    Eigen::Array4f b;
    b << 1.5f, 0.3f, -1.1f, 0.6f;
    CORRADE_COMPARE(Vector4{b}, a);

    #if defined(__GNUC__) && !defined(__clang__)
    /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
       here. We don't care one bit, so silence that. Clang doesn't have this
       warning enabled in UseCorrade.cmake. */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
    #endif
    Eigen::Ref<Eigen::Array4f> bref{b};
    Eigen::Ref<const Eigen::Array4f> cbref{b};
    #if defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
    CORRADE_COMPARE(Vector4{bref}, a);
    CORRADE_COMPARE(Vector4{cbref}, a);

    CORRADE_COMPARE_AS(cast<Eigen::Array4f>(a), b, EigenType);
}

void IntegrationTest::vectorMatrix() {
    Vector4i a{1, 0, -3, 4457};
    Eigen::Vector4i b{1, 0, -3, 4457};
    CORRADE_COMPARE(Vector4i{b}, a);

    #if defined(__GNUC__) && !defined(__clang__)
    /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
       here. We don't care one bit, so silence that. Clang doesn't have this
       warning enabled in UseCorrade.cmake. */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
    #endif
    Eigen::Ref<Eigen::Vector4i> bref{b};
    Eigen::Ref<const Eigen::Vector4i> cbref{b};
    #if defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
    CORRADE_COMPARE(Vector4i{bref}, a);
    CORRADE_COMPARE(Vector4i{cbref}, a);

    CORRADE_COMPARE_AS(cast<Eigen::Vector4i>(a), b, EigenType);
}

void IntegrationTest::matrixArray() {
    Matrix3x2 a{Vector2{1.5f, 0.3f},
                Vector2{-1.1f, 0.6f},
                Vector2{0.5f, 7.8f}};
    Eigen::Array<float, 2, 3> b;
    b << 1.5f, -1.1f, 0.5f,
         0.3f, 0.6f, 7.8f;
    CORRADE_COMPARE(Matrix3x2{b}, a);

    #if defined(__GNUC__) && !defined(__clang__)
    /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
       here. We don't care one bit, so silence that. Clang doesn't have this
       warning enabled in UseCorrade.cmake. */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
    #endif
    Eigen::Ref<Eigen::Array<float, 2, 3>> bref{b};
    Eigen::Ref<const Eigen::Array<float, 2, 3>> cbref{b};
    #if defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
    CORRADE_COMPARE(Matrix3x2{bref}, a);
    CORRADE_COMPARE(Matrix3x2{cbref}, a);

    CORRADE_COMPARE_AS((cast<Eigen::Array<float, 2, 3>>(a)), b, EigenType);
}

void IntegrationTest::matrixMatrix() {
    Matrix3x2d a{Vector2d{1.5, 0.3},
                 Vector2d{-1.1, 0.6},
                 Vector2d{0.5, 7.8}};
    Eigen::Matrix<double, 2, 3> b;
    b << 1.5, -1.1, 0.5,
         0.3, 0.6, 7.8;
    CORRADE_COMPARE(Matrix3x2d{b}, a);

    #if defined(__GNUC__) && !defined(__clang__)
    /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
       here. We don't care one bit, so silence that. Clang doesn't have this
       warning enabled in UseCorrade.cmake. */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
    #endif
    Eigen::Ref<Eigen::Matrix<double, 2, 3>> bref{b};
    Eigen::Ref<const Eigen::Matrix<double, 2, 3>> cbref{b};
    #if defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
    CORRADE_COMPARE(Matrix3x2d{bref}, a);
    CORRADE_COMPARE(Matrix3x2d{cbref}, a);

    CORRADE_COMPARE_AS((cast<Eigen::Matrix<double, 2, 3>>(a)), b, EigenType);
}


void IntegrationTest::stridedArrayView() {
    float data[4*5];
    for(int i = 0; i < 4*5; ++i) data[i] = float(i);
    Containers::StridedArrayView2D<float> view1(data, {4, 5});
    auto mapped = arrayCast(view1);
    auto view2 = arrayCast(mapped);
    for(int i = 0; i < 4; ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE_AS(view1[i], view2[i], TestSuite::Compare::Container);
    }
}

void IntegrationTest::matrixXf() {
    Eigen::MatrixXf m = Eigen::MatrixXf::Random(4,5);
    auto view = arrayCast(m);
    auto mapped = arrayCast(view);
    CORRADE_COMPARE_AS(mapped, m, EigenType);
}

void IntegrationTest::matrixXfMap() {
    float data[4*5];
    for(int i = 0; i < 4*5; ++i) data[i] = float(i);
    Eigen::Map<Eigen::MatrixXf> mapped1(data, 4, 5);
    auto view = arrayCast(mapped1);
    auto mapped2 = arrayCast(view);
    CORRADE_VERIFY(mapped1.isApprox(mapped2));
}

void IntegrationTest::matrixXfBlock() {
    Eigen::MatrixXf m = Eigen::MatrixXf::Random(4,5);
    auto block = m.block(1,1,3, 4);
    auto view = arrayCast(block);
    auto mapped = arrayCast(view);
    CORRADE_VERIFY(mapped.isApprox(block));
}

void IntegrationTest::matrixXfTranspose() {
    Eigen::MatrixXf m = Eigen::MatrixXf::Random(4,5);
    auto view = arrayCast(m.transpose());
    auto mapped = arrayCast(view);
    CORRADE_VERIFY(mapped.isApprox(m.transpose()));
}

void IntegrationTest::vectorXf() {
    Eigen::VectorXf v = Eigen::VectorXf::Random(4);
    auto view1 = arrayCast(v);
    auto mapped1 = arrayCast(view1);
    CORRADE_VERIFY(v.isApprox(mapped1));

    auto vt = v.transpose();
    auto view2 = arrayCast(vt);
    auto mapped2 = arrayCast(view2);
    /* since arrayCast always returns a Vector we compare with the original one */
    CORRADE_VERIFY(v.isApprox(mapped2));
}

void IntegrationTest::vectorXfBlock() {
    Eigen::MatrixXf m = Eigen::MatrixXf::Random(4, 5);
    auto row = m.row(1);
    auto view1 = arrayCast(row);
    auto mapped1 = arrayCast(view1);
    /* need to transpose since arrayCast returns a column vector */
    CORRADE_VERIFY(mapped1.isApprox(row.transpose()));

    auto col = m.col(2);
    auto view2 = arrayCast(col);
    auto mapped2 = arrayCast(view2);
    CORRADE_VERIFY(mapped2.isApprox(col));
}

}}}}

CORRADE_TEST_MAIN(Magnum::EigenIntegration::Test::IntegrationTest)
