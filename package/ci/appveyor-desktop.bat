if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2022" call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat" %PLATFORM% || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2019" call "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvarsall.bat" %PLATFORM% || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvarsall.bat" %PLATFORM% || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2015" call "C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/vcvarsall.bat" %PLATFORM% || exit /b
set PATH=%APPVEYOR_BUILD_FOLDER%\deps\bin;%PATH%

rem - Need to explicitly specify a 64-bit target, otherwise CMake+Ninja can't
rem   figure that out -- https://gitlab.kitware.com/cmake/cmake/issues/16259
rem - For TestSuite we need to enable exceptions explicitly with /EH as these
rem   are currently disabled -- https://github.com/catchorg/Catch2/issues/1113
rem - The MSVC 2022 build has coverage enabled, for which the profile library
rem   has to be explicitly linked -- https://bugs.llvm.org/show_bug.cgi?id=40877,
rem   https://gitlab.kitware.com/cmake/cmake/-/issues/23437
if "%COMPILER%" == "msvc-clang" if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2022" set COMPILER_EXTRA=-DCMAKE_C_COMPILER="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/clang-cl.exe" -DCMAKE_CXX_COMPILER="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/clang-cl.exe" -DCMAKE_LINKER="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/lld-link.exe" -DCMAKE_C_FLAGS="-m64 /EHsc --coverage" -DCMAKE_CXX_FLAGS="-m64 /EHsc --coverage" -DCMAKE_EXE_LINKER_FLAGS="-libpath:\"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/lib/clang/19/lib/windows\" \"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/lib/clang/19/lib/windows/clang_rt.profile-x86_64.lib\"" -DCMAKE_SHARED_LINKER_FLAGS="-libpath:\"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/lib/clang/19/lib/windows\" \"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/lib/clang/19/lib/windows/clang_rt.profile-x86_64.lib\""
if "%COMPILER%" == "msvc-clang" if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2019" set COMPILER_EXTRA=-DCMAKE_C_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/bin/clang-cl.exe" -DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/bin/clang-cl.exe" -DCMAKE_LINKER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/bin/lld-link.exe" -DCMAKE_C_FLAGS="-m64 /EHsc" -DCMAKE_CXX_FLAGS="-m64 /EHsc"

rem Build Bullet
if "%ENABLE_BULLET%" == "ON" (
    IF NOT EXIST %APPVEYOR_BUILD_FOLDER%\2.86.1.zip appveyor DownloadFile https://github.com/bulletphysics/bullet3/archive/2.86.1.zip || exit /b
    7z x 2.86.1.zip || exit /b
    cd bullet3-2.86.1 || exit /b
    mkdir build && cd build || exit /b
    cmake .. ^
        -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/bullet ^
        -DCMAKE_BUILD_TYPE=Debug ^
        -DUSE_GRAPHICAL_BENCHMARK=OFF ^
        -DBUILD_CPU_DEMOS=OFF ^
        -DBUILD_BULLET2_DEMOS=OFF ^
        -DBUILD_BULLET3=OFF ^
        -DBUILD_EXTRAS=OFF ^
        -DBUILD_OPENGL3_DEMOS=OFF ^
        -DINSTALL_LIBS=ON ^
        -DBUILD_UNIT_TESTS=OFF ^
        -DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON ^
        %COMPILER_EXTRA% -G Ninja || exit /b
    cmake --build . --target install || exit /b
    cd .. && cd ..
)

rem Build Corrade
git clone --depth 1 https://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCORRADE_WITH_INTERCONNECT=OFF ^
    -DCORRADE_UTILITY_USE_ANSI_COLORS=ON ^
    %COMPILER_EXTRA% -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build Magnum
git clone --depth 1 https://github.com/mosra/magnum.git || exit /b
cd magnum || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%/SDL ^
    -DMAGNUM_WITH_AUDIO=OFF ^
    -DMAGNUM_WITH_DEBUGTOOLS=ON ^
    -DMAGNUM_WITH_MATERIALTOOLS=OFF ^
    -DMAGNUM_WITH_MESHTOOLS=%ENABLE_BULLET% ^
    -DMAGNUM_WITH_PRIMITIVES=%ENABLE_BULLET% ^
    -DMAGNUM_WITH_SCENEGRAPH=%ENABLE_BULLET% ^
    -DMAGNUM_WITH_SCENETOOLS=OFF ^
    -DMAGNUM_WITH_SHADERS=ON ^
    -DMAGNUM_WITH_SHADERTOOLS=OFF ^
    -DMAGNUM_WITH_TEXT=OFF ^
    -DMAGNUM_WITH_TEXTURETOOLS=OFF ^
    -DMAGNUM_WITH_OPENGLTESTER=ON ^
    -DMAGNUM_WITH_SDL2APPLICATION=ON ^
    -DMAGNUM_WITH_GLFWAPPLICATION=ON ^
    %COMPILER_EXTRA% -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Unlike ALL OTHER VARIABLES, CMAKE_MODULE_PATH chokes on backwards slashes.
