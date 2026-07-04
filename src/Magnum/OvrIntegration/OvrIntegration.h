#ifndef Magnum_OvrIntegration_OvrIntegration_h
#define Magnum_OvrIntegration_OvrIntegration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015, 2016 Jonathan Hale <squareys@googlemail.com>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Forward declarations for the @ref Magnum::OvrIntegration namespace
 * @m_deprecated_since_latest The original Oculus hardware and the associated
 *      SDK are no longer supported by the manufacturer and the integration
 *      library is thus scheduled for removal. At the moment, no integration
 *      for the successor OpenXR library is provided.
 */
#endif

#include <Magnum/configure.h>

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/DeprecationMacros.h>
#include <Magnum/Magnum.h>

#ifndef _MAGNUM_NO_DEPRECATED_OVRINTEGRATION
CORRADE_DEPRECATED_FILE("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete")
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum { namespace OvrIntegration {

class CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Session;
class CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") TextureSwapChain;
class CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Context;
class CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Compositor;
class CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Layer;
class CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") LayerEyeFov;
class CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") LayerQuad;

enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") StatusFlag: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") HmdType: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") HmdCapability: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Button: UnsignedInt;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Touch: UnsignedInt;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") TrackingOrigin: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") TrackerFlag: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") ControllerType: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") SessionStatusFlag: UnsignedByte;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") MirrorOption: UnsignedInt;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") PerformanceHudMode: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") DebugHudStereoMode: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") DetectResult: UnsignedByte;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") LayerHudMode: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") LayerType: Int;
enum class CORRADE_DEPRECATED_ENUM("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") ErrorType: Int;

CORRADE_IGNORE_DEPRECATED_PUSH
typedef CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Containers::EnumSet<StatusFlag> StatusFlags;
typedef CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Containers::EnumSet<TrackerFlag> TrackerFlags;
CORRADE_IGNORE_DEPRECATED_POP
struct CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Buttons;
struct CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Touches;
CORRADE_IGNORE_DEPRECATED_PUSH
typedef CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Containers::EnumSet<SessionStatusFlag> SessionStatusFlags;
typedef CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Containers::EnumSet<DetectResult> DetectResults;
typedef CORRADE_DEPRECATED("the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete") Containers::EnumSet<MirrorOption> MirrorOptions;
CORRADE_IGNORE_DEPRECATED_POP

}}
#endif
#else
#error the original Oculus hardware and SDK is no longer supported and the OvrIntegration library is thus obsolete
#endif

#endif
