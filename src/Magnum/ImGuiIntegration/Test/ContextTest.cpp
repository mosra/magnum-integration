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

/* Explicitly disable deprecated functions on non-deprecated builds to catch
   issues early. Doing this only in tests so the library itself can be used
   with any newer version, but tests should be always run against the oldest
   supported which is mentioned in doc/namespaces.dox, and which is downloaded
   in all CI targets in package/ci/. The oldest supported version is tracked to
   be roughly two years back. */
#ifndef MAGNUM_BUILD_DEPRECATED
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#endif

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/ImGuiIntegration/Context.h"

namespace Magnum { namespace ImGuiIntegration { namespace Test { namespace {

struct ContextTest: TestSuite::Tester {
    explicit ContextTest();

    void constructNoCreate();
    void constructCopy();
};

ContextTest::ContextTest() {
    addTests({&ContextTest::constructNoCreate,
              &ContextTest::constructCopy});
}

void ContextTest::constructNoCreate() {
    {
        Context context{NoCreate};
        CORRADE_COMPARE(context.context(), nullptr);
    }

    CORRADE_VERIFY(true);
}

void ContextTest::constructCopy() {
    CORRADE_VERIFY(!std::is_constructible<Context, const Context&>{});
    CORRADE_VERIFY(!std::is_assignable<Context, const Context&>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::ImGuiIntegration::Test::ContextTest)
