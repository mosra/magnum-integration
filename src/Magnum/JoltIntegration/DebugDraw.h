#ifndef Magnum_JoltIntegration_DebugDraw_h
#define Magnum_JoltIntegration_DebugDraw_h
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

/** @file
 * @brief CLass @ref Magnum::JoltIntegration::DebugDraw
 * Implementation of `JPH::DebugRenderer` for physics visualization in Jolt.
 */
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Renderer/DebugRendererSimple.h>
#include <Jolt/Physics/Body/BodyManager.h>

#include <Corrade/Containers/Array.h>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Shaders/VertexColorGL.h>

#include "visibility.h"

namespace Magnum { namespace JoltIntegration {
    /**
     * @brief Jolt physics debug visualization
     */
    class MAGNUM_JOLTINTEGRATION_EXPORT DebugDraw : public JPH::DebugRendererSimple {
    public:
        /**
         * @brief Debug mode
         * @see @ref Modes, @ref setMode(), @ref drawWorldData()
         */
        enum class Mode: UnsignedInt {
            /* Draw body wireframes (via PhysicsSystem::DrawBodies()) */
            DrawWireframes = 1 << 0,
            /* Draw AABBs for active bodies */
            DrawAabbs = 1 << 1,
            /* Draw constraints */
            DrawConstraints = 1 << 2,
            /* Draw contact points/normals */
            DrawContacts = 1 << 3,
            /* Draw the GetSupport() function, used for convex collision detection */
            DrawGetSupportFunction = 1 << 4,
            /* When drawing the support function, also draw which direction mapped to a specific support point */
            DrawSupportDirection = 1 << 5,
            /* Draw the faces that were found colliding during collision detection */
            DrawGetSupportingFace = 1 << 6,
            /* Draw the shapes of all bodies */
            DrawShape = 1 << 7,
            /* When mDrawShape is true and this is true, the shapes will be drawn in wireframe instead of solid. */
            DrawShapeWireframe = 1 << 8,
            /* Draw a bounding box per body */
            DrawBoundingBox = 1 << 9,
            /* Draw the center of mass for each body */
            DrawCenterOfMassTransform = 1 << 10,
            /* Draw the world transform (which can be different than the center of mass) for each body */
            DrawWorldTransform = 1 << 11,
            /* Draw the velocity vector for each body */
            DrawVelocity = 1 << 12,
            /* Draw the mass and inertia (as the box equivalent) for each body */
            DrawMassAndInertia = 1 << 13,
            /* Draw stats regarding the sleeping algorithm of each body */
            DrawSleepStats = 1 << 14,
            /* Draw the vertices of soft bodies */
            DrawSoftBodyVertices = 1 << 15,
            /* Draw the velocities of the vertices of soft bodies */
            DrawSoftBodyVertexVelocities = 1 << 16,
            /* Draw the edge constraints of soft bodies */
            DrawSoftBodyEdgeConstraints = 1 << 17,
            /* Draw the bend constraints of soft bodies */
            DrawSoftBodyBendConstraints = 1 << 18,
            /* Draw the volume constraints of soft bodies */
            DrawSoftBodyVolumeConstraints = 1 << 19,
            /* Draw the skin constraints of soft bodies */
            DrawSoftBodySkinConstraints = 1 << 20,
            /* Draw the LRA constraints of soft bodies */
            DrawSoftBodyLRAConstraints = 1 << 21,
            /* Draw the rods of soft bodies */
            DrawSoftBodyRods = 1 << 22,
            /* Draw the rod states (orientation and angular velocity) of soft bodies */
            DrawSoftBodyRodStates = 1 << 23,
            /* Draw the rod bend twist constraints of soft bodies */
            DrawSoftBodyRodBendTwistConstraints = 1 << 24,
            /* Draw the predicted bounds of soft bodies */
            DrawSoftBodyPredictedBounds = 1 << 25
        };

        /**
         * @brief Debug modes
         * @see @ref setMode()
         */
        typedef Containers::EnumSet<Mode> Modes;

        /**
         * @brief Constructor
         * @param initialBufferCapacity  Number of lines for which to reserve memory in the buffer vector.
         *
         * Sets up @ref Shaders::VertexColorGL3D, @ref GL::Buffer and
         * @ref GL::Mesh for physics debug rendering.
         */
        explicit DebugDraw(std::size_t initialBufferCapacity = 0);

        /**
         * @brief Construct without creating the underlying OpenGL objects
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * for deferring the initialization to a later point, for example, if
         * the OpenGL context is not yet created. Move another instance over it
         * to make it useful.
         */
        explicit DebugDraw(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        DebugDraw(const DebugDraw&) = delete;

        ~DebugDraw() override;

        /** @brief Copying is not allowed */
        DebugDraw& operator=(const DebugDraw&) = delete;

        /**
          * @brief Debug modes bitfield
          * @see @ref setMode()
          */
        UnsignedInt mode() const { return _mode; }

        /**
         * @brief Set debug mode bitfield
         * @return Reference to self (for method chaining)
         *
         * Use `static_cast<UnsignedInt>(Mode::XXX)` or bitwise OR.
         * By default, nothing is enabled (`0`).
         */
        DebugDraw& setMode(UnsignedInt mode);

        /** @brief Get DrawSettings populated from current modes */
        const JPH::BodyManager::DrawSettings& drawSettings() const;

        /** @brief Set the transformation projection matrix used for rendering */
        DebugDraw& setTransformationProjectionMatrix(const Matrix4& matrix) {
            _transformationProjectionMatrix = matrix;
            return *this;
        }

        /** @brief Flush all accumulated debug lines to the GPU and draw them */
        void flush();

        void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
        void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor,
            ECastShadow inCastShadow) override;
        void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor,
            float inHeight) override;

private:
    struct Vertex {
        Vector3 position;
        Vector4 color;
    };

    UnsignedInt _mode{};
    JPH::BodyManager::DrawSettings _drawSettings{};

    Matrix4 _transformationProjectionMatrix;
    Shaders::VertexColorGL3D _shader;
    GL::Buffer _buffer;
    GL::Mesh _mesh;
    Containers::Array<Vertex> _bufferData;
};

CORRADE_ENUMSET_OPERATORS(DebugDraw::Modes)

/** @debugoperatorenum{Magnum::JoltIntegration::DebugDraw::Mode} */
MAGNUM_JOLTINTEGRATION_EXPORT Debug& operator<<(Debug& debug, DebugDraw::Mode value);

}}

#endif
