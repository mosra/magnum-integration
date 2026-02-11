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
set -o pipefail && xcodebuild -sdk iphonesimulator -jobs $XCODE_JOBS -parallelizeTargets | xcbeautify
cp build/Release-iphonesimulator/libSDL2.a $HOME/deps/lib
mkdir -p $HOME/deps/include/SDL2
cp -R ../../include/* $HOME/deps/include/SDL2
cd ../../..

# Crosscompile Yoga
mkdir -p yoga && cd yoga
# Without the LOL it downloads a zero-byte file and returns success, and then
# tar also returns success, only the echo after fails. LOL, geez, what the
# fuck.
curl -LOL https://github.com/facebook/yoga/archive/refs/tags/v2.0.1.tar.gz
tar --strip-components=1 -xzf v2.0.1.tar.gz
# Exclude tests (which cause the whole of Google test installed, ffs!!) by
# making the CMakeLists empty
echo > tests/CMakeLists.txt
mkdir -p build && cd build
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../../toolchains/generic/iOS.cmake \
    -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -G Xcode
set -o pipefail && cmake --build . --config Release --target install -j$XCODE_JOBS | xcbeautify
cd ../..

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
    -DMAGNUM_WITH_MESHTOOLS=OFF \
    -DMAGNUM_WITH_PRIMITIVES=OFF \
    -DMAGNUM_WITH_SCENEGRAPH=OFF \
    -DMAGNUM_WITH_SCENETOOLS=OFF \
    -DMAGNUM_WITH_SHADERS=ON \
    -DMAGNUM_WITH_SHADERTOOLS=OFF \
    -DMAGNUM_WITH_TEXT=ON \
    -DMAGNUM_WITH_TEXTURETOOLS=ON \
    -DMAGNUM_WITH_OPENGLTESTER=ON \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_TARGET_GLES2=$TARGET_GLES2 \
    -DMAGNUM_BUILD_STATIC=ON \
    -G Xcode
set -o pipefail && cmake --build . --config Release --target install -j$XCODE_JOBS | xcbeautify
cd ../..

# Crosscompile Magnum Extras, which are needed by YogaIntegration
git clone --depth 1 https://github.com/mosra/magnum-extras.git
cd magnum-extras
mkdir build-ios && cd build-ios
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../../toolchains/generic/iOS.cmake \
    -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DMAGNUM_WITH_UI=ON \
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
    `# Make libc++ remove transitive includes, both for faster build times` \
    `# and to detect if we're missing a transitive include. Works with` \
    `# libc++ 16+, which is used by Xcode 15 (i.e., will get used on the` \
    `# next CircleCI executor update). Not doing this for the Corrade,` \
    `# Magnum etc. builds, as that'd only add extra friction with no` \
    `# possibility of fixing from here.` \
    -DCMAKE_CXX_FLAGS="-D_LIBCPP_REMOVE_TRANSITIVE_INCLUDES" \
    -DEIGEN3_INCLUDE_DIR=$HOME/eigen/ \
    -DGLM_INCLUDE_DIR=$HOME/glm \
    -DIMGUI_DIR=$HOME/imgui \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DMAGNUM_WITH_BULLETINTEGRATION=OFF \
    -DMAGNUM_WITH_DARTINTEGRATION=OFF \
    -DMAGNUM_WITH_EIGENINTEGRATION=ON \
    -DMAGNUM_WITH_GLMINTEGRATION=ON \
    -DMAGNUM_WITH_IMGUIINTEGRATION=ON \
    -DMAGNUM_WITH_OVRINTEGRATION=OFF \
    -DMAGNUM_WITH_YOGAINTEGRATION=ON \
    -DMAGNUM_BUILD_STATIC=ON \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    -G Xcode
set -o pipefail && cmake --build . --config Release -j$XCODE_JOBS | xcbeautify

# TODO enable again once https://github.com/mosra/corrade/pull/176 is resolved
# TODO: find a better way to avoid
# Library not loaded: /System/Library/Frameworks/OpenGLES.framework/OpenGLES
# error
# DYLD_FALLBACK_LIBRARY_PATH="/Library/Developer/CoreSimulator/Profiles/Runtimes/iOS 12.4.simruntime/Contents/Resources/RuntimeRoot/System/Library/Frameworks/OpenGLES.framework" DYLD_FALLBACK_FRAMEWORK_PATH="/Library/Developer/CoreSimulator/Profiles/Runtimes/iOS 12.4.simruntime/Contents/Resources/RuntimeRoot/System/Library/Frameworks" CORRADE_TEST_COLOR=ON ctest -V -C Release -E GLTest

# Test install, after running the tests as for them it shouldn't be needed
set -o pipefail && cmake --build . --config Release --target install -j$XCODE_JOBS | xcbeautify
