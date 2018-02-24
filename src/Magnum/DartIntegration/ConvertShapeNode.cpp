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

#include "ConvertShapeNode.h"

#include <dart/dynamics/BodyNode.hpp>
#include <dart/dynamics/BoxShape.hpp>
#include <dart/dynamics/CapsuleShape.hpp>
#include <dart/dynamics/ConeShape.hpp>
#include <dart/dynamics/CylinderShape.hpp>
#include <dart/dynamics/EllipsoidShape.hpp>
#include <dart/dynamics/LineSegmentShape.hpp>
#include <dart/dynamics/MeshShape.hpp>
#include <dart/dynamics/MultiSphereConvexHullShape.hpp>
#include <dart/dynamics/PlaneShape.hpp>
#include <dart/dynamics/ShapeNode.hpp>
#include <dart/dynamics/SoftBodyNode.hpp>
#include <dart/dynamics/SoftMeshShape.hpp>
#include <dart/dynamics/SphereShape.hpp>

#include <Corrade/Utility/Directory.h>

#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/CombineIndexedArrays.h>
#include <Magnum/MeshTools/GenerateFlatNormals.h>
#include <Magnum/Primitives/Capsule.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Cylinder.h>
#include <Magnum/Primitives/Icosphere.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/ObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/TextureData.h>

