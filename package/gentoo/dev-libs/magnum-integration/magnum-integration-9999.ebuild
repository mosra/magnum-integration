# Copyright 1999-2014 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=5

EGIT_REPO_URI="git://github.com/mosra/magnum-integration.git"

inherit cmake-utils git-r3

DESCRIPTION="Integration libraries for Magnum OpenGL graphics engine"
HOMEPAGE="http://mosra.cz/blog/magnum.php"

LICENSE="MIT"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="
	dev-libs/magnum
	sci-physics/bullet
"
DEPEND="${RDEPEND}"

src_configure() {
	# general configuration
	local mycmakeargs=(
		-DCMAKE_INSTALL_PREFIX="${EPREFIX}/usr"
		-DCMAKE_BUILD_TYPE=Debug \
		-DWITH_BULLET=ON
    )
	cmake-utils_src_configure
}
