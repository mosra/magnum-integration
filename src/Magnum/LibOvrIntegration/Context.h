#ifndef Magnum_LibOvrIntegration_Context_h
#define Magnum_LibOvrIntegration_Context_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015
              Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::LibOvrIntegration::OVRTextureUtil,
 *        enum @ref Magnum::LibOvrIntegration::LibOvrContextStatusFlag
 *
 * @author Jonathan Hale (Squareys)
 */

#include <memory>

#include <Magnum/Texture.h>

#include "Magnum/LibOVRIntegration/visibility.h"
#include "Magnum/LibOVRIntegration/LibOvrIntegration.h"
#include "Magnum/LibOVRIntegration/Hmd.h" /* required for std::unique_ptr */
#include "Magnum/LibOVRIntegration/Compositor.h"

namespace Magnum { namespace LibOvrIntegration {

/**
@brief Singleton class LibOvrContext.

Context for the LibOvrIntegration, gateway to connecting devices and
creating @ref Hmd.

@see @ref Hmd, @ref SwapTextureSet

@todoc Usage...

@author Jonathan Hale (Squareys)
*/
class MAGNUM_LIBOVRINTEGRATION_EXPORT LibOvrContext {
    public:

        /** @brief Constructor. */
        explicit LibOvrContext();

        /** @brief Copying is not allowed. */
        LibOvrContext(const LibOvrContext& context) = delete;

        /** @brief Moving is not allowed. */
        LibOvrContext(LibOvrContext&& context) = delete;

        /** @brief Destructor. */
        ~LibOvrContext();

        /**
         * @brief Get the instance of the LibOvrContext.
         * @return the context.
         */
         static LibOvrContext& get();

        /**
         * @brief Detect how many devices are currently connected.
         * @return number of connected devices.
         */
        int detect() const;

        /**
         * @brief Create a handle to a connected hmd device.
         * @param index Index of the device, must be greater than 0 and smaller than @ref #detect()
         * @param debugType If not @ref HmdType::None, this device type will be used for a debug hmd
         *        in case a real connection cannot be established.
         * @return A @ref std::unique_pointer to the hmd at the given index (if exists), or a debug hmd,
         *         if debugType is not specified as @ref HmdType::None, in which case the alternate return
         *         value is an empty @ref std::unique_pointer.
         */
        std::unique_ptr<Hmd> createHmd(int index, HmdType debugType);

        /**
         * @brief Create a hmd handle which is not connected to an actual device.
         * @param debugType Type of device to create a debug handle to.
         * @return A @ref std::unique_pointer to the debug hmd.
         */
        std::unique_ptr<Hmd> createDebugHmd(HmdType debugType);

        /**
         * @return Reference to the compositor.
         */
        Compositor& compositor() {
            return _compositor;
        }

    private:
        static LibOvrContext* _instance;
        Compositor _compositor;
};

}}

#endif
