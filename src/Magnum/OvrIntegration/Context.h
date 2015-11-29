#ifndef Magnum_OvrIntegration_Context_h
#define Magnum_OvrIntegration_Context_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::OvrIntegration::Context
 */

#include <memory>
#include <Magnum/Texture.h>

#include "Magnum/OvrIntegration/Compositor.h"
#include "Magnum/OvrIntegration/Hmd.h" /* required for std::unique_ptr */
#include "Magnum/OvrIntegration/HmdEnum.h"
#include "Magnum/OvrIntegration/OvrIntegration.h"
#include "Magnum/OvrIntegration/visibility.h"

namespace Magnum { namespace OvrIntegration {

/**
 * @brief Error
 * @see @ref Context::error()
 */
struct Error {
    ErrorType type;         /**< @brief Error type */
    std::string message;    /**< @brief Error message */
};

/**
 * @brief Result of @ref Context::detect(int).
 */
enum class OvrDetectResult: Byte {
    ServiceRunning = 1,
    HmdConnected = 2
};

typedef Corrade::Containers::EnumSet<OvrDetectResult> OvrDetectResults;
CORRADE_ENUMSET_OPERATORS(OvrDetectResults)

/**
@brief Context singleton

Handles connection to devices, creation of debug HMDs and provides access to
the Oculus SDK @ref Compositor.

## Usage

There should always only be one instance of @ref Context. As soon as this
one instance is created, you can access it via @ref Context::get().

Example:

@code
Context context;

// ...

if(Context::get().detect()) {
    // ...
}
@endcode

Or without initializing the service you can do:

@code
const OvrDetectResults result = Context::detect();
if(result & OvrDetectResult::HmdConnected) {
    // ...
}
@endcode

@see @ref Hmd, @ref Compositor
*/
class MAGNUM_OVRINTEGRATION_EXPORT Context {
    public:

        /**
         * @brief Detect if a device is currently connected
         * @param timeout Timeout (in milliseconds) or 0 to poll
         *
         * Checks for Oculus Runtime and Oculus HMD device status without
         * loading the LibOVRRT shared library.  This may be called before
         * @ref Context() to help decide whether or not to initialize LibOVR.
         */
        static OvrDetectResults detect(Int timeout) {
            const ovrDetectResult result = ovr_Detect(timeout);
            return ((result.IsOculusHMDConnected) ? OvrDetectResult::ServiceRunning : OvrDetectResults{})
                 | ((result.IsOculusServiceRunning) ? OvrDetectResult::HmdConnected : OvrDetectResults{});
        }

        /**
         * @brief Global context instance
         *
         * Expects that the instance is created.
         */
        static Context& get();

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
         * @see @ref detect(int) to detect runtime and HMD before initializing Context.
         */
        bool detect() const;

        /**
         * @brief Create a handle to a connected HMD device
         *
         * Returns instance of the connected HMD, or `nullptr`,
         * if there is none.
         */
        std::unique_ptr<Hmd> createHmd();

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

/** @debugoperatorenum{Magnum::OvrIntegration::OvrDetectResult} */
MAGNUM_OVRINTEGRATION_EXPORT Debug& operator<<(Debug& debug, OvrDetectResult value);

inline Error Context::error() const {
    ovrErrorInfo info;
    ovr_GetLastErrorInfo(&info);

    return Error{ErrorType(info.Result), info.ErrorString};
}

}}

#endif
