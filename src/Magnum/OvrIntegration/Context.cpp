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

#include "Context.h"

#include "Magnum/OvrIntegration/Session.h"
#include "Magnum/OvrIntegration/Enums.h"

#include <OVR_CAPI_GL.h>

namespace Magnum { namespace OvrIntegration {

Debug& operator<<(Debug& debug, const DetectResult value) {
    switch(value) {
        #define _c(value) case DetectResult::value: return debug << "OvrIntegration::DetectResult::" #value;
        _c(ServiceRunning)
        _c(HmdConnected)
        #undef _c
    }

    return debug << "OvrIntegration::DetectResult::(invalid)";
}

Context* Context::_instance = nullptr;

Context::Context() : _compositor() {
    CORRADE_ASSERT(_instance == nullptr, "Another instance of Context already exists.", );

    _instance = this;
    ovr_Initialize(nullptr);
}

Context::~Context() {
    ovr_Shutdown();

    _instance = nullptr;
}

Context& Context::get() {
    CORRADE_ASSERT(_instance != nullptr,
                   "No instance of Context for ::get() exists.",
                   *Context::_instance);

    return *Context::_instance;
}

bool Context::detect() const {
    return ovr_GetHmdDesc(nullptr).Type != ovrHmd_None;
}

std::unique_ptr<Session> Context::createSession() {
    if(!detect()) return {};

    ovrSession session;
    ovrGraphicsLuid luid;
    ovr_Create(&session, &luid);
    return std::unique_ptr<Session>(new Session(session));
}

Error Context::error() const {
    ovrErrorInfo info;
    ovr_GetLastErrorInfo(&info);

    Error err;
    err.type = ErrorType(info.Result);
    std::strncpy(err.message, info.ErrorString, 512);

    return err;
}

}}
