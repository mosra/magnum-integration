rem Workaround for CMake not wanting sh.exe on PATH for MinGW. AARGH.
set PATH=%PATH:C:\Program Files\Git\usr\bin;=%
set PATH=C:\mingw-w64\x86_64-7.2.0-posix-seh-rt_v5-rev1\mingw64\bin;%APPVEYOR_BUILD_FOLDER%\deps\bin;%APPVEYOR_BUILD_FOLDER%\bullet\bin;%PATH%

rem Build Bullet
IF NOT EXIST %APPVEYOR_BUILD_FOLDER%\2.86.1.zip appveyor DownloadFile https://github.com/bulletphysics/bullet3/archive/2.86.1.zip || exit /b
7z x 2.86.1.zip || exit /b
cd bullet3-2.86.1 || exit /b
mkdir build && cd build || exit /b
rem Can remove the 3.5 override once https://github.com/bulletphysics/bullet3/commit/d1a4256b3a019117f2bb6cb8c63d6367aaf512e2
rem (from April 2023) reaches a stable version. So far it didn't (Sep 25).
cmake .. ^
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/bullet ^
    -DBUILD_SHARED_LIBS=ON ^
    -DUSE_GRAPHICAL_BENCHMARK=OFF ^
    -DBUILD_CPU_DEMOS=OFF ^
    -DBUILD_BULLET2_DEMOS=OFF ^
    -DBUILD_BULLET3=OFF ^
    -DBUILD_EXTRAS=OFF ^
    -DBUILD_OPENGL3_DEMOS=OFF ^
    -DINSTALL_LIBS=ON ^
    -DBUILD_UNIT_TESTS=OFF ^
    -G Ninja || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build Corrade
git clone --depth 1 https://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_CXX_FLAGS="--coverage" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCORRADE_WITH_INTERCONNECT=OFF ^
    -DCORRADE_UTILITY_USE_ANSI_COLORS=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build Magnum
git clone --depth 1 https://github.com/mosra/magnum.git || exit /b
cd magnum || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_CXX_FLAGS="--coverage" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%/SDL ^
    -DMAGNUM_WITH_AUDIO=OFF ^
    -DMAGNUM_WITH_DEBUGTOOLS=ON ^
    -DMAGNUM_WITH_MATERIALTOOLS=OFF ^
    -DMAGNUM_WITH_MESHTOOLS=ON ^
    -DMAGNUM_WITH_PRIMITIVES=ON ^
    -DMAGNUM_WITH_SCENEGRAPH=ON ^
    -DMAGNUM_WITH_SCENETOOLS=OFF ^
    -DMAGNUM_WITH_SHADERS=ON ^
    -DMAGNUM_WITH_SHADERTOOLS=OFF ^
    -DMAGNUM_WITH_TEXT=OFF ^
    -DMAGNUM_WITH_TEXTURETOOLS=OFF ^
    -DMAGNUM_WITH_OPENGLTESTER=ON ^
    -DMAGNUM_WITH_SDL2APPLICATION=ON ^
    -DMAGNUM_WITH_GLFWAPPLICATION=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Unlike ALL OTHER VARIABLES, CMAKE_MODULE_PATH chokes on backwards slashes.
rem What the hell. This insane snippet converts them.
set "APPVEYOR_BUILD_FOLDER_FWD=%APPVEYOR_BUILD_FOLDER:\=/%"

rem Build
rem For MinGW it's not possible to use the OVR SDK directly, the Oculus Runtime
rem is needed to be installed, but that's apparently not possible from a
rem command-line so I'm just disabling it.
rem https://forums.oculus.com/community/discussion/18303/silent-installation-of-oculus-runtime-windows
rem For a detailed Eigen rant, see appveyor-desktop.bat
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_CXX_FLAGS="--coverage" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%/SDL ^
    -DCMAKE_MODULE_PATH=%APPVEYOR_BUILD_FOLDER_FWD%/deps/eigen/cmake/ ^
    -DGLM_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/glm ^
    -DIMGUI_DIR=%APPVEYOR_BUILD_FOLDER%/deps/imgui ^
    -DEIGEN3_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/eigen/ ^
    -DMAGNUM_WITH_BULLETINTEGRATION=ON ^
    -DMAGNUM_WITH_DARTINTEGRATION=OFF ^
    -DMAGNUM_WITH_EIGENINTEGRATION=ON ^
    -DMAGNUM_WITH_GLMINTEGRATION=ON ^
    -DMAGNUM_WITH_IMGUIINTEGRATION=ON ^
    -DMAGNUM_WITH_OVRINTEGRATION=OFF ^
    -DMAGNUM_BUILD_TESTS=ON ^
    -DMAGNUM_BUILD_GL_TESTS=ON ^
    -G Ninja || exit /b
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

rem Coverage upload
cd %APPVEYOR_BUILD_FOLDER%
set GCOV=C:/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov.exe
rem Keep in sync with circleci.yml, appveyor-desktop.bat and PKBUILD-coverage,
rem please. The --source-dir needs to be present in order to circumvent
rem     Warning: "../foo" cannot be normalized because of "..", so skip it.
rem that happens with CMake before 3.21 that doesn't yet pass full paths to
rem Ninja: https://github.com/mozilla/grcov/issues/1182
grcov build -t lcov --source-dir %APPVEYOR_BUILD_FOLDER%/build --keep-only "*/src/Magnum/*" --ignore "*/Test/*" -o coverage.info --excl-line LCOV_EXCL_LINE --excl-start LCOV_EXCL_START --excl-stop LCOV_EXCL_STOP  || exit /b
rem Official docs say "not needed for public repos", in reality not using the
rem token is "extremely flakey". What's best is that if the upload fails, the
rem damn thing exits with a success error code, and nobody cares:
rem https://github.com/codecov/codecov-circleci-orb/issues/139
rem https://community.codecov.com/t/commit-sha-does-not-match-circle-build/4266
codecov -f ./coverage.info -t c2ea9495-5454-4294-a51d-4cadf575b325 -X gcov || exit /b
