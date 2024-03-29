EAPI=5

EGIT_REPO_URI="https://github.com/mosra/magnum-integration.git"

inherit cmake-utils git-r3

DESCRIPTION="Integration libraries for the Magnum C++11/C++14 graphics engine"
HOMEPAGE="https://magnum.graphics"

LICENSE="MIT"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="
	dev-cpp/eigen
	dev-libs/magnum
	media-libs/glm
	sci-physics/bullet
"
DEPEND="${RDEPEND}"

src_configure() {
	# general configuration
	local mycmakeargs=(
		-DCMAKE_INSTALL_PREFIX="${EPREFIX}/usr"
		-DCMAKE_BUILD_TYPE=Release
		-DMAGNUM_WITH_BULLET=ON
		-DMAGNUM_WITH_DART=OFF
		-DMAGNUM_WITH_EIGEN=ON
		-DMAGNUM_WITH_GLM=ON
		-DMAGNUM_WITH_IMGUI=OFF
	)
	cmake-utils_src_configure
}

# kate: replace-tabs off;
