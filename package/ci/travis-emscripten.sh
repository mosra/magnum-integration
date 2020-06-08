#!/bin/bash
set -ev

git submodule update --init

git clone --depth 1 git://github.com/mosra/corrade.git
cd corrade

# Build native corrade-rc
mkdir build && cd build || exit /b
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps-native \
    -DWITH_INTERCONNECT=OFF \
    -DWITH_PLUGINMANAGER=OFF \
    -DWITH_TESTSUITE=OFF \
    -DWITH_UTILITY=OFF \
    -G Ninja
ninja install
cd ..

# Crosscompile Corrade
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_TOOLCHAIN_FILE="../../toolchains/generic/Emscripten.cmake" \
    -DEMSCRIPTEN_PREFIX=$(echo /usr/local/Cellar/emscripten/*/libexec) \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS_RELEASE="-DNDEBUG -O1" \
    -DCMAKE_EXE_LINKER_FLAGS_RELEASE="-O1" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DWITH_INTERCONNECT=OFF \
    -G Ninja
ninja install
cd ../..

# Crosscompile Bullet
wget https://github.com/bulletphysics/bullet3/archive/2.87.tar.gz
tar -xzf 2.87.tar.gz && cd bullet3-2.87
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="../../toolchains/generic/Emscripten.cmake" \
    -DEMSCRIPTEN_PREFIX=$(echo /usr/local/Cellar/emscripten/*/libexec) \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS_RELEASE="-DNDEBUG -O1" \
    -DCMAKE_EXE_LINKER_FLAGS_RELEASE="-O1" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DBUILD_BULLET2_DEMOS=OFF \
    -DBUILD_BULLET3=OFF \
    -DBUILD_CLSOCKET=OFF \
    -DBUILD_CPU_DEMOS=OFF \
    -DBUILD_ENET=OFF \
    -DBUILD_EXTRAS=OFF \
    -DBUILD_OPENGL3_DEMOS=OFF \
    -DBUILD_PYBULLET=OFF \
    -DBUILD_UNIT_TESTS=OFF \
    -DINSTALL_LIBS=ON \
    -DINSTALL_CMAKE_FILES=OFF \
    -DUSE_GLUT=OFF \
    -DUSE_GRAPHICAL_BENCHMARK=OFF \
    -D_FIND_LIB_PYTHON_PY=$TRAVIS_BUILD_DIR/bullet3-2.87/build3/cmake/FindLibPython.py \
    -G Ninja
ninja install
cd ../..

# Crosscompile Magnum
git clone --depth 1 git://github.com/mosra/magnum.git
cd magnum
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_TOOLCHAIN_FILE="../../toolchains/generic/Emscripten.cmake" \
    -DEMSCRIPTEN_PREFIX=$(echo /usr/local/Cellar/emscripten/*/libexec) \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS_RELEASE="-DNDEBUG -O1" \
    -DCMAKE_EXE_LINKER_FLAGS_RELEASE="-O1" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DWITH_AUDIO=OFF \
    -DWITH_DEBUGTOOLS=OFF \
    -DWITH_MESHTOOLS=OFF \
    -DWITH_PRIMITIVES=OFF \
    -DWITH_SCENEGRAPH=ON \
    -DWITH_SHADERS=ON \
    -DWITH_TEXT=OFF \
    -DWITH_TEXTURETOOLS=OFF \
    -DWITH_EMSCRIPTENAPPLICATION=ON \
    -DTARGET_GLES2=$TARGET_GLES2 \
    -G Ninja
ninja install
cd ../..

# Crosscompile. There's extra crazy stuff for Eigen3. It's header-only but the
# archive is so stupid that it's not possible to just use Eigen3Config.cmake,
# as it's generated using CMake from Eigen3Config.cmake.in. There's
# FindEigen3.cmake next to it, but that doesn't help with ANYTHING AT ALL
# (like, what about looking one directory up, eh?! too hard?!) and also defines
# just EIGEN3_INCLUDE_DIR, not the Eigen3::Eigen target nor
# EIGEN3_INCLUDE_DIRS. Now I get why people hate CMake. It's because project
# maintainers are absolutely clueless on how to write usable find scripts with
# it.
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/Emscripten.cmake" \
    -DEMSCRIPTEN_PREFIX=$(echo /usr/local/Cellar/emscripten/*/libexec) \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS_RELEASE="-DNDEBUG -O1" \
    -DCMAKE_EXE_LINKER_FLAGS_RELEASE="-O1" \
    -DCMAKE_MODULE_PATH=$HOME/eigen/cmake/ \
    -DEIGEN3_INCLUDE_DIR=$HOME/eigen/ \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DGLM_INCLUDE_DIR=$HOME/glm \
    -DIMGUI_DIR=$HOME/imgui \
    -DWITH_BULLET=ON \
    -DWITH_DART=OFF \
    -DWITH_EIGEN=ON \
    -DWITH_GLM=ON \
    -DWITH_IMGUI=ON \
    -DWITH_OVR=OFF \
    -DBUILD_TESTS=ON \
    -G Ninja
# Otherwise the job gets killed (probably because using too much memory)
ninja -j4

# Test
CORRADE_TEST_COLOR=ON ctest -V
