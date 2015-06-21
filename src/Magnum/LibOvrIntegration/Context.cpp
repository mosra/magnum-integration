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

#include "Magnum/LibOvrIntegration/Context.h"

#include "Magnum/LibOvrIntegration/Hmd.h"
#include "Magnum/LibOvrIntegration/HmdEnum.h"

#include <OVR_CAPI_GL.h>

namespace Magnum { namespace LibOvrIntegration {

LibOvrContext* LibOvrContext::_instance = nullptr;

LibOvrContext::LibOvrContext() : _compositor() {
    CORRADE_ASSERT(_instance == nullptr, "Another instance of LibOvrContext already exists.", );

    _instance = this;
    ovr_Initialize(nullptr);
}

LibOvrContext::~LibOvrContext() {
    ovr_Shutdown();

    _instance = nullptr;
}

LibOvrContext& LibOvrContext::get() {
    CORRADE_ASSERT(_instance != nullptr,
                   "No instance of LibOvrContext for ::get() exists.",
                   *LibOvrContext::_instance);

    return *LibOvrContext::_instance;
}

Int LibOvrContext::detect() const {
    return ovrHmd_Detect();
}

std::unique_ptr<Hmd> LibOvrContext::createHmd(Int index, HmdType debugType) {
    /* check if index is valid */
    if(index >= 0 && detect() > index) {
        ovrHmd hmd;
        ovrHmd_Create(index, &hmd);
        return std::unique_ptr<Hmd>(new Hmd(hmd, {}));
    } else if(debugType != HmdType::None){
        /* create a debug hmd instead of connecting to a device */
        return createDebugHmd(debugType);
    }

    return std::unique_ptr<Hmd>();
}

std::unique_ptr<Hmd> LibOvrContext::createDebugHmd(HmdType debugType) {
    ovrHmd hmd;
    ovrHmd_CreateDebug(ovrHmdType(Corrade::Containers::EnumSet<HmdType>::UnderlyingType(debugType)), &hmd);

    return std::unique_ptr<Hmd>(new Hmd(hmd, HmdStatusFlag::Debug));
}

}}
