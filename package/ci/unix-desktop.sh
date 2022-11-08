#!/bin/bash
set -ev

# Corrade
git clone --depth 1 https://github.com/mosra/corrade.git
cd corrade
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    -DCORRADE_BUILD_DEPRECATED=$BUILD_DEPRECATED \
    -G Ninja
ninja install
cd ../..

# Magnum
git clone --depth 1 https://github.com/mosra/magnum.git
cd magnum
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_PREFIX_PATH=$HOME/sdl2 \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DMAGNUM_WITH_AUDIO=OFF \
    -DMAGNUM_WITH_DEBUGTOOLS=ON \
    -DMAGNUM_WITH_MATERIALTOOLS=OFF \
    -DMAGNUM_WITH_MESHTOOLS=ON \
    -DMAGNUM_WITH_PRIMITIVES=ON \
    -DMAGNUM_WITH_SCENEGRAPH=ON \
    -DMAGNUM_WITH_SCENETOOLS=OFF \
    -DMAGNUM_WITH_SHADERS=ON \
    -DMAGNUM_WITH_SHADERTOOLS=OFF \
    -DMAGNUM_WITH_TEXT=OFF \
    -DMAGNUM_WITH_TEXTURETOOLS=OFF \
    -DMAGNUM_WITH_OPENGLTESTER=ON \
    -DMAGNUM_WITH_ANYIMAGEIMPORTER=ON \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_WITH_WINDOWLESS${PLATFORM_GL_API}APPLICATION=ON \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_BUILD_DEPRECATED=$BUILD_DEPRECATED \
    -G Ninja
ninja install
cd ../..

# DartIntegration needs plugins
if [ "$WITH_DART" == "ON" ]; then
    # Magnum Plugins
    git clone --depth 1 https://github.com/mosra/magnum-plugins.git
    cd magnum-plugins
    mkdir build && cd build
    cmake .. \
        -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps \
        -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
        -DCMAKE_BUILD_TYPE=$CONFIGURATION \
        -DMAGNUM_WITH_ASSIMPIMPORTER=$WITH_DART \
        -DMAGNUM_WITH_STBIMAGEIMPORTER=$WITH_DART \
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
    -DMAGNUM_WITH_BULLET=ON \
    -DMAGNUM_WITH_DART=$WITH_DART \
    -DMAGNUM_WITH_EIGEN=ON \
    -DMAGNUM_WITH_GLM=ON \
    -DMAGNUM_WITH_IMGUI=ON \
    -DMAGNUM_WITH_OVR=OFF \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    -G Ninja
ninja $NINJA_JOBS

# DART leaks somewhere deep in std::string, run these tests separately to avoid
# suppressing too much
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always" CORRADE_TEST_COLOR=ON ctest -V -E "GLTest|Dart"
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always suppressions=$(pwd)/../package/ci/leaksanitizer.conf" CORRADE_TEST_COLOR=ON ctest -V -R Dart -E GLTest

# Test install, after running the tests as for them it shouldn't be needed
ninja install
