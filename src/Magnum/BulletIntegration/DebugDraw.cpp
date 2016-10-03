/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
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

namespace Magnum { namespace BulletIntegration {

Debug& operator<<(Debug& debug, const DebugDraw::Mode value) {
    switch(value) {
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
        _c(FastWirefram)
        _c(DrawNormals)
        _c(DrawFrames)
        #undef _c
    }

    return debug << "BulletIntegration::DebugDraw::Mode(" << Debug::nospace << reinterpret_cast<void*>(Int(value)) << Debug::nospace << ")";
}

DebugDraw::DebugDraw(const UnsignedInt initialBufferCapacity):
    btIDebugDraw(),
    _mesh{MeshPrimitive::Lines}
{
    _mesh.addVertexBuffer(_buffer, 0, Shaders::VertexColor3D::Position{}, Shaders::VertexColor3D::Color{});
    _bufferData.reserve(initialBufferCapacity*4);
}

DebugDraw::~DebugDraw() = default;

void DebugDraw::setDebugMode(int debugMode) {
    _debugMode = Mode(debugMode);
}

int DebugDraw::getDebugMode() const {
    return Int(_debugMode);
}

void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    drawLine(from, to, color, color);
}

void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) {
   _bufferData.push_back(Vector3(from));
   _bufferData.push_back(Color3(fromColor));
   _bufferData.push_back(Vector3(to));
   _bufferData.push_back(Color3(toColor));
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
   _buffer.setData(Containers::ArrayView<Vector3>(_bufferData.data(), _bufferData.size()),
                   BufferUsage::DynamicDraw);

   /* Update shader and draw */
   _shader.setTransformationProjectionMatrix(_transformationProjectionMatrix);
   _mesh.setCount(_bufferData.size()/2)
        .draw(_shader);

   /* Clear buffer to receive new data */
   _bufferData.clear();
}

}}
