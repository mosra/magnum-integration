if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2019" call "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvarsall.bat" x64 || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvarsall.bat" x64 || exit /b
set PATH=%APPVEYOR_BUILD_FOLDER%\deps\bin;%PATH%

rem Build Bullet
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
    -G Ninja || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build Corrade
git clone --depth 1 git://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DWITH_INTERCONNECT=OFF ^
    -DUTILITY_USE_ANSI_COLORS=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build Magnum
git clone --depth 1 git://github.com/mosra/magnum.git || exit /b
cd magnum || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%/SDL ^
    -DTARGET_GLES=ON ^
    -DTARGET_GLES2=%TARGET_GLES2% ^
    -DTARGET_DESKTOP_GLES=ON ^
    -DWITH_AUDIO=OFF ^
    -DWITH_DEBUGTOOLS=OFF ^
    -DWITH_MESHTOOLS=OFF ^
    -DWITH_PRIMITIVES=OFF ^
    -DWITH_SCENEGRAPH=ON ^
    -DWITH_SHADERS=ON ^
    -DWITH_TEXT=OFF ^
    -DWITH_TEXTURETOOLS=OFF ^
    -DWITH_OPENGLTESTER=ON ^
    -DWITH_WINDOWLESSWGLAPPLICATION=ON ^
    -DWITH_SDL2APPLICATION=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Unlike ALL OTHER VARIABLES, CMAKE_MODULE_PATH chokes on backwards slashes.
rem What the hell. This insane snippet converts them.
set "APPVEYOR_BUILD_FOLDER_FWD=%APPVEYOR_BUILD_FOLDER:\=/%"

rem Build
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH="%APPVEYOR_BUILD_FOLDER%/bullet;%APPVEYOR_BUILD_FOLDER%/SDL" ^
    -DGLM_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/glm ^
    -DIMGUI_DIR=%APPVEYOR_BUILD_FOLDER%/deps/imgui ^
    -DCMAKE_MODULE_PATH=%APPVEYOR_BUILD_FOLDER_FWD%/deps/eigen/cmake/ ^
    -DEIGEN3_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/eigen/ ^
    -DWITH_BULLET=ON ^
    -DWITH_DART=OFF ^
    -DWITH_EIGEN=ON ^
    -DWITH_GLM=ON ^
    -DWITH_IMGUI=ON ^
    -DWITH_OVR=OFF ^
    -DBUILD_TESTS=ON ^
    -DBUILD_GL_TESTS=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b

rem Test
set CORRADE_TEST_COLOR=ON
ctest -V -E GLTest || exit /b

rem Test install, after running the tests as for them it shouldn't be needed
cmake --build . --target install || exit /b
