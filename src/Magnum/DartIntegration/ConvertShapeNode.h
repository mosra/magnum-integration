#ifndef Magnum_DartIntegration_ConvertShapeNode_h
#define Magnum_DartIntegration_ConvertShapeNode_h
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
 * @brief Struct @ref Magnum::DartIntegration::ShapeData, function @ref Magnum::DartIntegration::convertShapeNode()
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Trade/Trade.h>

#include "Magnum/DartIntegration/DartIntegration.h"
#include "Magnum/DartIntegration/visibility.h"

namespace dart { namespace dynamics {
    class ShapeNode;
}}

namespace Magnum { namespace DartIntegration {

/**
@brief Shape data

Returned from @ref convertShapeNode().
@experimental
*/
struct MAGNUM_DARTINTEGRATION_EXPORT ShapeData {
    /**
     * @brief Constructor
     * @param meshes    Mesh data
     * @param materials Material data
     * @param images    Image data
     * @param textures  Texture data
     * @param scaling   Shape scaling
     *
     * Used internally by @ref convertShapeNode().
     */
    explicit ShapeData(Containers::Array<Trade::MeshData> meshes, Containers::Array<Trade::PhongMaterialData> materials, Containers::Array<Containers::Optional<Trade::ImageData2D>> images, Containers::Array<Containers::Optional<Trade::TextureData>> textures, const Vector3& scaling);

    /** @brief Copying is not allowed */
    ShapeData(const ShapeData&) = delete;

    /** @brief Move constructor */
    ShapeData(ShapeData&&) noexcept = default;

    /** @brief Copying is not allowed */
    ShapeData& operator=(const ShapeData&) = delete;

    /** @brief Move assignment */
    ShapeData& operator=(ShapeData&&) noexcept = default;

    ~ShapeData();

    /**
     * @brief Mesh data
     *
     * Each mesh has corresponding material data in @ref materials and
     * optionally texture data in @ref images and @ref textures at the same
     * index.
     */
    Containers::Array<Trade::MeshData> meshes;

    /** @brief Material data corresponding to meshes */
    Containers::Array<Trade::PhongMaterialData> materials;

    /**
     * @brief Image data corresponding to meshes
     *
     * Set to @ref Corrade::Containers::NullOpt in case given mesh has no
     * texture.
     */
    Containers::Array<Containers::Optional<Trade::ImageData2D>> images;

    /**
     * @brief Texture data corresponding to meshes
     *
     * Set to @ref Corrade::Containers::NullOpt in case given mesh has no
     * texture.
     */
    Containers::Array<Containers::Optional<Trade::TextureData>> textures;

    /** @brief Scaling */
    Vector3 scaling;
};

/**
@brief Shape convert type

@see @ref convertShapeNode()
@experimental
*/
enum class ConvertShapeType: UnsignedInt {
    Material = 1 << 0,              /**< Load only material information */
    Primitive = 1 << 1,             /**< Load scaling information */
    Mesh = 1 << 2,                  /**< Load full meshes */
    All = Material|Primitive|Mesh   /**< Load everything */
};

/**
@brief Shape convert types

@see @ref convertShapeNode()
@experimental
*/
typedef Containers::EnumSet<ConvertShapeType> ConvertShapeTypes;

CORRADE_ENUMSET_OPERATORS(ConvertShapeTypes)

/**
@brief Convert `dart::dynamics::ShapeNode` to meshes and material data

Returns @ref Corrade::Containers::NullOpt if the shape of given `ShapeNode` is
not supported. The following DART shapes are supported:

-   `BoxShape`
-   `CapsuleShape`
-   `ConeShape`
-   `CylinderShape`
-   `EllipsoidShape`
-   `MeshShape`
-   `SoftMeshShape`
-   `SphereShape`

The following DART shapes are not yet supported:

-   `LineSegmentShape`
-   `MultiSphereConvexHullShape`
-   `PlaneShape` (this is an infinite plane with normal)

The @p importer argument is an optional instance of @ref Trade::AssimpImporter.
It is only used when loading `dart::dynamics::ShapeNode`  with a shape of type
`dart::dynamics::MeshShape`. As a consequence, you can omit it you do not want
to load such a `dart::dynamics::ShapeNode`. On the contrary, when you are
trying to load a `dart::dynamics::MeshShape` and the importer is a @cpp nullptr @ce,
the function will return @ref Corrade::Containers::NullOpt.

@attention Soft meshes should be drawn with @ref GL::Renderer::Feature::FaceCulling
    enabled as each triangle is drawn twice (once with the original orientation
    and once with the reversed orientation).

@experimental
*/
Containers::Optional<ShapeData> MAGNUM_DARTINTEGRATION_EXPORT convertShapeNode(dart::dynamics::ShapeNode& shapeNode, ConvertShapeTypes convertTypes, Trade::AbstractImporter* importer = nullptr);

}}

#endif
