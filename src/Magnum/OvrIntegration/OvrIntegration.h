#ifndef Magnum_OvrIntegration_OvrIntegration_h
#define Magnum_OvrIntegration_OvrIntegration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
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

/** @file
 * @brief Forward declarations for the @ref Magnum::OvrIntegration namespace
 */

#include <Magnum/Magnum.h>

namespace Magnum { namespace OvrIntegration {

class Session;
class TextureSwapChain;
class Context;
class Compositor;
class Layer;
class LayerEyeFov;
class LayerQuad;

enum class StatusFlag: Int;
enum class HmdType: Int;
enum class HmdCapability: Int;
enum class Button: UnsignedInt;
enum class Touch: UnsignedInt;
enum class TrackingOrigin: Int;
enum class TrackerFlag: Int;
enum class ControllerType: Int;
enum class SessionStatusFlag: UnsignedByte;
enum class MirrorOption: UnsignedInt;
enum class PerformanceHudMode: Int;
enum class DebugHudStereoMode: Int;
enum class DetectResult: UnsignedByte;
enum class LayerHudMode: Int;
enum class LayerType: Int;
enum class ErrorType: Int;

typedef Containers::EnumSet<StatusFlag> StatusFlags;
typedef Containers::EnumSet<TrackerFlag> TrackerFlags;
struct Buttons;
struct Touches;
typedef Containers::EnumSet<SessionStatusFlag> SessionStatusFlags;
typedef Containers::EnumSet<DetectResult> DetectResults;
typedef Corrade::Containers::EnumSet<MirrorOption> MirrorOptions;

}}

#endif
