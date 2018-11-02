# kate: indent-width 2;

class MagnumIntegration < Formula
  desc "Integration libraries for the Magnum C++11/C++14 graphics engine"
  homepage "https://magnum.graphics"
  url "https://github.com/mosra/magnum-integration/archive/v2018.10.tar.gz"
  sha256 "f999800faf5fe0f24f5de7ad42fc11f3371c377e95a9c7b667c45bf264b3ceaf"
  head "git://github.com/mosra/magnum-integration.git"

  depends_on "cmake"
  depends_on "glm"
  depends_on "magnum"
  depends_on "bullet"

  def install
    system "mkdir build"
    cd "build" do
      system "cmake", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_INSTALL_PREFIX=#{prefix}", "-DWITH_BULLET=ON", "-DWITH_DART=OFF", "-DWITH_GLM=ON", ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
