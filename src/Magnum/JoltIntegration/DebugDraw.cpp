/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2026 Igal Alkon <igal@alkontek.com>

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
#include <Corrade/Containers/Array.h>
#include <Magnum/Math/Color.h>

namespace Magnum { namespace JoltIntegration {

    Debug& operator<<(Debug& debug, const DebugDraw::Mode value) {
        switch(value) {
            #define _c(value) case DebugDraw::Mode::value: return debug << "JoltIntegration::DebugDraw::Mode::" #value;
            _c(DrawWireframes)
            _c(DrawAabbs)
            _c(DrawConstraints)
            _c(DrawContacts)
            _c(DrawGetSupportFunction)
            _c(DrawSupportDirection)
            _c(DrawGetSupportingFace)
            _c(DrawShape)
            _c(DrawShapeWireframe)
            _c(DrawBoundingBox)
            _c(DrawCenterOfMassTransform)
            _c(DrawWorldTransform)
            _c(DrawVelocity)
            _c(DrawMassAndInertia)
            _c(DrawSleepStats)
            _c(DrawSoftBodyVertices)
            _c(DrawSoftBodyVertexVelocities)
            _c(DrawSoftBodyEdgeConstraints)
            _c(DrawSoftBodyBendConstraints)
            _c(DrawSoftBodyVolumeConstraints)
            _c(DrawSoftBodySkinConstraints)
            _c(DrawSoftBodyLRAConstraints)
            _c(DrawSoftBodyRods)
            _c(DrawSoftBodyRodStates)
            _c(DrawSoftBodyRodBendTwistConstraints)
            _c(DrawSoftBodyPredictedBounds)
            #undef _c
        }
        return debug << "JoltIntegration::DebugDraw::Mode(" << Debug::nospace
            << Debug::hex << static_cast<UnsignedInt>(static_cast<Int>(value)) << Debug::nospace << ")";
    }

    DebugDraw::DebugDraw(const std::size_t initialBufferCapacity): _mesh{GL::MeshPrimitive::Lines} {
    _mesh.addVertexBuffer(
        _buffer,
        0,
        Shaders::VertexColorGL3D::Position{},
        Shaders::VertexColorGL3D::Color4{});

    arrayReserve(_bufferData, initialBufferCapacity * 2);
}

DebugDraw::DebugDraw(NoCreateT) noexcept
    : _shader{NoCreate}, _buffer{NoCreate}, _mesh{NoCreate} {}

DebugDraw::~DebugDraw() = default;

void DebugDraw::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) {
    const Vector3 from {inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()};
    const Vector3 to {inTo.GetX(), inTo.GetY(), inTo.GetZ()};

    const Vector4 color {
        static_cast<Float>(inColor.r) / 255.0f,
        static_cast<Float>(inColor.g) / 255.0f,
        static_cast<Float>(inColor.b) / 255.0f,
        1.0f
    };

    arrayAppend(_bufferData, {
        Vertex{from, color},
        Vertex{to,   color}
    });
}

void DebugDraw::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3,
                             JPH::ColorArg inColor, ECastShadow inCastShadow) {
    DrawLine(inV1, inV2, inColor);
    DrawLine(inV2, inV3, inColor);
    DrawLine(inV3, inV1, inColor);
}

void DebugDraw::DrawText3D(JPH::RVec3Arg, const std::string_view&, JPH::ColorArg, float) {
    // not implemented yet
}

void DebugDraw::flush() {
    if (_bufferData.isEmpty()) return;

    _buffer.setData(_bufferData, GL::BufferUsage::DynamicDraw);
    _mesh.setCount(_bufferData.size());

    _shader
        .setTransformationProjectionMatrix(_transformationProjectionMatrix)
        .draw(_mesh);

    arrayResize(_bufferData, 0);
}

DebugDraw& DebugDraw::setMode(const UnsignedInt mode) {
    _mode = mode;
    _drawSettings = JPH::BodyManager::DrawSettings{};

    _drawSettings.mDrawGetSupportFunction = (_mode & static_cast<UnsignedInt>(Mode::DrawGetSupportFunction)) != 0;
    _drawSettings.mDrawSupportDirection = (_mode & static_cast<UnsignedInt>(Mode::DrawSupportDirection)) != 0;
    _drawSettings.mDrawGetSupportingFace = (_mode & static_cast<UnsignedInt>(Mode::DrawGetSupportingFace)) != 0;
    _drawSettings.mDrawShape = (_mode & static_cast<UnsignedInt>(Mode::DrawShape)) != 0;
    _drawSettings.mDrawShapeWireframe = (_mode & static_cast<UnsignedInt>(Mode::DrawShapeWireframe)) != 0;
    _drawSettings.mDrawBoundingBox = (_mode & static_cast<UnsignedInt>(Mode::DrawBoundingBox)) != 0;
    _drawSettings.mDrawCenterOfMassTransform = (_mode & static_cast<UnsignedInt>(Mode::DrawCenterOfMassTransform)) != 0;
    _drawSettings.mDrawWorldTransform = (_mode & static_cast<UnsignedInt>(Mode::DrawWorldTransform)) != 0;
    _drawSettings.mDrawVelocity = (_mode & static_cast<UnsignedInt>(Mode::DrawVelocity)) != 0;
    _drawSettings.mDrawMassAndInertia = (_mode & static_cast<UnsignedInt>(Mode::DrawMassAndInertia)) != 0;
    _drawSettings.mDrawSleepStats = (_mode & static_cast<UnsignedInt>(Mode::DrawSleepStats)) != 0;
    _drawSettings.mDrawSoftBodyVertices = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyVertices)) != 0;
    _drawSettings.mDrawSoftBodyVertexVelocities = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyVertexVelocities)) != 0;
    _drawSettings.mDrawSoftBodyEdgeConstraints = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyEdgeConstraints)) != 0;
    _drawSettings.mDrawSoftBodyBendConstraints = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyBendConstraints)) != 0;
    _drawSettings.mDrawSoftBodyVolumeConstraints = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyVolumeConstraints)) != 0;
    _drawSettings.mDrawSoftBodySkinConstraints = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodySkinConstraints)) != 0;
    _drawSettings.mDrawSoftBodyLRAConstraints = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyLRAConstraints)) != 0;
    _drawSettings.mDrawSoftBodyRods = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyRods)) != 0;
    _drawSettings.mDrawSoftBodyRodStates = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyRodStates)) != 0;
    _drawSettings.mDrawSoftBodyRodBendTwistConstraints = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyRodBendTwistConstraints)) != 0;
    _drawSettings.mDrawSoftBodyPredictedBounds = (_mode & static_cast<UnsignedInt>(Mode::DrawSoftBodyPredictedBounds)) != 0;

    return *this;
}

const JPH::BodyManager::DrawSettings& DebugDraw::drawSettings() const {
    return _drawSettings;
}

}}
