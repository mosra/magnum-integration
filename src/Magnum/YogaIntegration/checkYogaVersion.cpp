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

#include <yoga/Yoga.h>

#ifndef CHECK_VERSION
#error CHECK_VERSION not defined
#define CHECK_VERSION 0xffffffff
#endif

int main() {
    /* Version 3.2.0 has YGDisplayContents compared to 3.1.0:
        https://github.com/facebook/yoga/commit/68bb2343d2b470962065789d09016bba8e785340 */
    #if CHECK_VERSION >= 302000
    return YGDisplayContents;
    #endif

    /* Version 3.0.0 has YGAlignSpaceEvenly compared to 2.0.1:
        https://github.com/facebook/yoga/commit/0d28b283e2c9931fe9f73f737c8fd9e58f922876 */
    #if CHECK_VERSION >= 300000
    return YGAlignSpaceEvenly;
    #endif
}
