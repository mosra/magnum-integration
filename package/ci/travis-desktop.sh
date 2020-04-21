#!/bin/bash
set -ev

# Corrade
git clone --depth 1 git://github.com/mosra/corrade.git
cd corrade
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DWITH_INTERCONNECT=OFF \
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED \
    -G Ninja
ninja install
cd ../..

# Magnum
git clone --depth 1 git://github.com/mosra/magnum.git
cd magnum
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_PREFIX_PATH=$HOME/sdl2 \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DWITH_AUDIO=OFF \
    -DWITH_DEBUGTOOLS=OFF \
    -DWITH_MESHTOOLS=$WITH_DART \
    -DWITH_PRIMITIVES=$WITH_DART \
    -DWITH_SCENEGRAPH=ON \
    -DWITH_SHADERS=ON \
    -DWITH_TEXT=OFF \
    -DWITH_TEXTURETOOLS=OFF \
    -DWITH_OPENGLTESTER=ON \
    -DWITH_ANYIMAGEIMPORTER=ON \
    -DWITH_SDL2APPLICATION=ON \
    -DWITH_WINDOWLESS${PLATFORM_GL_API}APPLICATION=ON \
    -DWITH_SDL2APPLICATION=ON \
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED \
    -G Ninja
ninja install
cd ../..

# DartIntegration needs plugins
if [ "$TRAVIS_OS_NAME" == "osx" ]; then
    # Magnum Plugins
    git clone --depth 1 git://github.com/mosra/magnum-plugins.git
    cd magnum-plugins
    mkdir build && cd build
    cmake .. \
        -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps \
        -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
        -DCMAKE_BUILD_TYPE=$CONFIGURATION \
        -DWITH_ASSIMPIMPORTER=$WITH_DART \
        -DWITH_STBIMAGEIMPORTER=$WITH_DART \
        -G Ninja
    ninja install
    cd ../..
fi

mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_PREFIX_PATH="$HOME/deps-dart;$HOME/sdl2" \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DIMGUI_DIR=$HOME/imgui \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DWITH_BULLET=ON \
    -DWITH_DART=$WITH_DART \
    -DWITH_EIGEN=ON \
    -DWITH_GLM=ON \
    -DWITH_IMGUI=ON \
    -DWITH_OVR=OFF \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=ON \
    -G Ninja
# Otherwise the job gets killed (probably because using too much memory)
ninja -j4

# DART leaks somewhere deep in std::string, run these tests separately to avoid
# suppressing too much
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always" CORRADE_TEST_COLOR=ON ctest -V -E "GLTest|Dart"
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always suppressions=$TRAVIS_BUILD_DIR/package/ci/leaksanitizer.conf" CORRADE_TEST_COLOR=ON ctest -V -R Dart -E GLTest
