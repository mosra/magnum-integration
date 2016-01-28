# kate: indent-width 2;

class MagnumIntegration < Formula
  desc "Integration libraries for Magnum graphics engine"
  homepage "https://github.com/mosra/magnum"
  head "git://github.com/mosra/magnum-integration.git"

  depends_on "cmake"
  depends_on "magnum"
  depends_on "bullet"

  def install
    system "mkdir build"
    cd "build" do
      system "cmake", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_INSTALL_PREFIX=#{prefix}", "-DWITH_BULLET=ON", ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
