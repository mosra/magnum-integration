# kate: indent-width 2;

class MagnumIntegration < Formula
  desc "Integration libraries for the Magnum C++11/C++14 graphics engine"
  homepage "http://magnum.graphics"
  url "https://github.com/mosra/magnum-integration/archive/v2018.02.tar.gz"
  sha256 "e7ec9feb856e6cf9abf1da7cefdb66e724c560619197c548a97248feddbb8714"
  head "git://github.com/mosra/magnum-integration.git"

  depends_on "cmake"
  depends_on "magnum"
  depends_on "bullet"

  def install
    system "mkdir build"
    cd "build" do
      system "cmake", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_INSTALL_PREFIX=#{prefix}", "-DWITH_BULLET=ON", "-DWITH_DART=OFF", ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