namespace Magnum { namespace DartIntegration {

Containers::Optional<ShapeData> convertShapeNode(dart::dynamics::ShapeNode& shapeNode, ConvertShapeTypes loadType, Trade::AbstractImporter* importer) {
    dart::dynamics::ShapePtr shape = shapeNode.getShape();

    if(shape->getType() == dart::dynamics::ConeShape::getStaticType() ||
       shape->getType() == dart::dynamics::LineSegmentShape::getStaticType() ||
       shape->getType() == dart::dynamics::MultiSphereConvexHullShape::getStaticType() ||
       shape->getType() == dart::dynamics::PlaneShape::getStaticType()) {
        Error{} << "DartIntegration::convertShapeNode(): shape type" << shape->getType() << "is not supported";
        return Containers::NullOpt;
    }

    ShapeData shapeData{{}, {}, {}, {}};

    Trade::PhongMaterialData nodeMaterial{Trade::PhongMaterialData::Flags{}, 80.f};
    if(loadType & ConvertShapeType::Material) {
        /* Get material information -- we ignore the alpha value
        Note that this material is not necessarily used for the MeshShapeNodes */
        Eigen::Vector4d col = shapeNode.getVisualAspect()->getRGBA();
        /* @to-do: Create Trade Material Data that includes alpha channel */

        /* get diffuse color from Dart ShapeNode */
        nodeMaterial.diffuseColor() = Color3(col(0), col(1), col(2));
        /* default colors for ambient (black) and specular (white) */
        nodeMaterial.ambientColor() = Vector3{0.f, 0.f, 0.f};
        nodeMaterial.specularColor() = Vector3{1.f, 1.f, 1.f};

        if(shape->getType() != dart::dynamics::MeshShape::getStaticType()) {
            shapeData.materials = Containers::Array<Trade::PhongMaterialData>(Containers::NoInit, 1);
            new(&shapeData.materials[0]) Trade::PhongMaterialData{std::move(nodeMaterial)};
        }
    }

    if(shape->getType() == dart::dynamics::BoxShape::getStaticType()) {
        if(loadType & ConvertShapeType::Primitive) {
            auto boxShape = std::static_pointer_cast<dart::dynamics::BoxShape>(shape);
            Eigen::Vector3d size = boxShape->getSize();

            /* Multiplying by 0.5f because the Primitives::Cube is 2x2x2 */
            shapeData.scaling = Vector3(size(0), size(1), size(2)) * 0.5f;
        }

        if(loadType & ConvertShapeType::Mesh) {
            shapeData.meshes = Containers::Array<Trade::MeshData3D>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData3D{Primitives::Cube::solid()};
        }
    } else if(shape->getType() == dart::dynamics::CapsuleShape::getStaticType()) {
        auto capsuleShape = std::static_pointer_cast<dart::dynamics::CapsuleShape>(shape);

        Float r = Float(capsuleShape->getRadius());

        if(loadType & ConvertShapeType::Primitive)
            shapeData.scaling = Vector3{r};

        if(loadType & ConvertShapeType::Mesh) {
            Float h = Float(capsuleShape->getHeight());
            Float halfLength = 0.5f * h / r;

            shapeData.meshes = Containers::Array<Trade::MeshData3D>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData3D{Primitives::Capsule3D::solid(32, 32, 32, halfLength)};
        }
    } else if(shape->getType() == dart::dynamics::CylinderShape::getStaticType()) {
        auto cylinderShape = std::static_pointer_cast<dart::dynamics::CylinderShape>(shape);

        Float r = Float(cylinderShape->getRadius());

        if(loadType & ConvertShapeType::Primitive)
            shapeData.scaling = Vector3{r};

        if(loadType & ConvertShapeType::Mesh) {
            Float h = Float(cylinderShape->getHeight());
            Float halfLength = 0.5f * h / r;

            shapeData.meshes = Containers::Array<Trade::MeshData3D>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData3D{Primitives::Cylinder::solid(32, 32, halfLength, Primitives::Cylinder::Flag::CapEnds)};
        }
    } else if(shape->getType() == dart::dynamics::EllipsoidShape::getStaticType()) {
        if(loadType & ConvertShapeType::Primitive) {
            auto ellipsoidShape = std::static_pointer_cast<dart::dynamics::EllipsoidShape>(shape);

            Eigen::Vector3d size = ellipsoidShape->getDiameters().array() * 0.5;
            shapeData.scaling = Vector3(size(0), size(1), size(2));
        }

        if(loadType & ConvertShapeType::Mesh) {
            shapeData.meshes = Containers::Array<Trade::MeshData3D>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData3D{Primitives::Icosphere::solid(5)};
        }
    } else if(shape->getType() == dart::dynamics::MeshShape::getStaticType()) {
        if (!importer) {
            Error{} << "DartIntegration::convertShapeNode(): AssimpImporter is not available and you are trying to load a dart::dynamics::MeshShape";
            return Containers::NullOpt;
        }
        auto meshShape = std::static_pointer_cast<dart::dynamics::MeshShape>(shape);

        if(loadType & ConvertShapeType::Primitive) {
            /* @todo: check if scaling can be per mesh */
            Eigen::Vector3d scale = meshShape->getScale();
            shapeData.scaling = Vector3(scale(0), scale(1), scale(2));
        }

        const aiScene* aiMesh = meshShape->getMesh();
        std::string meshPath = Utility::Directory::path(meshShape->getMeshPath());

        bool loaded = importer->openState(aiMesh, meshPath);
        if(!loaded || importer->mesh3DCount() < 1) {
            Error{} << "DartIntegration::convertShapeNode(): Could not load aiScene or there is no mesh in it";
            return Containers::NullOpt;
        }

        /* Count mesh objects */
        UnsignedInt meshesCount = 0;
        for(UnsignedInt i = 0; i < importer->object3DCount(); i++) {
            auto objectData = importer->object3D(i);
            if(objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh) meshesCount++;
        }

        Containers::Array<Containers::Optional<Trade::MeshData3D>> meshes(meshesCount);
        Containers::Array<Containers::Optional<Trade::PhongMaterialData>> materials(meshesCount);

        UnsignedInt j = 0;
        for(UnsignedInt i = 0; i < importer->object3DCount(); i++) {
            auto objectData = importer->object3D(i);
            if(objectData->instanceType() != Trade::ObjectInstanceType3D::Mesh) continue;

            Trade::MeshObjectData3D& meshObjectData = static_cast<Trade::MeshObjectData3D&>(*objectData);
            Containers::Optional<Trade::MeshData3D> meshData = importer->mesh3D(meshObjectData.instance());
            if(!meshData){
                Error{} << "DartIntegration::convertShapeNode(): Could not load mesh with index" << meshObjectData.instance();
                return Containers::NullOpt;
            }

            if(loadType & ConvertShapeType::Material) {
                auto colorMode = meshShape->getColorMode();
                /* only get materials from mesh if the appropriate color mode */
                if(importer->materialCount() && loadType & ConvertShapeType::Material) {
                    if(colorMode == dart::dynamics::MeshShape::ColorMode::MATERIAL_COLOR) {
                        auto matPtr = importer->material(meshObjectData.material());
                        if(matPtr)
                            materials[j] = std::move(*static_cast<Trade::PhongMaterialData*>(matPtr.get()));
                        else {
                            Warning{} << "DartIntegration::convertShapeNode(): Could not load material with index" << meshObjectData.material() << Debug::nospace << ".Falling back to SHAPE_COLOR mode";
                            materials[j] = std::move(nodeMaterial);
                        }
                    }
                    else if(colorMode == dart::dynamics::MeshShape::ColorMode::COLOR_INDEX) {
                        /* get diffuse color from Mesh color */
                        if(meshData->hasColors()) {
                            /* use max index if MeshShape color index is bigger than available;
                             * this is the behavior described in Dart
                             */
                            Int colorIndex = (static_cast<UnsignedInt>(meshShape->getColorIndex())>=meshData->colors(0).size()) ? meshData->colors(0).size()-1: meshShape->getColorIndex();
                            Color4 meshColor = meshData->colors(0)[colorIndex];
                            materials[j] = Trade::PhongMaterialData{Trade::PhongMaterialData::Flags{}, 80.f};
                            materials[j]->diffuseColor() = Color3(meshColor[0], meshColor[1], meshColor[2]);
                            /* default colors for ambient (black) and specular (white) */
                            materials[j]->ambientColor() = Vector3{0.f, 0.f, 0.f};
                            materials[j]->specularColor() = Vector3{1.f, 1.f, 1.f};
                        }
                        /* fallback to SHAPE_COLOR if MeshData has no colors */
                        else {
                            Warning{} << "DartIntegration::convertShapeNode(): Assimp mesh has no colors. Falling back to SHAPE_COLOR mode";
                            materials[j] = std::move(nodeMaterial);
                        }
                    }
                    else if (colorMode == dart::dynamics::MeshShape::ColorMode::SHAPE_COLOR) {
                        materials[j] = std::move(nodeMaterial);
                    }
                }
            }

            if(loadType & ConvertShapeType::Mesh) {
                meshes[j] = std::move(*meshData);
            }

            j++;
        }

        Containers::Array<Containers::Optional<Trade::TextureData>> textures(importer->textureCount());
        Containers::Array<Containers::Optional<Trade::ImageData2D>> images(importer->textureCount());

        if(loadType & ConvertShapeType::Material) {
            for(UnsignedInt i = 0; i < importer->textureCount(); ++i) {
                /* Cannot load, leave this element set to NullOpt */
                Containers::Optional<Trade::TextureData> textureData = importer->texture(i);
                if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
                    Warning{} << "Cannot load texture, skipping";
                    continue;
                }

                /* Cannot load, leave this element set to NullOpt */
                Containers::Optional<Trade::ImageData2D> imageData = importer->image2D(textureData->image());
                if (!imageData) {
                    Warning{} << "Cannot load texture image, skipping";
                    continue;
                }

                textures[i] = std::move(textureData);
                images[i] = std::move(imageData);
            }
        }

        if(loadType & ConvertShapeType::Mesh) {
            shapeData.meshes = Containers::Array<Trade::MeshData3D>(Containers::NoInit, meshes.size());
            for(UnsignedInt m = 0; m < meshes.size(); m++)
                new(&shapeData.meshes[m]) Trade::MeshData3D{std::move(*meshes[m])};
        }

        if(loadType & ConvertShapeType::Material) {
            shapeData.materials = Containers::Array<Trade::PhongMaterialData>(Containers::NoInit, materials.size());
            for(UnsignedInt m = 0; m < materials.size(); m++)
                new(&shapeData.materials[m]) Trade::PhongMaterialData{std::move(*materials[m])};
            shapeData.images = std::move(images);
            shapeData.textures = std::move(textures);
        }

        /* Close any file if opened */
        importer->close();
    } else if(loadType & ConvertShapeType::Mesh && shape->getType() == dart::dynamics::SoftMeshShape::getStaticType()) {
        /* Soft meshes should be drawn with face culling */
        auto meshShape = std::static_pointer_cast<dart::dynamics::SoftMeshShape>(shape);

        const dart::dynamics::SoftBodyNode* bn = meshShape->getSoftBodyNode();

        std::vector<UnsignedInt> indices;
        std::vector<std::vector<Vector3>> positions, normals;
        positions.push_back(std::vector<Vector3>());
        normals.push_back(std::vector<Vector3>());

        /* Get vertex positions from SoftBody */
        for(UnsignedInt i = 0; i < bn->getNumPointMasses(); ++i) {
            const Eigen::Vector3d& pos = bn->getPointMass(i)->getLocalPosition();
            positions[0].push_back(Vector3(pos(0), pos(1), pos(2)));
        }

        /* Add each face twice; once with the original orientation
         * and once with reversed orientation
         */
        for(UnsignedInt i = 0; i < bn->getNumFaces(); ++i) {
            const Eigen::Vector3i& F = bn->getFace(i);
            /* add original face */
            indices.push_back(F[0]);
            indices.push_back(F[1]);
            indices.push_back(F[2]);
            /* add reversed face */
            indices.push_back(F[2]);
            indices.push_back(F[1]);
            indices.push_back(F[0]);
        }

        /* Generate flat normals */
        std::vector<UnsignedInt> normalIndices;
        std::tie(normalIndices, normals[0]) = MeshTools::generateFlatNormals(indices, positions[0]);

        /* Combine normal and vertex indices */
        std::vector<UnsignedInt> finalIndices = MeshTools::combineIndexedArrays(
            std::make_pair(std::cref(indices), std::ref(positions[0])),
            std::make_pair(std::cref(normalIndices), std::ref(normals[0]))
        );

        /* Create the mesh data */
        Trade::MeshData3D meshData{MeshPrimitive::Triangles, finalIndices, positions, normals, std::vector<std::vector<Vector2>>(), std::vector<std::vector<Color4>>()};

        shapeData.meshes = Containers::Array<Trade::MeshData3D>(Containers::NoInit, 1);
        new(&shapeData.meshes[0]) Trade::MeshData3D{std::move(meshData)};
    } else if(shape->getType() == dart::dynamics::SphereShape::getStaticType()) {
        if(loadType & ConvertShapeType::Primitive) {
            auto sphereShape = std::static_pointer_cast<dart::dynamics::SphereShape>(shape);

            Float r = Float(sphereShape->getRadius());
            shapeData.scaling = Vector3{r};
        }

        if(loadType & ConvertShapeType::Mesh) {
            shapeData.meshes = Containers::Array<Trade::MeshData3D>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData3D{Primitives::Icosphere::solid(4)};
        }
    }

    return std::move(shapeData);
}

}}