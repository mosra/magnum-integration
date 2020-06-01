#ifndef Magnum_DartIntegration_Object_h
#define Magnum_DartIntegration_Object_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018, 2019 Konstantinos Chatzilygeroudis <costashatz@gmail.com>

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
 * @brief Class @ref Magnum::DartIntegration::Object, struct @ref Magnum::DartIntegration::DrawData
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Magnum/GL/GL.h>
#include <Magnum/SceneGraph/AbstractFeature.h>
#include <Magnum/SceneGraph/AbstractTranslationRotation3D.h>
#include <Magnum/Trade/Trade.h>

#include "Magnum/DartIntegration/visibility.h"

namespace dart { namespace dynamics {
    class BodyNode;
    class ShapeNode;
}}

namespace Magnum { namespace DartIntegration {

/**
@brief Shape draw data

@see @ref Object::drawData()
@experimental
*/
struct DrawData {
    /**
     * @brief Constructor
     * @param meshes            Meshes
     * @param materials         Material data
     * @param textures          Textures
     * @param scaling           Object scaling
     *
     * Used internally by @ref Object.
     */
    explicit DrawData(Containers::Array<GL::Mesh> meshes, Containers::Array<Trade::PhongMaterialData> materials, Containers::Array<Containers::Optional<GL::Texture2D>> textures, const Vector3& scaling);

    /** @brief Copying is not allowed */
    DrawData(const DrawData&) = delete;

    /** @brief Move constructor */
    DrawData(DrawData&&) noexcept = default;

    /** @brief Copying is not allowed */
    DrawData& operator=(const DrawData&) = delete;

    /** @brief Move assignment */
    DrawData& operator=(DrawData&&) noexcept = default;

    ~DrawData();

    /** @brief Meshes */
    Containers::Array<GL::Mesh> meshes;

    /** @brief Material data */
    Containers::Array<Trade::PhongMaterialData> materials;

    /** @brief Textures */
    Containers::Array<Containers::Optional<GL::Texture2D>> textures;

    /** @brief Scaling */
    Vector3 scaling;
};

/**
@brief DART Physics BodyNode or ShapeNode

Encapsulates `BodyNode` or `ShapeNode` as a @ref SceneGraph feature.

@section DartIntegration-Object-usage Usage

Common usage is to create a @ref DartIntegration::Object to share
transformation with a DART `BodyNode` or `ShapeNode` by passing a pointer to
its constructor:

@snippet DartIntegration.cpp Object-bodynode

or

@snippet DartIntegration.cpp Object-shapenode

Only the DART body/node can affect the transformation of the Magnum object and
not the other way around. To get the latest DART transformation, you should
update the object with @ref update().

@experimental
*/
class MAGNUM_DARTINTEGRATION_EXPORT Object: public SceneGraph::AbstractBasicFeature3D<Float> {
    public:
        /**
         * @brief Constructor
         * @param object    SceneGraph::Object this @ref DartIntegration::Object belongs to
         * @param node      DART `ShapeNode` to connect with
         */
        template<class T> Object(T& object, dart::dynamics::ShapeNode* node = nullptr): Object{object, object, node, nullptr} {}

        /**
         * @brief Constructor
         * @param object    SceneGraph::Object this @ref DartIntegration::Object belongs to
         * @param body      DART `BodyNode` to connect with
         */
        template<class T> Object(T& object, dart::dynamics::BodyNode* body = nullptr): Object{object, object, nullptr, body} {}

        /**
         * @brief Get transformation from DART
         * @return Reference to self (for method chaining)
         */
        Object& update(Trade::AbstractImporter* importer = nullptr);

        /**
         * @brief Whether the object was updated
         *
         * @see @ref clearUpdateFlag()
         */
        bool isUpdated() const { return _updated; }

        /**
         * @brief Clear update flag
         * @return Reference to self (for method chaining)
         */
        Object& clearUpdateFlag() {
            _updated = false;
            return *this;
        }

        /** @brief Whether object mesh was updated */
        bool hasUpdatedMesh() const { return _updatedMesh; }

        /** @brief Data for drawing */
        DrawData& drawData() { return *_drawData; }

        /** @brief Underlying DART `ShapeNode` */
        dart::dynamics::ShapeNode* shapeNode() { return _node; }

        /** @brief Underlying DART `BodyNode` */
        dart::dynamics::BodyNode* bodyNode() { return _body; }

    private:
        explicit Object(SceneGraph::AbstractBasicObject3D<Float>& object, SceneGraph::AbstractBasicTranslationRotation3D<Float>& transformation, dart::dynamics::ShapeNode* node, dart::dynamics::BodyNode* body);

        bool MAGNUM_DARTINTEGRATION_LOCAL extractDrawData(Trade::AbstractImporter* importer = nullptr);

        SceneGraph::AbstractBasicTranslationRotation3D<Float>& _transformation;
        dart::dynamics::ShapeNode* _node;
        dart::dynamics::BodyNode* _body;
        Containers::Optional<DrawData> _drawData;
        bool _updated, _updatedMesh;
};

}}

#endif
