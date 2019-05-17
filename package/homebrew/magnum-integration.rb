class MagnumIntegration < Formula
  desc "Integration libraries for the Magnum C++11/C++14 graphics engine"
  homepage "https://magnum.graphics"
  url "https://github.com/mosra/magnum-integration/archive/v2019.01.tar.gz"
  # wget https://github.com/mosra/magnum-integration/archive/v2019.01.tar.gz -O - | sha256sum
  sha256 "a1de0eeb4b9ff58a1d4320e5632a4c84f9e54bc554c22aba334cafa1e7c7e868"
  head "git://github.com/mosra/magnum-integration.git"

  depends_on "cmake"
  depends_on "eigen"
  depends_on "glm"
  depends_on "magnum"
  depends_on "bullet"
  depends_on "dartsim"

  def install
    system "mkdir build"
    cd "build" do
      system "cmake", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_INSTALL_PREFIX=#{prefix}", "-DWITH_BULLET=ON", "-DWITH_DART=ON", "-DWITH_EIGEN=ON", "-DWITH_GLM=ON", "-DWITH_IMGUI=OFF", ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
