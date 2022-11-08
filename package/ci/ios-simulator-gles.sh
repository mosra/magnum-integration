#!/bin/bash
set -ev

git submodule update --init

# Corrade
git clone --depth 1 https://github.com/mosra/corrade.git
cd corrade

# Build native corrade-rc
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps-native \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    -DCORRADE_WITH_PLUGINMANAGER=OFF \
    -DCORRADE_WITH_TESTSUITE=OFF \
    -DCORRADE_WITH_UTILITY=OFF \
    -G Ninja
ninja install
cd ..

# Crosscompile Corrade
mkdir build-ios && cd build-ios
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../../toolchains/generic/iOS.cmake \
    -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCORRADE_BUILD_STATIC=ON \
    -DCORRADE_TESTSUITE_TARGET_XCTEST=ON \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    -DCORRADE_BUILD_STATIC=ON \
    -G Xcode
set -o pipefail && cmake --build . --config Release --target install -j$XCODE_JOBS | xcbeautify
cd ../..

# Crosscompile SDL. On 2022-14-02 curl says the certificate is expired, so
# ignore that.
# TODO use a CMake build instead
curl --insecure -O https://www.libsdl.org/release/SDL2-2.0.10.tar.gz
tar -xzvf SDL2-2.0.10.tar.gz
cd SDL2-2.0.10/Xcode-iOS/SDL
set -o pipefail && xcodebuild -sdk iphonesimulator13.7 -jobs $XCODE_JOBS -parallelizeTargets | xcbeautify
cp build/Release-iphonesimulator/libSDL2.a $HOME/deps/lib
mkdir -p $HOME/deps/include/SDL2
cp -R ../../include/* $HOME/deps/include/SDL2
cd ../../..

# Crosscompile Magnum
git clone --depth 1 https://github.com/mosra/magnum.git
cd magnum
mkdir build-ios && cd build-ios
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../../toolchains/generic/iOS.cmake \
    -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DMAGNUM_WITH_AUDIO=OFF \
    -DMAGNUM_WITH_DEBUGTOOLS=ON \
    -DMAGNUM_WITH_MATERIALTOOLS=OFF \
    -DMAGNUM_WITH_MESHTOOLS=ON \
    -DMAGNUM_WITH_PRIMITIVES=ON \
    -DMAGNUM_WITH_SCENEGRAPH=OFF \
    -DMAGNUM_WITH_SCENETOOLS=OFF \
    -DMAGNUM_WITH_SHADERS=ON \
    -DMAGNUM_WITH_SHADERTOOLS=OFF \
    -DMAGNUM_WITH_TEXT=OFF \
    -DMAGNUM_WITH_TEXTURETOOLS=OFF \
    -DMAGNUM_WITH_OPENGLTESTER=ON \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_TARGET_GLES2=$TARGET_GLES2 \
    -DMAGNUM_BUILD_STATIC=ON \
    -G Xcode
set -o pipefail && cmake --build . --config Release --target install -j$XCODE_JOBS | xcbeautify
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
mkdir build-ios && cd build-ios
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchains/generic/iOS.cmake \
    -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DCMAKE_MODULE_PATH=$HOME/eigen/cmake/ \
    -DCMAKE_PREFIX_PATH=$HOME/deps \
    -DEIGEN3_INCLUDE_DIR=$HOME/eigen/ \
    -DGLM_INCLUDE_DIR=$HOME/glm \
    -DIMGUI_DIR=$HOME/imgui \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DMAGNUM_WITH_BULLET=OFF \
    -DMAGNUM_WITH_DART=OFF \
    -DMAGNUM_WITH_EIGEN=ON \
    -DMAGNUM_WITH_GLM=ON \
    -DMAGNUM_WITH_IMGUI=ON \
    -DMAGNUM_WITH_OVR=OFF \
    -DMAGNUM_BUILD_STATIC=ON \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    -G Xcode
set -o pipefail && cmake --build . --config Release -j$XCODE_JOBS | xcbeautify
# TODO: find a better way to avoid
# Library not loaded: /System/Library/Frameworks/OpenGLES.framework/OpenGLES
# error
DYLD_FALLBACK_LIBRARY_PATH="/Library/Developer/CoreSimulator/Profiles/Runtimes/iOS 12.4.simruntime/Contents/Resources/RuntimeRoot/System/Library/Frameworks/OpenGLES.framework" DYLD_FALLBACK_FRAMEWORK_PATH="/Library/Developer/CoreSimulator/Profiles/Runtimes/iOS 12.4.simruntime/Contents/Resources/RuntimeRoot/System/Library/Frameworks" CORRADE_TEST_COLOR=ON ctest -V -C Release -E GLTest

# Test install, after running the tests as for them it shouldn't be needed
set -o pipefail && cmake --build . --config Release --target install -j$XCODE_JOBS | xcbeautify
