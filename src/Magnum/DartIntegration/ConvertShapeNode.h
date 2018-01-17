#ifndef Magnum_DARTIntegration_ConvertShapeNode_h
#define Magnum_DARTIntegration_ConvertShapeNode_h
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
 * @brief Function @ref Magnum::DartIntegration::convertShapeNode()
 * @brief Class @ref Magnum::DartIntegration::ShapeData
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>

#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/TextureData.h>

#include <Magnum/DartIntegration/DartObject.h>
#include <Magnum/DartIntegration/visibility.h>

namespace Magnum { namespace Trade {
    class MeshData3D;
    class PhongMaterialData;
    template<UnsignedInt dimensions> class ImageData;
    typedef ImageData<2> ImageData2D;
    class TextureData;
}}

namespace Magnum { namespace DartIntegration {
/**
@brief ShapeData struct
*/
struct ShapeData {
    /** mesh information */
    Trade::MeshData3D mesh;
    /** material information */
    Trade::PhongMaterialData material;
    /** ImageData2D (vector) */
    Containers::Array<Containers::Optional<Trade::ImageData2D>> images;
    /** TextureData (vector) */
    Containers::Array<Containers::Optional<Trade::TextureData>> textures;
};

/**
@brief Convert Dart ShapeNode to @ref ShapeData that can be processed by Magnum
@param shapeNode   Dart ShapeNode to convert

Returns `Corrade::Containers::NullOpt` if the shape of the given ShapeNode is not supported.

## Supported/Unsupported Dart shapes

The following Dart shapes are supported:
   - BoxShape
   - CapsuleShape
   - CylinderShape
   - EllipsoidShape
   - MeshShape
   - SphereShape

The following Dart shapes are NOT yet supported:
   - ConeShape
   - LineSegmentShape
   - MultiSphereConvexHullShape
   - PlaneShape (this is infite plane with normal)
   - SoftMeshShape
*/
Containers::Optional<ShapeData> MAGNUM_DARTINTEGRATION_EXPORT convertShapeNode(dart::dynamics::ShapeNode& shapeNode);

/**
@brief Convert DartObject to @ref ShapeData that can be processed by Magnum

See @ref convertShape(dart::dynamics::ShapeNode* shapeNode)
for more information.
*/
Containers::Optional<ShapeData> MAGNUM_DARTINTEGRATION_EXPORT convertShapeNode(DartObject& object);

}}

#endif