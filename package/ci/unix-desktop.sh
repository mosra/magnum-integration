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
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DMAGNUM_TARGET_EGL=$TARGET_EGL \
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
    -DMAGNUM_WITH_SHADERTOOLS=OFF \
    -DMAGNUM_WITH_TEXT=OFF \
    -DMAGNUM_WITH_TEXTURETOOLS=OFF \
    -DMAGNUM_WITH_OPENGLTESTER=ON \
    -DMAGNUM_WITH_ANYIMAGEIMPORTER=ON \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_WITH_GLFWAPPLICATION=ON \
    -DMAGNUM_BUILD_DEPRECATED=$BUILD_DEPRECATED \
    -G Ninja
ninja install
cd ../..

# DartIntegration needs some plugins, GL tests (thus, targeting EGL) need some
# other
git clone --depth 1 https://github.com/mosra/magnum-plugins.git
cd magnum-plugins
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DMAGNUM_WITH_ASSIMPIMPORTER=$WITH_DART \
    `# Needed by either DartIntegration or GL tests, so enabling always` \
    -DMAGNUM_WITH_STBIMAGEIMPORTER=ON \
    `# StbImageConverter so we can easier debug rendering failures on the CI` \
    -DMAGNUM_WITH_STBIMAGECONVERTER=$TARGET_EGL \
    -G Ninja
ninja install
cd ../..

mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_PREFIX_PATH="$HOME/deps-dart" \
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

export CORRADE_TEST_COLOR=ON
# Sanitizer options are used only in sanitizer builds, they do nothing
# elsewhere
export ASAN_OPTIONS="color=always"
export LSAN_OPTIONS="color=always"

# Run GL tests if we can use llvmpipe through EGL. (Not benchmarks because I'm
# not interested in knowing speed of a random software GPU emulation, further
# offset by inherent randomness of a CI VM.) Enabled only on some builds to
# make sure the MAGNUM_TARGET_EGL=OFF option is at least compile-tested too.
if [ "$TARGET_EGL" == "ON" ]; then
    # Keep in sync with package/archlinux/PKGBUILD and PKGBUILD-coverage
    ctest -V -E "GLBenchmark|Dart"
    MAGNUM_DISABLE_EXTENSIONS="GL_ARB_draw_elements_base_vertex" ctest -V -R GLTest -E Dart
    # DART leaks somewhere deep in std::string, run these tests separately to
    # avoid suppressing too much
    LSAN_OPTIONS="color=always suppressions=$(pwd)/../package/ci/leaksanitizer.conf" ctest -V -R "Dart.+GLTest"
    MAGNUM_DISABLE_EXTENSIONS="GL_ARB_draw_elements_base_vertex" LSAN_OPTIONS="color=always suppressions=$(pwd)/../package/ci/leaksanitizer.conf" ctest -V -R "Dart.+GLTest"
else
    ctest -V -E "GLTest|GLBenchmark|Dart"
    # DART leaks somewhere deep in std::string, run these tests separately to
    # avoid suppressing too much
    LSAN_OPTIONS="color=always suppressions=$(pwd)/../package/ci/leaksanitizer.conf" ctest -V -R Dart -E "GLTest|GLBenchmark"
fi

# Test install, after running the tests as for them it shouldn't be needed
ninja install
