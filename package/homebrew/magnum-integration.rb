class MagnumIntegration < Formula
  desc "Integration libraries for the Magnum C++11/C++14 graphics engine"
  homepage "https://magnum.graphics"
  url "https://github.com/mosra/magnum-integration/archive/v2020.06.tar.gz"
  # wget https://github.com/mosra/magnum-integration/archive/v2020.06.tar.gz -O - | sha256sum
  sha256 "4eb461e0a38d7be69a52b8faf7664493da4e4cabc2c4fa86bd672d2e8f0a9311"
  head "https://github.com/mosra/magnum-integration.git"

  depends_on "cmake" => :build
  depends_on "eigen" => :recommended
  depends_on "glm" => :recommended
  depends_on "magnum"
  depends_on "bullet" => :recommended
  depends_on "dartsim" => :optional

  def install
    # Bundle ImGui
    imgui_tag = build.head? ? 'v1.88' : 'v1.77'
    system "curl", "-L", "https://github.com/ocornut/imgui/archive/#{imgui_tag}.tar.gz", "-o", "src/MagnumExternal/imgui.tar.gz"
    cd "src/MagnumExternal" do
      system "mkdir", "ImGui"
      system "tar", "xzvf", "imgui.tar.gz", "-C", "ImGui", "--strip-components=1"
    end

    # 2020.06 has the options unprefixed, current master has them prefixed.
    # Options not present in 2020.06 are prefixed always.
    option_prefix = build.head? ? 'MAGNUM_' : ''
    # 2020.06 has CMake 3.5 as minimum required for backwards compatibility
    # purposes, but it works with any newer. CMake 4.0 removed compatibility
    # with it and suggests this as an override.
    # TODO remove once a new release is finally made
    extra_cmake_args = build.head? ? [] : ['-DCMAKE_POLICY_VERSION_MINIMUM=3.5']

    system "mkdir build"
    cd "build" do
      system "cmake",
        *(std_cmake_args + extra_cmake_args),
        # Without this, ARM builds will try to look for dependencies in
        # /usr/local/lib and /usr/lib (which are the default locations) instead
        # of /opt/homebrew/lib which is dedicated for ARM binaries. Please
        # complain to Homebrew about this insane non-obvious filesystem layout.
        "-DCMAKE_INSTALL_NAME_DIR:STRING=#{lib}",
        "-D#{option_prefix}WITH_BULLET=#{(build.with? 'bullet') ? 'ON' : 'OFF'}",
        "-D#{option_prefix}WITH_DART=#{(build.with? 'dartsim') ? 'ON' : 'OFF'}",
        "-D#{option_prefix}WITH_EIGEN=#{(build.with? 'eigen') ? 'ON' : 'OFF'}",
        "-D#{option_prefix}WITH_GLM=#{(build.with? 'glm') ? 'ON' : 'OFF'}",
        "-D#{option_prefix}WITH_IMGUI=ON",
        ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
