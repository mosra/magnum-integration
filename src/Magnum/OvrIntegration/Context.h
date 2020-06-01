#ifndef Magnum_OvrIntegration_Context_h
#define Magnum_OvrIntegration_Context_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
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
 * @brief Class @ref Magnum::OvrIntegration::Context, struct @ref Magnum::OvrIntegration::Error, enum @ref Magnum::OvrIntegration::DetectResult, enum set @ref Magnum::OvrIntegration::DetectResults
 */

#include <memory>

#include "Magnum/OvrIntegration/Compositor.h"
#include "Magnum/OvrIntegration/Session.h" /* required for std::unique_ptr<Session> */
#include "Magnum/OvrIntegration/OvrIntegration.h"
#include "Magnum/OvrIntegration/visibility.h"

namespace Magnum { namespace OvrIntegration {

/**
@brief Error

@see @ref Context::error()
*/
struct Error {
    ErrorType type;     /**< @brief Error type */
    char message[512];  /**< @brief Error message */
};

/**
@brief Detection result

@see @ref DetectResults, @ref Context::detect(Int)
*/
enum class DetectResult: UnsignedByte {
    ServiceRunning = 1,     /**< Service is running */
    HmdConnected = 2        /**< HMD is connected */
};

/**
@brief Detection results

@see @ref Context::detect(Int)
*/
typedef Containers::EnumSet<DetectResult> DetectResults;

CORRADE_ENUMSET_OPERATORS(DetectResults)

/**
@brief Context singleton

Handles connection to devices, creation of debug HMDs and provides access to
the Oculus SDK @ref Compositor.

@section OvrIntegration-Context-usage Usage

There should always only be one instance of @ref Context. As soon as this
one instance is created, you can access it via @ref Context::get().

Example:

@code{.cpp}
Context context;

// ...

if(Context::get().detect()) {
    // ...
}
@endcode

Or without initializing the service you can do:

@code{.cpp}
const OvrDetectResults result = Context::detect();
if(result & OvrDetectResult::HmdConnected) {
    // ...
}
@endcode

@see @ref Session, @ref Compositor
*/
class MAGNUM_OVRINTEGRATION_EXPORT Context {
    public:
        /**
         * @brief Detect if a device is currently connected
         * @param timeout Timeout (in milliseconds) or @cpp 0 @ce to poll
         *
         * Checks for Oculus Runtime and Oculus HMD device status without
         * loading the LibOVRRT shared library.  This may be called before
         * @ref Context() to help decide whether or not to initialize LibOVR.
         */
        static DetectResults detect(Int timeout) {
            const ovrDetectResult result = ovr_Detect(timeout);
            return ((result.IsOculusHMDConnected) ? DetectResult::ServiceRunning : DetectResults{}) |
                   ((result.IsOculusServiceRunning) ? DetectResult::HmdConnected : DetectResults{});
        }

        /**
         * @brief Global context instance
         *
         * Expects that the instance is created.
         */
        static Context& get();

        /** @brief Constructor */
        explicit Context();

        /** @brief Copying is not allowed */
        Context(const Context&) = delete;

        /** @brief Moving is not allowed */
        Context(Context&&) = delete;

        ~Context();

        /** @brief Copying is not allowed */
        Context& operator=(const Context&) = delete;

        /** @brief Moving is not allowed */
        Context& operator=(Context&&) = delete;

        /**
         * @brief Detect if a device is currently connected
         *
         * See @ref detect(Int) to detect runtime and HMD before initializing
         * context.
         */
        bool detect() const;

        /**
         * @brief Create a handle to a Oculus Session
         *
         * Returns instance of @ref Session, or @cpp nullptr @ce,
         * if there is none.
         */
        std::unique_ptr<Session> createSession();

        /** @return Reference to the compositor */
        Compositor& compositor() { return _compositor; }

        /**
         * @brief Last error
         */
        Error error() const;

    private:
        static Context* _instance;
        Compositor _compositor;
};

/** @debugoperatorenum{Magnum::OvrIntegration::DetectResult} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, DetectResult value);

}}

#endif
