#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018 Jonathan Hale <squareys@googlemail.com>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

# This script generates a minimal Oculus SDK archive from the original SDK archive.
# Unpacks the archive, patches the VS project to link CRT dynamically and then
# zips up the libraries and includes only.

import os
import sys
import subprocess
import zipfile
import fileinput

def main():
    if len(sys.argv) != 2:
        print("Usage: python make_minimal_ovr_sdk.py <path to oculus sdk archive>")
        return

    filename = sys.argv[1]
    basedir = filename[:-4]

    # Unzip archive
    if not os.path.isdir(basedir) or not os.listdir(basedir):
        print("Extracting %s into %s" % (filename, basedir))
        os.mkdir(basedir)
        with zipfile.ZipFile(filename, 'r') as zf:
            zf.extractall(basedir)

    # Patch RuntimeLibrary property here to link the CRT dynamically like the rest of magnum instead of statically
    # See https://docs.microsoft.com/en-us/dotnet/api/microsoft.visualstudio.vcprojectengine.runtimelibraryoption?view=visualstudiosdk-2017
    # for possible RuntimeLibrary values.
    vcProjectPath = os.path.join(basedir, "OculusSDK", "LibOVR", "Projects", "Windows", "VS2015", "LibOVR.vcxproj");
    with fileinput.FileInput(vcProjectPath, inplace=True) as file:
        for line in file:
            print(line.replace("MultiThreadedDebug<", "MultiThreadedDebugDLL<")
                      .replace("MultiThreaded<", "MultiThreadedDLL<"), # Prevent replacing substring of the above
                  end="")

    # Build libovr
    print("Building libovr")

    if subprocess.call(["msbuild", vcProjectPath, "/p:Configuration=Debug", "/nologo"]) != 0:
        raise Exception("Failed to build debug config of libovr")
    if subprocess.call(["msbuild", vcProjectPath, "/p:Configuration=Release", "/nologo"]) != 0:
        raise Exception("Failed to build release config of libovr")

    # Zip up required files
    print("Zipping final result")
    with zipfile.ZipFile(basedir + "_minimal.zip", "w") as zf:
        libDir = os.path.join("OculusSDK", "LibOVR", "Lib", "Windows", "x64");

        paths = [os.path.join(libDir, "Debug", "VS2015", "LibOVR.lib"),
                 os.path.join(libDir, "Release", "VS2015", "LibOVR.lib"),
                 os.path.join("OculusSDK", "THIRD_PARTY_NOTICES.txt"),
                 os.path.join("OculusSDK", "LICENSE.txt")]

        os.chdir(basedir)
        for path, dirs, files in os.walk(os.path.join("OculusSDK", "LibOVR", "Include")):
            for file in files:
                paths.append(os.path.join(path, file))
        os.chdir("..")

        for path in paths:
            print("Zipping %s..." % (path))
            zf.write(os.path.join(basedir, path), path);

if __name__ == "__main__":
    main()
