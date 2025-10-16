/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Magnum/Ui/Handle.h>

#include "Magnum/YogaIntegration/Layouter.h"

namespace Magnum { namespace YogaIntegration { namespace Test { namespace {

struct LayouterTest: TestSuite::Tester {
    explicit LayouterTest();

    void construct();
    void constructCopy();
    void constructMove();

    void invalidHandle();
};

LayouterTest::LayouterTest() {
    addTests({&LayouterTest::construct,
              &LayouterTest::constructCopy,
              &LayouterTest::constructMove,

              &LayouterTest::invalidHandle});
}

void LayouterTest::construct() {
    Layouter layouter{Ui::layouterHandle(0xab, 0x12)};
    CORRADE_COMPARE(layouter.handle(), Ui::layouterHandle(0xab, 0x12));
}

void LayouterTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<Layouter>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Layouter>{});
}

void LayouterTest::constructMove() {
    Layouter a{Ui::layouterHandle(0xab, 0x12)};

    Layouter b{Utility::move(a)};
    CORRADE_COMPARE(b.handle(), Ui::layouterHandle(0xab, 0x12));

    Layouter c{Ui::layouterHandle(3, 5)};
    c = Utility::move(b);
    CORRADE_COMPARE(c.handle(), Ui::layouterHandle(0xab, 0x12));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Layouter>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Layouter>::value);
}

void LayouterTest::invalidHandle() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Layouter layouter{Ui::layouterHandle(0, 1)};

    Containers::String out;
    Error redirectError{&out};
    layouter.yogaNode(Ui::LayoutHandle::Null);
    layouter.yogaNode(Ui::LayouterDataHandle::Null);
    CORRADE_COMPARE_AS(out,
        "YogaIntegration::Layouter::yogaNode(): invalid handle Ui::LayoutHandle::Null\n"
        "YogaIntegration::Layouter::yogaNode(): invalid handle Ui::LayouterDataHandle::Null\n",
        TestSuite::Compare::String);
}

}}}}

CORRADE_TEST_MAIN(Magnum::YogaIntegration::Test::LayouterTest)
