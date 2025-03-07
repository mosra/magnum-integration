if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvarsall.bat" x64 || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" set GENERATOR=Visual Studio 15 2017
rem This is what should make the *native* corrade-rc getting found. Corrade
rem should not cross-compile it on this platform, because then it'd get found
rem instead of the native version with no way to distinguish the two, and all
rem hell breaks loose. Thus also not passing CORRADE_RC_EXECUTABLE anywhere
rem below to ensure this doesn't regress.
set PATH=%APPVEYOR_BUILD_FOLDER%\deps-native\bin;%PATH%

rem Build ANGLE. The repo is now just a README redirecting to googlesource.
rem I don't want to bother with this right now, so checking out last usable
rem version from 2017. TODO: fix when I can be bothered
git clone --depth 10 https://github.com/MSOpenTech/angle.git || exit /b
cd angle || exit /b
git checkout c61d0488abd9663e0d4d2450db7345baa2c0dfb6 || exit /b
cd winrt\10\src || exit /b
msbuild angle.sln /p:Configuration=Release || exit /b
cd ..\..\..\.. || exit /b

git clone --depth 1 https://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b

rem Build native corrade-rc
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps-native ^
    -DCORRADE_WITH_INTERCONNECT=OFF ^
    -DCORRADE_WITH_PLUGINMANAGER=OFF ^
    -DCORRADE_WITH_TESTSUITE=OFF ^
    -DCORRADE_WITH_UTILITY=OFF ^
    -G Ninja || exit /b
cmake --build . --target install || exit /b
cd .. || exit /b

rem Crosscompile Corrade
mkdir build-rt && cd build-rt || exit /b
cmake .. ^
    -DCMAKE_SYSTEM_NAME=WindowsStore ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCORRADE_WITH_INTERCONNECT=OFF ^
    -DCORRADE_BUILD_STATIC=ON ^
    -G "%GENERATOR%" -A x64 || exit /b
cmake --build . --config Release --target install -- /m /v:m || exit /b
cd .. && cd ..

rem Crosscompile Magnum
git clone --depth 1 https://github.com/mosra/magnum.git || exit /b
cd magnum || exit /b
mkdir build-rt && cd build-rt || exit /b
cmake .. ^
    -DCMAKE_SYSTEM_NAME=WindowsStore ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DOPENGLES2_LIBRARY=%APPVEYOR_BUILD_FOLDER%/angle/winrt/10/src/Release_x64/lib/libGLESv2.lib ^
    -DOPENGLES2_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/angle/include ^
    -DOPENGLES3_LIBRARY=%APPVEYOR_BUILD_FOLDER%/angle/winrt/10/src/Release_x64/lib/libGLESv2.lib ^
    -DOPENGLES3_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/angle/include ^
    -DMAGNUM_WITH_AUDIO=OFF ^
    -DMAGNUM_WITH_DEBUGTOOLS=ON ^
    -DMAGNUM_WITH_MATERIALTOOLS=OFF ^
    -DMAGNUM_WITH_MESHTOOLS=OFF ^
    -DMAGNUM_WITH_PRIMITIVES=OFF ^
    -DMAGNUM_WITH_SCENEGRAPH=OFF ^
    -DMAGNUM_WITH_SCENETOOLS=OFF ^
    -DMAGNUM_WITH_SHADERS=ON ^
    -DMAGNUM_WITH_SHADERTOOLS=OFF ^
    -DMAGNUM_WITH_TEXT=OFF ^
    -DMAGNUM_WITH_TEXTURETOOLS=OFF ^
    -DMAGNUM_TARGET_GLES2=%TARGET_GLES2% ^
    -DMAGNUM_BUILD_STATIC=ON ^
    -G "%GENERATOR%" -A x64 || exit /b
cmake --build . --config Release --target install -- /m /v:m || exit /b
cd .. && cd ..

rem Unlike ALL OTHER VARIABLES, CMAKE_MODULE_PATH chokes on backwards slashes.
rem What the hell. This insane snippet converts them.
set "APPVEYOR_BUILD_FOLDER_FWD=%APPVEYOR_BUILD_FOLDER:\=/%"

rem Crosscompile. For a detailed Eigen rant, see appveyor-desktop.bat. Also no
rem tests because they take ages to build, each executable is a msix file, and
rem they can't be reasonably run either. F this platform.
mkdir build-rt && cd build-rt || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_MODULE_PATH=%APPVEYOR_BUILD_FOLDER_FWD%/deps/eigen/cmake/ ^
    -DGLM_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/glm ^
    -DIMGUI_DIR=%APPVEYOR_BUILD_FOLDER%/deps/imgui ^
    -DOPENGLES2_LIBRARY=%APPVEYOR_BUILD_FOLDER%/angle/winrt/10/src/Release_x64/lib/libGLESv2.lib ^
    -DOPENGLES2_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/angle/include ^
    -DOPENGLES3_LIBRARY=%APPVEYOR_BUILD_FOLDER%/angle/winrt/10/src/Release_x64/lib/libGLESv2.lib ^
    -DOPENGLES3_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/angle/include ^
    -DEIGEN3_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/deps/eigen/ ^
    -DMAGNUM_WITH_BULLET=OFF ^
    -DMAGNUM_WITH_DART=OFF ^
    -DMAGNUM_WITH_EIGEN=ON ^
    -DMAGNUM_WITH_GLM=ON ^
    -DMAGNUM_WITH_IMGUI=ON ^
    -DMAGNUM_WITH_OVR=OFF ^
    -DMAGNUM_BUILD_STATIC=ON ^
    -G "%GENERATOR%" -A x64 || exit /b
cmake --build . --config Release -- /m /v:m || exit /b

rem Test install, after running the tests as for them it shouldn't be needed
cmake --build . --config Release --target install || exit /b
