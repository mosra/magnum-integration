#ifndef Magnum_BulletIntegration_DebugDraw_h
#define Magnum_BulletIntegration_DebugDraw_h
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

/** @file
 * @brief Implementation of `btIDebugDraw` for physics visualization in Bullet
 */

#include <vector>

#include <Corrade/Containers/ArrayView.h>

#include <Magnum/Buffer.h>
#include <Magnum/DebugOutput.h>
#include <Magnum/Magnum.h>
#include <Magnum/Mesh.h>
#include <Magnum/Shaders/VertexColor.h>

#include <LinearMath/btIDebugDraw.h>

#include "Magnum/BulletIntegration/Integration.h"

namespace Magnum { namespace BulletIntegration {

/**
@brief Bullet physics debug visualization

This class implements `btIDebugDraw`, which allows rendering a visualization
of a Bullet physics world for debugging purposes.

## Usage

@code
btDynamicsWorld* btWorld = // ...
DebugDraw debugDraw;

debugDraw.setDebugMode(DebugDraw::Mode::DrawWireframe | DebugDraw::Mode::DrawConstraints);
btWorld->setDebugDrawer(&debugDraw);

// Per frame call:

debugDraw.setTransformationProjectionMatrix(projection*view*transformation);
btWorld->debugDrawWorld();
@endcode
*/
class MAGNUM_BULLETINTEGRATION_EXPORT DebugDraw: public btIDebugDraw {
    public:

        /** @brief Debug mode enum */
        enum class Mode: Int {
            NoDebug = DBG_NoDebug,                      /**< Disable debug rendering */
            DrawWireframe = DBG_DrawWireframe,          /**< Draw wireframe of all collision shapes */
            DrawAabb = DBG_DrawAabb,                    /**< Draw axis aligned bounding box of all collision object */
            DrawFeaturesText = DBG_DrawFeaturesText,    /**< Draw text for features */
            DrawContactPoints = DBG_DrawContactPoints,  /**< Draw contact points */
            NoDeactivation = DBG_NoDeactivation,        /**< Disable deactivation of objects */
            NoHelpText = DBG_NoHelpText,                /**< Diable help text */
            DrawText = DBG_DrawText,                    /**< Enable text drawing */
            ProfileTimings = DBG_ProfileTimings,        /**< Profile timings */
            EnableSatComparison = DBG_EnableSatComparison, /**< Enable Sat Comparison */
            DisableBulletLCP = DBG_DisableBulletLCP,    /**< Disable Bullet LCP */
            EnableCCD = DBG_EnableCCD,                  /**< Enable CCD */
            DrawConstraints = DBG_DrawConstraints,      /**< Draw constaints */
            DrawConstraintLimits = DBG_DrawConstraintLimits, /**< Draw constraint limits */
            FastWirefram = DBG_FastWireframe,           /**< Draw fast wireframes */
            DrawNormals = DBG_DrawNormals,              /**< Draw normals */
            DrawFrames = DBG_DrawFrames,                /**< Draw frames */
        };

        /** @brief Debug draw mode flags */
        typedef Containers::EnumSet<Mode> Modes;

        /**
         * @brief Constructor
         * @param initialBufferCapacity Amount of lines for which to reserve
         *        memory in the buffer vector.
         *
         * Sets up @ref Shaders::VertexColor3D, @ref Buffer and @ref Mesh for physics debug rendering.
         */
        explicit DebugDraw(UnsignedInt initialBufferCapacity=0);
        ~DebugDraw();

        /** @brief Debug mode */
        Modes debugMode() const {
            return _debugMode;
        }

        /**
         * @brief Set debug mode
         * @return Reference to self (for method chaining)
         */
        DebugDraw& setDebugMode(Modes debugMode) {
            _debugMode = debugMode;
            return *this;
        }

        /** @brief Set transformation projection matrix used for rendering */
        DebugDraw& setTransformationProjectionMatrix(const Matrix4& transProj) {
            _transformationProjectionMatrix = transProj;
            return *this;
        }

    private:

        void setDebugMode(int debugMode) override;

        int getDebugMode() const override;

        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

        void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;

        void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int CORRADE_UNUSED lifeTime, const btVector3& color) override;

        void reportErrorWarning(const char *warningString) override;

        void draw3dText(const btVector3& CORRADE_UNUSED location, const char* CORRADE_UNUSED textString) override;

        void flushLines() override;

        Modes _debugMode;

        Matrix4 _transformationProjectionMatrix;
        Shaders::VertexColor3D _shader;

        Buffer _buffer;
        Mesh _mesh;
        std::vector<Vector3> _bufferData;
};

CORRADE_ENUMSET_OPERATORS(DebugDraw::Modes)

/** @debugoperatorenum{Magnum::BulletIntegration::DebugDraw::Mode} */
MAGNUM_BULLETINTEGRATION_EXPORT Debug& operator<<(Debug& debug, DebugDraw::Mode value);

}}


#endif
