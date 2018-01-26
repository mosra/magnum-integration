#!/bin/bash
set -ev

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
    # libccd
    git clone --depth 1 git://github.com/danfis/libccd.git
    cd libccd
    mkdir build && cd build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps \
        -DCMAKE_INSTALL_RPATH=$HOME/deps/lib
    make -j install
    cd ../..

    # fcl
    git clone git://github.com/flexible-collision-library/fcl.git
    cd fcl
    git checkout fcl-0.5
    mkdir build && cd build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps \
        -DCMAKE_INSTALL_RPATH=$HOME/deps/lib
    # Otherwise the job gets killed (probably because using too much memory)
    make -j4 install
    cd ../..

    # DART
    git clone git://github.com/dartsim/dart.git
    cd dart
    git checkout release-6.3
    mkdir build && cd build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps \
        -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
        -DCMAKE_BUILD_TYPE=Debug
    # Otherwise the job gets killed (probably because using too much memory)
    make -j4 install
    cd ../..
fi

# Corrade
git clone --depth 1 git://github.com/mosra/corrade.git
cd corrade
mkdir build && cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_INTERCONNECT=OFF \
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED
make -j install
cd ../..

# Magnum
git clone --depth 1 git://github.com/mosra/magnum.git
cd magnum
mkdir build && cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_AUDIO=OFF \
    -DWITH_DEBUGTOOLS=OFF \
    -DWITH_MESHTOOLS=ON \
    -DWITH_PRIMITIVES=ON \
    -DWITH_SCENEGRAPH=ON \
    -DWITH_SHADERS=ON \
    -DWITH_SHAPES=ON \
    -DWITH_TEXT=OFF \
    -DWITH_TEXTURETOOLS=OFF \
    -DWITH_WINDOWLESS${PLATFORM_GL_API}APPLICATION=ON \
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED
make -j install
cd ../..

if [ "$TRAVIS_OS_NAME" == "linux" ]; then
    # Magnum Plugins
    git clone --depth 1 git://github.com/mosra/magnum-plugins.git
    cd magnum-plugins
    mkdir build && cd build
    cmake .. \
        -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps \
        -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
        -DCMAKE_BUILD_TYPE=Debug \
        -DWITH_ASSIMPIMPORTER=ON \
        -DWITH_JPEGIMPORTER=ON \
        -DWITH_PNGIMPORTER=ON \
        -DWITH_STBIMAGEIMPORTER=ON
    # Otherwise the job gets killed (probably because using too much memory)
    make -j4
    make install
    cd ../..
fi

mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_BULLET=ON \
    -DWITH_DART=$WITH_DART \
    -DWITH_OVR=OFF \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=ON
# Otherwise the job gets killed (probably because using too much memory)
make -j4
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always" CORRADE_TEST_COLOR=ON ctest -V -E "GLTest|Dart"
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always suppressions=$TRAVIS_BUILD_DIR/package/ci/leaksanitizer.conf" CORRADE_TEST_COLOR=ON ctest -V -R Dart
