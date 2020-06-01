/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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

#include "DebugDraw.h"

#include <Corrade/Containers/GrowableArray.h>
#include <Magnum/Math/Color.h>

namespace Magnum { namespace BulletIntegration {

Debug& operator<<(Debug& debug, const DebugDraw::Mode value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DebugDraw::Mode::value: return debug << "BulletIntegration::DebugDraw::Mode::" #value;
        _c(NoDebug)
        _c(DrawWireframe)
        _c(DrawAabb)
        _c(DrawFeaturesText)
        _c(DrawContactPoints)
        _c(NoDeactivation)
        _c(NoHelpText)
        _c(DrawText)
        _c(ProfileTimings)
        _c(EnableSatComparison)
        _c(DisableBulletLCP)
        _c(EnableCCD)
        _c(DrawConstraints)
        _c(DrawConstraintLimits)
        _c(FastWireframe)
        _c(DrawNormals)
        #if BT_BULLET_VERSION >= 284
        /* Actually, it was at some point between 2.83 and 2.83.4. Relevant
           commit: https://github.com/bulletphysics/bullet3/commit/4af9c5a4c98cd2ecd8595f728af2f3d70512f8b2 */
        _c(DrawFrames)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "BulletIntegration::DebugDraw::Mode(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(Int(value))) << Debug::nospace << ")";
}

DebugDraw::DebugDraw(const std::size_t initialBufferCapacity): _mesh{GL::MeshPrimitive::Lines} {
    _mesh.addVertexBuffer(_buffer, 0, Shaders::VertexColor3D::Position{}, Shaders::VertexColor3D::Color3{});
    arrayReserve(_bufferData, initialBufferCapacity*4);
}

DebugDraw::DebugDraw(NoCreateT) noexcept: _shader{NoCreate}, _buffer{NoCreate}, _mesh{NoCreate} {}

DebugDraw::DebugDraw(DebugDraw&&) noexcept = default;

DebugDraw::~DebugDraw() = default;

DebugDraw& DebugDraw::operator=(DebugDraw&&) noexcept = default;

void DebugDraw::setDebugMode(int mode) {
    _mode = Mode(mode);
}

int DebugDraw::getDebugMode() const {
    return int(_mode);
}

void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    drawLine(from, to, color, color);
}

void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) {
    arrayAppend(_bufferData, {
        Vector3{from}, Vector3{fromColor},
        Vector3{to}, Vector3{toColor}});

    /* The flushLines() API was added at some point between 2.83 and 2.83.4,
       but that's below the resolution of the constant below. Moreover, 284
       corresponds to 2.83.6, while 2.84 is 285. Fun, right? Relevant commit:
       https://github.com/bulletphysics/bullet3/commit/7b28e86c7b9ab3c4bb8d479f843b9b5c2e1c9a06
       The Bullet API and documentation and everything is utter crap, so I'm
       not bothering more here. Yes, every call to drawLine() will cause a new
       drawcall with a single line segment to be submitted. Just to be clear
       why I did that: Ubuntu 14.04, which is now the oldest supported
       platform, has only 2.81 in the repositories. */
    #if BT_BULLET_VERSION < 284
    flushLines();
    #endif
}

void DebugDraw::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, const btScalar distance, const int, const btVector3& color) {
    drawLine(pointOnB, pointOnB + normalOnB*distance, color);
}

void DebugDraw::reportErrorWarning(const char *warningString) {
    Warning() << "DebugDraw:" << warningString;
}

void DebugDraw::draw3dText(const btVector3&, const char*) {
    /** @todo Implement once debug text drawing is implemented. */
}

void DebugDraw::flushLines() {
    /* Update buffer with new data */
    _buffer.setData(_bufferData, GL::BufferUsage::DynamicDraw);

    /* Update shader and draw */
    _mesh.setCount(_bufferData.size()/2);
    _shader
        .setTransformationProjectionMatrix(_transformationProjectionMatrix)
        .draw(_mesh);

    /* Clear buffer to receive new data */
    arrayResize(_bufferData, 0);
}

}}
