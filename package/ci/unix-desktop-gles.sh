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
    -DCMAKE_BUILD_TYPE=Debug \
    -DCORRADE_WITH_INTERCONNECT=OFF \
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
    -DCMAKE_PREFIX_PATH="$HOME/sdl2;$HOME/swiftshader" \
    -DCMAKE_INSTALL_RPATH="$HOME/deps/lib;$HOME/swiftshader/lib" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DMAGNUM_TARGET_GLES=ON \
    -DMAGNUM_TARGET_GLES2=$TARGET_GLES2 \
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
    -G Ninja
ninja install
cd ../..

# Magnum Plugins, required by ImGuiIntegration tests
git clone --depth 1 https://github.com/mosra/magnum-plugins.git
cd magnum-plugins
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=Debug \
    -DMAGNUM_WITH_STBIMAGEIMPORTER=ON \
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
    -DMAGNUM_WITH_BULLET=ON \
    -DMAGNUM_WITH_DART=OFF \
    -DMAGNUM_WITH_EIGEN=ON \
    -DMAGNUM_WITH_GLM=ON \
    -DMAGNUM_WITH_IMGUI=ON \
    -DMAGNUM_WITH_OVR=OFF \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    -G Ninja
ninja $NINJA_JOBS

CORRADE_TEST_COLOR=ON ctest -V
if [ "$TARGET_GLES2" == "ON" ]; then CORRADE_TEST_COLOR=ON MAGNUM_DISABLE_EXTENSIONS="OES_vertex_array_object" ctest -V -R GLTest; fi

# Test install, after running the tests as for them it shouldn't be needed
ninja install
