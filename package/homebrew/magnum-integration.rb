class MagnumIntegration < Formula
  desc "Integration libraries for the Magnum C++11 graphics engine"
  homepage "https://magnum.graphics"
  # git describe origin/master, except the `v` prefix
  version "2020.06-306-g30d179f3"
  # Clone instead of getting an archive to have tags for version.h generation
  url "https://github.com/mosra/magnum-integration.git", revision: "30d179f3"
  head "https://github.com/mosra/magnum-integration.git"

  depends_on "cmake" => :build
  depends_on "eigen" => :recommended
  depends_on "glm" => :recommended
  depends_on "magnum"
  depends_on "bullet" => :recommended
  depends_on "dartsim" => :optional

  def install
    # Bundle ImGui
    system "curl", "-L", "https://github.com/ocornut/imgui/archive/v1.88.tar.gz", "-o", "src/MagnumExternal/imgui.tar.gz"
    cd "src/MagnumExternal" do
      system "mkdir", "ImGui"
      system "tar", "xzvf", "imgui.tar.gz", "-C", "ImGui", "--strip-components=1"
    end

    system "mkdir build"
    cd "build" do
      system "cmake",
        *std_cmake_args,
        # Without this, ARM builds will try to look for dependencies in
        # /usr/local/lib and /usr/lib (which are the default locations) instead
        # of /opt/homebrew/lib which is dedicated for ARM binaries. Please
        # complain to Homebrew about this insane non-obvious filesystem layout.
        "-DCMAKE_INSTALL_NAME_DIR:STRING=#{lib}",
        "-DMAGNUM_WITH_BULLET=#{(build.with? 'bullet') ? 'ON' : 'OFF'}",
        "-DMAGNUM_WITH_DART=#{(build.with? 'dartsim') ? 'ON' : 'OFF'}",
        "-DMAGNUM_WITH_EIGEN=#{(build.with? 'eigen') ? 'ON' : 'OFF'}",
        "-DMAGNUM_WITH_GLM=#{(build.with? 'glm') ? 'ON' : 'OFF'}",
        "-DMAGNUM_WITH_IMGUI=ON",
        ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
