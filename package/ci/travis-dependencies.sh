#!/bin/bash
set -ev

if [ "$TRAVIS_OS_NAME" == "linux" ] && [ ! -d "$HOME/deps-dart/include" ]; then
    # libccd
    wget https://github.com/danfis/libccd/archive/v2.0.tar.gz
    tar -xzf v2.0.tar.gz
    cd libccd-2.0
    mkdir build && cd build
    cmake .. \
        -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps-dart \
        -DCMAKE_INSTALL_RPATH=$HOME/deps-dart/lib \
        -DCMAKE_BUILD_TYPE=Debug \
        -DBUILD_TESTING=OFF \
        -G Ninja
    ninja install
    cd ../..

    # fcl
    wget https://github.com/flexible-collision-library/fcl/archive/0.5.0.tar.gz
    tar -xzf 0.5.0.tar.gz
    cd fcl-0.5.0
    mkdir build && cd build
    cmake .. \
        -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps-dart \
        -DCMAKE_INSTALL_RPATH=$HOME/deps-dart/lib \
        -DCMAKE_BUILD_TYPE=Debug \
        -DFCL_BUILD_TESTS=OFF \
        -G Ninja
    # Otherwise the job gets killed (probably because using too much memory)
    ninja -j4 install
    cd ../..

    # DART
    wget https://github.com/dartsim/dart/archive/v6.3.0.tar.gz
    tar -xzf v6.3.0.tar.gz
    cd dart-6.3.0
    mkdir build && cd build
    cmake .. \
        -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
        -DCMAKE_INSTALL_PREFIX=$HOME/deps-dart \
        -DCMAKE_INSTALL_RPATH=$HOME/deps-dart/lib \
        -DCMAKE_BUILD_TYPE=Debug \
        -DDART_BUILD_GUI_OSG=OFF \
        -G Ninja
    # Otherwise the job gets killed (probably because using too much memory)
    ninja -j4 install
    cd ../..
fi
