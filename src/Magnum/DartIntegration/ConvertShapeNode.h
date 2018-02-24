#ifndef Magnum_DartIntegration_ConvertShapeNode_h
#define Magnum_DartIntegration_ConvertShapeNode_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018 Konstantinos Chatzilygeroudis <costashatz@gmail.com>

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
 * @brief Class @ref Magnum::DartIntegration::ShapeData, function @ref Magnum::DartIntegration::convertShapeNode()
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/TextureData.h>
#include <Magnum/Trade/Trade.h>

#include "Magnum/DartIntegration/DartIntegration.h"
#include "Magnum/DartIntegration/visibility.h"

namespace dart { namespace dynamics {
    class BodyNode;
    class ShapeNode;
}}

namespace Magnum { namespace DartIntegration {

/**
@brief Shape data

@see @ref convertShapeNode()
@experimental
*/
struct ShapeData {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    explicit ShapeData(Containers::Array<Trade::MeshData3D> meshes, Containers::Array<Trade::PhongMaterialData> materials, Containers::Array<Containers::Optional<Trade::ImageData2D>> images, Containers::Array<Containers::Optional<Trade::TextureData>> textures, const Vector3& scaling = Vector3{1.f, 1.f, 1.f}): meshes{std::move(meshes)}, materials{std::move(materials)}, images{std::move(images)}, textures{std::move(textures)}, scaling(scaling) {}

    ShapeData(const ShapeData&) = delete;
    ShapeData(ShapeData&&) noexcept = default;
    ShapeData& operator=(const ShapeData&) = delete;
    ShapeData& operator=(ShapeData&&) noexcept = default;

    ~ShapeData() = default;
    #endif

    /** @brief Mesh data */
    Containers::Array<Trade::MeshData3D> meshes;

    /** @brief Material data */
    Containers::Array<Trade::PhongMaterialData> materials;

    /** @brief Image data */
    Containers::Array<Containers::Optional<Trade::ImageData2D>> images;

    /** @brief Texture data */
    Containers::Array<Containers::Optional<Trade::TextureData>> textures;

    /** @brief Scaling */
    Vector3 scaling;
};

/**
@brief ConvertShapeType

What should be loaded from the dart::dynamics::ShapeNode, see @ref convertShapeNode()
@experimental
*/
enum class ConvertShapeType: unsigned int {
    /* @brief Load only material information */
    Material = 1 << 0,
    /* @brief Load scaling information */
    Primitive = 1 << 1,
    /* @brief Load full meshes */
    Mesh = 1 << 2,
    /* @brief Load everything */
    All = Material | Primitive | Mesh
};

typedef Containers::EnumSet<ConvertShapeType> ConvertShapeTypes;
CORRADE_ENUMSET_OPERATORS(ConvertShapeTypes)

/**
@brief Convert `dart::dynamics::ShapeNode` to meshes and material data

Returns @ref Corrade::Containers::NullOpt if the shape of given `ShapeNode` is
not supported. The following DART shapes are supported:

-   `BoxShape`
-   `CapsuleShape`
-   `CylinderShape`
-   `EllipsoidShape`
-   `MeshShape`
-   `SoftMeshShape`
-   `SphereShape`

The following DART shapes are not yet supported:

-   `ConeShape`
-   `LineSegmentShape`
-   `MultiSphereConvexHullShape`
-   `PlaneShape` (this is an infinite plane with normal)

The @ref Trade::AbstractImporter `importer` argument is only used when loading
`dart::dynamics::ShapeNode` with a shape of type `dart::dynamics::MeshShape`.
As a consequence, you can pass a nullptr if you do not want to load such a
`dart::dynamics::ShapeNode`. On the contrary, when you are trying to load a
`dart::dynamics::MeshShape` and the importer is a nullptr, the function will
return @ref Corrade::Containers::NullOpt

@attention
Soft meshes should be drawn with @ref Renderer::Feature::FaceCulling enabled
as each triangle is drawn twice (once with the original orientation and once
with the reversed orientation)

@experimental
*/
Containers::Optional<ShapeData> MAGNUM_DARTINTEGRATION_EXPORT convertShapeNode(dart::dynamics::ShapeNode& shapeNode, ConvertShapeTypes loadType, Trade::AbstractImporter* importer = nullptr);

}}

#endif