rem What the hell. This insane snippet converts them.
set "APPVEYOR_BUILD_FOLDER_FWD=%APPVEYOR_BUILD_FOLDER:\=/%"

rem Build. Eigen3 is header-only but the archive is so stupid that it's not
rem possible to just use Eigen3Config.cmake, as it's generated using CMake from
rem Eigen3Config.cmake.in. There's FindEigen3.cmake next to it, but that
rem doesn't help with ANYTHING AT ALL (like, what about looking one directory
rem up, eh?! too hard?!) and also defines just EIGEN3_INCLUDE_DIR, not the
rem Eigen3::Eigen target nor EIGEN3_INCLUDE_DIRS. Now I get why people hate
rem CMake. It's because project maintainers are absolutely clueless on how to
rem write usable find scripts with it.
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH="%APPVEYOR_BUILD_FOLDER%/SDL;%APPVEYOR_BUILD_FOLDER%/bullet" ^
    -DCMAKE_MODULE_PATH=%APPVEYOR_BUILD_FOLDER_FWD%/deps/eigen/cmake/ ^
    -DEIGEN3_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/eigen/ ^
    -DGLM_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/glm ^
    -DIMGUI_DIR=%APPVEYOR_BUILD_FOLDER%/deps/imgui ^
    -DMAGNUM_WITH_BULLET=%ENABLE_BULLET% ^
    -DMAGNUM_WITH_DART=OFF ^
    -DMAGNUM_WITH_EIGEN=ON ^
    -DMAGNUM_WITH_GLM=ON ^
    -DMAGNUM_WITH_IMGUI=ON ^
    -DMAGNUM_WITH_OVR=ON ^
    -DMAGNUM_BUILD_TESTS=ON ^
    -DMAGNUM_BUILD_GL_TESTS=ON ^
    %COMPILER_EXTRA% -G Ninja || exit /b
cmake --build . || exit /b

rem Test
set CORRADE_TEST_COLOR=ON
rem On Windows, if an assertion or other issue happens, A DIALOG WINDOWS POPS
rem UP FROM THE CONSOLE. And then, for fucks sake, IT WAITS ENDLESSLY FOR YOU
rem TO CLOSE IT!! Such behavior is utterly stupid in a non-interactive setting
rem such as on this very CI, so I'm setting a timeout to 60 seconds to avoid
rem the CI job being stuck for an hour if an assertion happens. CTest's default
rem timeout is somehow 10M seconds, which is as useful as nothing at all.
ctest -V -E GLTest --timeout 60 || exit /b

rem Test install, after running the tests as for them it shouldn't be needed
cmake --build . --target install || exit /b

cd %APPVEYOR_BUILD_FOLDER%

rem Gather and upload coverage on the clang-cl MSVC 2022 build. Keep in sync
rem with circleci.yml, appveyor-desktop-mingw.bat and PKBUILD-coverage, please.
if "%COMPILER%" == "msvc-clang" if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2022" grcov build -t lcov --keep-only "*/src/Magnum/*" --ignore "*/Test/*" --ignore "*/build/src/*" -o coverage.info --excl-line LCOV_EXCL_LINE --excl-start LCOV_EXCL_START --excl-stop LCOV_EXCL_STOP || exit /b
rem Official docs say "not needed for public repos", in reality not using the
rem token is "extremely flakey". What's best is that if the upload fails, the
rem damn thing exits with a success error code, and nobody cares:
rem https://github.com/codecov/codecov-circleci-orb/issues/139
rem https://community.codecov.com/t/commit-sha-does-not-match-circle-build/4266
if "%COMPILER%" == "msvc-clang" if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2022" codecov -f ./coverage.info -t c2ea9495-5454-4294-a51d-4cadf575b325 -X gcov || exit /b
