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
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_INTERCONNECT=OFF \
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
    -DCMAKE_PREFIX_PATH="$HOME/sdl2;$HOME/swiftshader" \
    -DCMAKE_INSTALL_RPATH="$HOME/deps/lib;$HOME/swiftshader/lib" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTARGET_GLES=ON \
    -DTARGET_GLES2=$TARGET_GLES2 \
    -DWITH_AUDIO=OFF \
    -DWITH_DEBUGTOOLS=OFF \
    -DWITH_MESHTOOLS=OFF \
    -DWITH_PRIMITIVES=OFF \
    -DWITH_SCENEGRAPH=ON \
    -DWITH_SHADERS=ON \
    -DWITH_SHADERTOOLS=OFF \
    -DWITH_TEXT=OFF \
    -DWITH_TEXTURETOOLS=OFF \
    -DWITH_OPENGLTESTER=ON \
    -DWITH_ANYIMAGEIMPORTER=OFF \
    -DWITH_SDL2APPLICATION=ON \
    -DWITH_WINDOWLESS${PLATFORM_GL_API}APPLICATION=ON \
    -DWITH_SDL2APPLICATION=ON \
    -G Ninja
ninja install
cd ../..

mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_PREFIX_PATH="$HOME/deps-dart;$HOME/sdl2;$HOME/swiftshader" \
    -DCMAKE_INSTALL_RPATH="$HOME/deps/lib;$HOME/swiftshader/lib" \
    -DIMGUI_DIR=$HOME/imgui \
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_BULLET=ON \
    -DWITH_DART=OFF \
    -DWITH_EIGEN=ON \
    -DWITH_GLM=ON \
    -DWITH_IMGUI=ON \
    -DWITH_OVR=OFF \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=ON \
    -G Ninja
# Otherwise the job gets killed (probably because using too much memory)
ninja -j4
CORRADE_TEST_COLOR=ON ctest -V
if [ "$TARGET_GLES2" == "ON" ]; then CORRADE_TEST_COLOR=ON MAGNUM_DISABLE_EXTENSIONS="OES_vertex_array_object" ctest -V -R GLTest; fi
