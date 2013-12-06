# Author: mosra <mosra@centrum.cz>
pkgname=magnum-integration
pkgver=dev
pkgrel=1
pkgdesc="Integration library for Magnum OpenGL 3 graphics engine"
arch=('i686' 'x86_64')
url="https://github.com/magnum-integration"
license=('MIT')
depends=('magnum')
makedepends=('cmake')
options=(!strip)
provides=('magnum-integration-git')

build() {
    mkdir -p "$startdir/build"
    cd "$startdir/build/"

    cmake .. \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DBUILD_TESTS=TRUE
    make
}

check() {
    cd "$startdir/build"
    ctest --output-on-failure
}

package() {
  cd "$startdir/build"
  make DESTDIR="$pkgdir/" install
}
