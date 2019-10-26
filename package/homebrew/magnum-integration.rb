class MagnumIntegration < Formula
  desc "Integration libraries for the Magnum C++11/C++14 graphics engine"
  homepage "https://magnum.graphics"
  url "https://github.com/mosra/magnum-integration/archive/v2019.10.tar.gz"
  # wget https://github.com/mosra/magnum-integration/archive/v2019.10.tar.gz -O - | sha256sum
  sha256 "c96a8237878ae7d74a8a4331122f96a50374d2eee6aed8f15bb039321d66fd48"
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
