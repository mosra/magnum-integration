#!/bin/bash
set -ev

git submodule update --init

# Corrade
git clone --depth 1 git://github.com/mosra/corrade.git
cd corrade

# Build native corrade-rc
mkdir build && cd build || exit /b
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps-native \
    -DCMAKE_INSTALL_RPATH=$HOME/deps-native/lib \
    -DWITH_INTERCONNECT=OFF \
    -DWITH_PLUGINMANAGER=OFF \
    -DWITH_TESTSUITE=OFF \
    -G Ninja
ninja install
cd ..

# Crosscompile Corrade
mkdir build-android-arm && cd build-android-arm
ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r10e cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchains/generic/Android-ARM.cmake \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DWITH_INTERCONNECT=OFF \
    -G Ninja
ninja install
cd ../..

# Crosscompile Magnum
git clone --depth 1 git://github.com/mosra/magnum.git
cd magnum
mkdir build-android-arm && cd build-android-arm
ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r10e cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchains/generic/Android-ARM.cmake \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DWITH_AUDIO=OFF \
    -DWITH_DEBUGTOOLS=OFF \
    -DWITH_MESHTOOLS=OFF \
    -DWITH_PRIMITIVES=OFF \
    -DWITH_SCENEGRAPH=OFF \
    -DWITH_SHADERS=OFF \
    -DWITH_SHAPES=OFF \
    -DWITH_TEXT=OFF \
    -DWITH_TEXTURETOOLS=OFF \
    -DTARGET_GLES2=$TARGET_GLES2 \
    -G Ninja
ninja install
cd ../..

# Crosscompile
mkdir build-android-arm && cd build-android-arm
ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r10e cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchains/generic/Android-ARM.cmake \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DWITH_BULLET=OFF \
    -DWITH_DART=OFF \
    -DWITH_OVR=OFF \
    -DBUILD_TESTS=ON \
    -G Ninja
# Otherwise the job gets killed (probably because using too much memory)
ninja -j4

# Start simulator and run tests
echo no | android create avd --force -n test -t android-19 --abi armeabi-v7a
emulator -avd test -no-audio -no-window &
android-wait-for-emulator
CORRADE_TEST_COLOR=ON ctest -V
