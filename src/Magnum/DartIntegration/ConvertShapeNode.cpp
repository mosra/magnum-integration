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

#include <dart/dynamics/BoxShape.hpp>
#include <dart/dynamics/CapsuleShape.hpp>
#include <dart/dynamics/CylinderShape.hpp>
#include <dart/dynamics/EllipsoidShape.hpp>
#include <dart/dynamics/MeshShape.hpp>
#include <dart/dynamics/ShapeNode.hpp>
#include <dart/dynamics/SphereShape.hpp>

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>

#include <Magnum/MeshTools/Transform.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/Primitives/Capsule.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Cylinder.h>
#include <Magnum/Primitives/Icosphere.h>
#include <Magnum/TextureFormat.h>
#include <Magnum/Trade/AbstractImporter.h>

#include <Magnum/DartIntegration/ConvertShapeNode.h>

namespace Magnum { namespace DartIntegration {

Containers::Optional<ShapeData> convertShapeNode(dart::dynamics::ShapeNode& shapeNode) {
    dart::dynamics::ShapePtr shape = shapeNode.getShape();

    /** Get material information -- we ignore the alpha value
     *  Note that if this is a MeshShape we will ignore this material
     */
    Eigen::Vector4d col = shapeNode.getVisualAspect()->getRGBA();

    /* default shininess to 80.f */
    Trade::PhongMaterialData matData = Trade::PhongMaterialData{Trade::PhongMaterialData::Flags{}, 80.f};
    /* get diffuse color from Dart ShapeNode */
    matData.diffuseColor() = Color3(col(0), col(1), col(2));
    /* default colors for ambient (black) and specular (white) */
    matData.ambientColor() = Vector3{0.f, 0.f, 0.f};
    matData.specularColor() = Vector3{1.f, 1.f, 1.f};

    if (shape->getType() == dart::dynamics::BoxShape::getStaticType()) {
        auto boxShape = std::static_pointer_cast<dart::dynamics::BoxShape>(shape);
        Eigen::Vector3d size = boxShape->getSize();

        Trade::MeshData3D meshData = Trade::MeshData3D{Primitives::Cube::solid()};
        /* Multiplying by 0.5f because the cube is 2x2x2 */
        MeshTools::transformPointsInPlace(Matrix4::scaling(Vector3(size(0), size(1), size(2)) * 0.5f), meshData.positions(0));

        return ShapeData{std::move(meshData), std::move(matData), {}, {}};
    }
    if (shape->getType() == dart::dynamics::CapsuleShape::getStaticType()) {
        auto capsuleShape = std::static_pointer_cast<dart::dynamics::CapsuleShape>(shape);

        Float r = Float(capsuleShape->getRadius());
        Float h = Float(capsuleShape->getHeight());
        Float halfLength = 0.5f * h / r;

        Trade::MeshData3D meshData = Trade::MeshData3D{Primitives::Capsule3D::solid(32, 32, 32, halfLength)};
        MeshTools::transformPointsInPlace(Matrix4::scaling(Vector3{r}), meshData.positions(0));

        return ShapeData{std::move(meshData), std::move(matData), {}, {}};
    }
    if (shape->getType() == dart::dynamics::CylinderShape::getStaticType()) {
        auto cylinderShape = std::static_pointer_cast<dart::dynamics::CylinderShape>(shape);

        Float r = Float(cylinderShape->getRadius());
        Float h = Float(cylinderShape->getHeight());
        Float halfLength = 0.5f * h / r;

        Trade::MeshData3D meshData = Trade::MeshData3D{Primitives::Cylinder::solid(32, 32, halfLength)};
        MeshTools::transformPointsInPlace(Matrix4::scaling(Vector3{r}), meshData.positions(0));

        return ShapeData{std::move(meshData), std::move(matData), {}, {}};
    }
    if (shape->getType() == dart::dynamics::EllipsoidShape::getStaticType()) {
        auto ellipsoidShape = std::static_pointer_cast<dart::dynamics::EllipsoidShape>(shape);

        Eigen::Vector3d size = ellipsoidShape->getDiameters();

        Trade::MeshData3D meshData = Trade::MeshData3D{Primitives::Icosphere::solid(5)};
        MeshTools::transformPointsInPlace(Matrix4::scaling(Vector3{Float(size(0)), Float(size(1)), Float(size(2))}), meshData.positions(0));

        return ShapeData{std::move(meshData), std::move(matData), {}, {}};
    }
    if (shape->getType() == dart::dynamics::MeshShape::getStaticType()) {
        /*  @todo check if we should not ignore the transformation in the Mesh */
        auto meshShape = std::static_pointer_cast<dart::dynamics::MeshShape>(shape);

        const aiScene* mesh = meshShape->getMesh();
        std::string meshPath = Utility::Directory::path(meshShape->getMeshPath());

        PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
        std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AssimpImporter");
        if (!importer)
            return Containers::NullOpt;

        bool loaded = importer->openState(mesh, meshPath);
        if (!loaded || importer->mesh3DCount() < 1)
            return Containers::NullOpt;

        /*  Most probably it does not make sense that one ShapeNode has multiple materials */
        if (importer->materialCount() > 0) {
            auto matPtr = importer->material(0);
            matData = std::move(*static_cast<Trade::PhongMaterialData*>(matPtr.release()));
        }

        /* Most probably it does not make sense that one ShapeNode has multiple meshes */
        Containers::Optional<Trade::MeshData3D> meshData = importer->mesh3D(0);
        if (!meshData)
            return Containers::NullOpt;

        Containers::Array<Containers::Optional<Trade::ImageData2D>> imgData(importer->textureCount());
        Containers::Array<Containers::Optional<Trade::TextureData>> texData(importer->textureCount());

        for (UnsignedInt i = 0; i < importer->textureCount(); ++i) {
            Containers::Optional<Trade::TextureData> textureData = importer->texture(i);
            if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
                Warning{} << "Cannot load texture, skipping";
                /* we need this to keep the texture indexing correct */
                imgData[i] = Containers::NullOpt;
                texData[i] = Containers::NullOpt;
                continue;
            }

            Containers::Optional<Trade::ImageData2D> imageData = importer->image2D(textureData->image());
            if (!imageData) {
                Warning{} << "Cannot load texture image, skipping";
                /* we need this to keep the texture indexing correct */
                imgData[i] = Containers::NullOpt;
                texData[i] = Containers::NullOpt;
                continue;
            }

            imgData[i] = std::move(imageData);
            texData[i] = std::move(textureData);
        }

        return ShapeData{std::move(*meshData), std::move(matData), std::move(imgData), std::move(texData)};
    }
    if (shape->getType() == dart::dynamics::SphereShape::getStaticType()) {
        auto sphereShape = std::static_pointer_cast<dart::dynamics::SphereShape>(shape);

        Float r = Float(sphereShape->getRadius());

        Trade::MeshData3D meshData = Trade::MeshData3D{Primitives::Icosphere::solid(4)};
        MeshTools::transformPointsInPlace(Matrix4::scaling(Vector3{r, r, r}), meshData.positions(0));

        return ShapeData{std::move(meshData), std::move(matData), {}, {}};
    }

    Error{} << "DartIntegration::convertShapeNode(): shape type" << shape->getType() << "is not supported!";

    return Containers::NullOpt;
}

Containers::Optional<ShapeData> convertShapeNode(DartObject& object) {
    return convertShapeNode(*object.shapeNode());
}

}}