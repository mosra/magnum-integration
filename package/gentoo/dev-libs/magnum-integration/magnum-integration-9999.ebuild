EAPI=5

EGIT_REPO_URI="git://github.com/mosra/magnum-integration.git"

inherit cmake-utils git-r3

DESCRIPTION="Integration libraries for the Magnum C++11/C++14 graphics engine"
HOMEPAGE="http://magnum.graphics"

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
		-DCMAKE_BUILD_TYPE=Release
		-DWITH_BULLET=ON
		-DWITH_DART=OFF
	)
	cmake-utils_src_configure
}

# kate: replace-tabs off;
