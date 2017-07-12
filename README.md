This is integration library for Magnum C++11/C++14 OpenGL graphics engine,
providing integration of various math and physics libraries into the engine
itself. If you don't know what Magnum is, see https://github.com/mosra/magnum.

[![Join the chat at https://gitter.im/mosra/magnum](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mosra/magnum?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

SUPPORTED PLATFORMS
===================

*   **Linux** [![Build Status](https://travis-ci.org/mosra/magnum-integration.svg?branch=master)](https://travis-ci.org/mosra/magnum-integration) [![Coverage Status](https://coveralls.io/repos/github/mosra/magnum-integration/badge.svg?branch=master)](https://coveralls.io/github/mosra/magnum-integration?branch=master)
*   **Windows** [![Build Status](https://ci.appveyor.com/api/projects/status/hs6ykva1ld74vavr/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/magnum-integration/branch/master)
*   **OS X** [![Build Status](https://travis-ci.org/mosra/magnum-integration.svg?branch=master)](https://travis-ci.org/mosra/magnum-integration)
*   **iOS** [![Build Status](https://travis-ci.org/mosra/magnum-integration.svg?branch=master)](https://travis-ci.org/mosra/magnum-integration)
*   **Android** [![Build Status](https://travis-ci.org/mosra/magnum-integration.svg?branch=master)](https://travis-ci.org/mosra/magnum-integration)
*   **Windows RT** [![Build Status](https://ci.appveyor.com/api/projects/status/hs6ykva1ld74vavr/branch/master?svg=true)](https://ci.appveyor.com/project/mosra/magnum-integration/branch/master)
*   **Google Chrome**
*   **HTML5/JavaScript** [![Build Status](https://travis-ci.org/mosra/magnum-integration.svg?branch=master)](https://travis-ci.org/mosra/magnum-integration)

INSTALLATION
============

You can either use packaging scripts, which are stored in `package/`
subdirectory, or compile and install everything manually. The building process
is similar to Magnum itself - see [Magnum documentation](http://doc.magnum.graphics/magnum/)
for more comprehensive guide for building, packaging and crosscompiling.

Minimal dependencies
--------------------

*   C++ compiler with good C++11 support. Compilers which are tested to have
    everything needed are **GCC** >= 4.7, **Clang** >= 3.1 and **MSVC** >= 2015.
    On Windows you can also use **MinGW-w64**.
*   **CMake** >= 2.8.12
*   **Corrade**, **Magnum** -- The engine itself

Compilation, installation
-------------------------

The integration library can be built and installed using these four commands:

    mkdir -p build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=/usr ..
    make
    make install

Building and running unit tests
-------------------------------

If you want to build also unit tests (which are not built by default), pass
`-DBUILD_TESTS=ON` to CMake. Unit tests use Corrade's TestSuite framework and
can be run using

    ctest --output-on-failure

in build directory. Everything should pass ;-)

CONTACT
=======

Want to learn more about the library? Found a bug or want to share an awesome
idea? Feel free to visit the project website or contact the team at:

*   Website -- http://magnum.graphics
*   GitHub -- https://github.com/mosra/magnum-integration
*   Gitter -- https://gitter.im/mosra/magnum
*   IRC -- join `#magnum-engine` channel on freenode
*   Google Groups -- https://groups.google.com/forum/#!forum/magnum-engine
*   Twitter -- https://twitter.com/czmosra
*   E-mail -- mosra@centrum.cz
*   Jabber -- mosra@jabbim.cz

CREDITS
=======

See [CREDITS.md](CREDITS.md) file for details. Big thanks to everyone involved!

LICENSE
=======

Magnum is licensed under MIT/Expat license, see [COPYING](COPYING) file for
details.
