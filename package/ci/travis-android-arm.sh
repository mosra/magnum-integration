#!/bin/bash
set -ev

# Corrade
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
mkdir build-android-arm && cd build-android-arm
cmake .. \
    -DCMAKE_ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r16b \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=22 \
    -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCMAKE_BUILD_TYPE=Release \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DWITH_INTERCONNECT=OFF \
    -G Ninja
ninja install
cd ../..

# Crosscompile Magnum
git clone --depth 1 git://github.com/mosra/magnum.git
cd magnum
mkdir build-android-arm && cd build-android-arm
cmake .. \
    -DCMAKE_ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r16b \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=22 \
    -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCMAKE_BUILD_TYPE=Release \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DWITH_AUDIO=OFF \
    -DWITH_ANDROIDAPPLICATION=ON \
    -DWITH_DEBUGTOOLS=OFF \
    -DWITH_MESHTOOLS=OFF \
    -DWITH_PRIMITIVES=OFF \
    -DWITH_SCENEGRAPH=OFF \
    -DWITH_SHADERS=ON \
    -DWITH_TEXT=OFF \
    -DWITH_TEXTURETOOLS=OFF \
    -DWITH_OPENGLTESTER=ON \
    -DTARGET_GLES2=$TARGET_GLES2 \
    -G Ninja
ninja install
cd ../..

# Generate debug keystore for APK signing
keytool -genkeypair -keystore $HOME/.android/debug.keystore -storepass android -alias androiddebugkey -keypass android -keyalg RSA -validity 10000 -dname CN=,OU=,O=,L=,S=,C=

# Crosscompile. There's extra crazy stuff for Eigen3. It's header-only but the
# archive is so stupid that it's not possible to just use Eigen3Config.cmake,
# as it's generated using CMake from Eigen3Config.cmake.in. There's
# FindEigen3.cmake next to it, but that doesn't help with ANYTHING AT ALL
# (like, what about looking one directory up, eh?! too hard?!) and also defines
# just EIGEN3_INCLUDE_DIR, not the Eigen3::Eigen target nor
# EIGEN3_INCLUDE_DIRS. Now I get why people hate CMake. It's because project
# maintainers are absolutely clueless on how to write usable find scripts with
# it.
mkdir build-android-arm && cd build-android-arm
cmake .. \
    -DANDROID_SDK=/usr/local/android-sdk \
    -DCMAKE_ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r16b \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=22 \
    -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCMAKE_BUILD_TYPE=Release \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DCMAKE_MODULE_PATH=$HOME/eigen/cmake/ \
    -DEIGEN3_INCLUDE_DIR=$HOME/eigen/ \
    -DGLM_INCLUDE_DIR=$HOME/glm \
    -DIMGUI_DIR=$HOME/imgui \
    -DWITH_BULLET=OFF \
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

# Start simulator and run tests
echo no | android create avd --force -n test -t android-22 --abi armeabi-v7a
emulator -avd test -no-audio -no-window &
android-wait-for-emulator
CORRADE_TEST_COLOR=ON ctest -V -E GLTest
