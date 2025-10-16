#!/bin/bash
set -ev

git submodule update --init

# Crosscompile Corrade
git clone --depth 1 https://github.com/mosra/corrade.git
cd corrade
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="../../toolchains/generic/Emscripten-wasm.cmake" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    $EXTRA_OPTS \
    -G Ninja
ninja install
cd ../..

# Crosscompile Bullet
wget https://github.com/bulletphysics/bullet3/archive/2.87.tar.gz
tar -xzf 2.87.tar.gz && cd bullet3-2.87
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="../../toolchains/generic/Emscripten-wasm.cmake" \
    -DCMAKE_BUILD_TYPE=Debug \
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
    $EXTRA_OPTS \
    -G Ninja
ninja install
cd ../..

# Crosscompile Magnum
git clone --depth 1 https://github.com/mosra/magnum.git
cd magnum
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="../../toolchains/generic/Emscripten-wasm.cmake" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DMAGNUM_WITH_AUDIO=OFF \
    -DMAGNUM_WITH_DEBUGTOOLS=ON \
    -DMAGNUM_WITH_MATERIALTOOLS=OFF \
    `# MeshTools and Primitives are needed for DebugTools if SceneGraph is` \
    `# enabled as well (which is needed by BulletIntegration)` \
    -DMAGNUM_WITH_MESHTOOLS=ON \
    -DMAGNUM_WITH_PRIMITIVES=ON \
    -DMAGNUM_WITH_SCENEGRAPH=ON \
    -DMAGNUM_WITH_SCENETOOLS=OFF \
    -DMAGNUM_WITH_SHADERS=ON \
    -DMAGNUM_WITH_TEXT=OFF \
    -DMAGNUM_WITH_TEXTURETOOLS=OFF \
    -DMAGNUM_WITH_OPENGLTESTER=ON \
    -DMAGNUM_WITH_EMSCRIPTENAPPLICATION=ON \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_TARGET_GLES2=$TARGET_GLES2 \
    $EXTRA_OPTS \
    -G Ninja
ninja install
cd ../..

# ImGuiIntegration tests use Magnum Plugins, but only for GL tests, which we
# don't run here

# Magnum Extras, required for YogaIntegration (which depends on Ui, which is
# ES3-only)
if [[ "$TARGET_GLES3" == "ON" ]]; then
    git clone --depth 1 https://github.com/mosra/magnum-extras.git
    cd magnum-extras
    mkdir build-emscripten && cd build-emscripten
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE="../../toolchains/generic/Emscripten-wasm.cmake" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps \
        -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
        -DMAGNUM_WITH_UI=ON \
        $EXTRA_OPTS \
        -G Ninja
    ninja install
    cd ../..
fi

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
    -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/Emscripten-wasm.cmake" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_MODULE_PATH=$HOME/eigen/cmake/ \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DEIGEN3_INCLUDE_DIR=$HOME/eigen/ \
    -DGLM_INCLUDE_DIR=$HOME/glm \
    -DIMGUI_DIR=$HOME/imgui \
    -DMAGNUM_WITH_BULLETINTEGRATION=ON \
    -DMAGNUM_WITH_DARTINTEGRATION=OFF \
    -DMAGNUM_WITH_EIGENINTEGRATION=ON \
    -DMAGNUM_WITH_GLMINTEGRATION=ON \
    -DMAGNUM_WITH_IMGUIINTEGRATION=ON \
    -DMAGNUM_WITH_OVRINTEGRATION=OFF \
    -DMAGNUM_WITH_YOGAINTEGRATION=$TARGET_GLES3 \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    $EXTRA_OPTS \
    -G Ninja
ninja $NINJA_JOBS

# Test
CORRADE_TEST_COLOR=ON ctest -V -E GLTest

# Test install, after running the tests as for them it shouldn't be needed
ninja install
