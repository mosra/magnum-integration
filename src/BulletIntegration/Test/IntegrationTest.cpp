/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2013 Jan Dupal <dupal.j@gmail.com>

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

#include <TestSuite/Tester.h>

#include "BulletIntegration/Integration.h"

namespace Magnum { namespace BulletIntegration { namespace Test {

typedef Math::Vector<3, btScalar> Vector3;

class IntegrationTest: public Corrade::TestSuite::Tester {
    public:
        IntegrationTest();

        void vector();
};

IntegrationTest::IntegrationTest() {
    addTests({&IntegrationTest::vector});
}

void IntegrationTest::vector() {
    constexpr Vector3 a(1.0f, 2.0f, 3.0f);
    btVector3 b{1.0f, 2.0f, 3.0f};

    CORRADE_COMPARE(Vector3(b), a);
    CORRADE_VERIFY(btVector3(a) == b);
}

}}}

CORRADE_TEST_MAIN(Magnum::BulletIntegration::Test::IntegrationTest)
