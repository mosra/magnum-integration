/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Janos <janos.meny@googlemail.com>

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
#include <Corrade/TestSuite/Compare/Container.h>
#include <Magnum/Magnum.h>

#include "Magnum/EigenIntegration/DynamicMatrixIntegration.h"

namespace Magnum { namespace EigenIntegration { namespace Test { namespace {

struct DynamicMatrixIntegrationTest: TestSuite::Tester {
    explicit DynamicMatrixIntegrationTest();

    void block();
    void transpose();
    void reverse();
    void broadcasted();
};

using Map2Df = Eigen::Map<Eigen::MatrixXf, Eigen::Unaligned, Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>>;
using Map1Df = Eigen::Map<Eigen::VectorXf, Eigen::Unaligned, Eigen::InnerStride<>>;

DynamicMatrixIntegrationTest::DynamicMatrixIntegrationTest() {
    addTests({&DynamicMatrixIntegrationTest::block,
              &DynamicMatrixIntegrationTest::transpose,
              &DynamicMatrixIntegrationTest::reverse,
              &DynamicMatrixIntegrationTest::broadcasted});
}

void DynamicMatrixIntegrationTest::block() {
    Eigen::MatrixXf m = Eigen::MatrixXf::Random(4, 5);
    auto row = m.row(1);
    auto col = m.col(2);
    auto block = m.block(1, 1, 2, 3);
    Containers::StridedArrayView1D<Float> rowView = arrayCast(row);
    Containers::StridedArrayView1D<Float> colView = arrayCast(col);
    Containers::StridedArrayView2D<Float> blockView = arrayCast(block);

    for(Int k = 0; k != 5; ++k) {
        CORRADE_ITERATION(k);
        CORRADE_COMPARE(rowView[k], row[k]);
    }

    for(Int k = 0; k != 4; ++k) {
        CORRADE_ITERATION(k);
        CORRADE_COMPARE(colView[k], col[k]);
    }

    for(Int i = 0; i != 3; ++i) {
        CORRADE_ITERATION(i);
        for(Int j = 0; j != 2; ++j) {
            CORRADE_ITERATION(j);
            CORRADE_COMPARE(blockView[j][i], block(j, i));
        }
    }

    Map1Df rowMapped = arrayCast(rowView);
    Map1Df colMapped = arrayCast(colView);
    Map2Df blockMapped = arrayCast(blockView);

    /* need to transpose since arrayCast returns a column vector */
    CORRADE_VERIFY(rowMapped.isApprox(row.transpose()));

    CORRADE_VERIFY(colMapped.isApprox(col));
    CORRADE_VERIFY(blockMapped.isApprox(block));
}

void DynamicMatrixIntegrationTest::transpose() {
    Float data[4*5];
    for(Int i = 0; i != 4*5; ++i)
        data[i] = Float(i);

    Containers::StridedArrayView2D<Float> view{data, {4, 5}};
    Containers::StridedArrayView2D<Float> viewTransposed = view.transposed<0, 1>();
    Map2Df mapped = arrayCast(viewTransposed);

    for(Int i = 0; i != 4; ++i) {
        CORRADE_ITERATION(i);
        for(Int j = 0; j != 5; ++j) {
            CORRADE_ITERATION(j);
            CORRADE_COMPARE(mapped(j, i), viewTransposed[j][i]);
        }
    }

    Eigen::MatrixXf m = mapped;
    Containers::StridedArrayView2D<Float> view2 = arrayCast(mapped.transpose());
    Containers::StridedArrayView2D<Float> view3 = arrayCast(m.transpose());

    for(Int i = 0; i != 4; ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE_AS(view[i], view2[i], TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(view[i], view3[i], TestSuite::Compare::Container);
    }
}

void DynamicMatrixIntegrationTest::reverse() {
    Eigen::MatrixXf m = Eigen::MatrixXf::Random(4, 5);

    Containers::StridedArrayView2D<Float> rowReversedView = arrayCast(m.rowwise().reverse());
    Containers::StridedArrayView2D<Float> colReversedView = arrayCast(m.colwise().reverse());
    Containers::StridedArrayView2D<Float> reversedView = arrayCast(m.reverse());

    Containers::StridedArrayView1D<Float> col0ReversedView = arrayCast(m.col(0).reverse());
    Containers::StridedArrayView1D<Float> row0ReversedView = arrayCast(m.row(0).reverse());

    for(Int i = 0; i != 4; ++i) {
        CORRADE_ITERATION(i);
        for(Int j = 0; j != 5; ++j) {
            CORRADE_ITERATION(j);
            CORRADE_COMPARE(m(i, j), colReversedView[4 - i - 1][j]);
            CORRADE_COMPARE(m(i, j), rowReversedView[i][5 - j - 1]);
            CORRADE_COMPARE(m(i, j), reversedView[4 - i - 1][5 - j -1]);
        }
    }

    for(Int k = 0; k != 4; ++k) {
        CORRADE_ITERATION(k);
        CORRADE_COMPARE(col0ReversedView[k], m.col(0)[4 - k - 1]);
    }
    for(Int k = 0; k != 5; ++k) {
        CORRADE_ITERATION(k);
        CORRADE_COMPARE(row0ReversedView[k], m.row(0)[5 - k - 1]);
    }

    Map2Df mapped1 = arrayCast(rowReversedView.flipped<1>());
    Map2Df mapped2 = arrayCast(colReversedView.flipped<0>());
    Map2Df mapped3 = arrayCast(reversedView.flipped<0>().flipped<1>());

    Map1Df mapped4 = arrayCast(col0ReversedView.flipped<0>());
    Map1Df mapped5 = arrayCast(row0ReversedView.flipped<0>());

    CORRADE_VERIFY(mapped1.isApprox(m));
    CORRADE_VERIFY(mapped2.isApprox(m));
    CORRADE_VERIFY(mapped3.isApprox(m));

    CORRADE_VERIFY(mapped4.isApprox(m.col(0)));
    CORRADE_VERIFY(mapped5.isApprox(m.row(0).transpose()));
}

void DynamicMatrixIntegrationTest::broadcasted() {
    Float data[5];
    for(Int k = 0; k != 5; ++k)
        data[k] = Float(k);

    Containers::StridedArrayView1D<Float> view{data};
    Map2Df mapped = arrayCast(view.slice<2>().broadcasted<1>(10));

    for(Int i = 0; i != 5; ++i) {
        CORRADE_ITERATION(i);
        for(Int j = 0; j != 10; ++j) {
            CORRADE_ITERATION(j);
            CORRADE_COMPARE(mapped(i, j), view[i]);
        }
    }

    Containers::StridedArrayView2D<Float> view2 = arrayCast(mapped);
    for(Int i = 0; i != 5; ++i) {
        CORRADE_ITERATION(i);
        for(Int j = 0; j != 10; ++j) {
            CORRADE_ITERATION(j);
            CORRADE_COMPARE(view2[i][j], data[i]);
        }
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::EigenIntegration::Test::DynamicMatrixIntegrationTest)
