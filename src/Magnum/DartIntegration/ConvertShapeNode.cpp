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
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Magnum/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/GenerateNormals.h>
#include <Magnum/MeshTools/Transform.h>
#include <Magnum/Primitives/Capsule.h>
#include <Magnum/Primitives/Cone.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Cylinder.h>
#include <Magnum/Primitives/Icosphere.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/TextureData.h>

#include "Magnum/EigenIntegration/Integration.h"

namespace Magnum { namespace DartIntegration {

using namespace Math::Literals;

ShapeData::ShapeData(Containers::Array<Trade::MeshData> meshes, Containers::Array<Trade::PhongMaterialData> materials, Containers::Array<Containers::Optional<Trade::ImageData2D>> images, Containers::Array<Containers::Optional<Trade::TextureData>> textures, const Vector3& scaling): meshes{std::move(meshes)}, materials{std::move(materials)}, images{std::move(images)}, textures{std::move(textures)}, scaling(scaling) {}

ShapeData::~ShapeData() = default;

Containers::Optional<ShapeData> convertShapeNode(dart::dynamics::ShapeNode& shapeNode, ConvertShapeTypes convertTypes, Trade::AbstractImporter* importer) {
    dart::dynamics::ShapePtr shape = shapeNode.getShape();

    if(shape->getType() == dart::dynamics::LineSegmentShape::getStaticType() ||
       shape->getType() == dart::dynamics::MultiSphereConvexHullShape::getStaticType() ||
       shape->getType() == dart::dynamics::PlaneShape::getStaticType()) {
        Error{} << "DartIntegration::convertShapeNode(): shape type" << shape->getType() << "is not supported";
        return Containers::NullOpt;
    }

    ShapeData shapeData{{}, {}, {}, {}, Vector3{1.0f}};

    Containers::Array<Trade::MaterialAttributeData> nodeMaterialAttributes;
    if(convertTypes & ConvertShapeType::Material) {
        /* Get material information -- we ignore the alpha value. Note that
           this material is not necessarily used for the MeshShapeNodes */
        Eigen::Vector4d col = shapeNode.getVisualAspect()->getRGBA();

        /* Get diffuse color from Dart ShapeNode */
        arrayAppend(nodeMaterialAttributes, Containers::InPlaceInit,
            Trade::MaterialAttribute::DiffuseColor,
            Color4(col(0), col(1), col(2), col(3)));

        /* Remove specular color from soft bodies (otherwise the default would
           be white) */
        if(shape->getType() == dart::dynamics::SoftMeshShape::getStaticType())
            arrayAppend(nodeMaterialAttributes, Containers::InPlaceInit,
                Trade::MaterialAttribute::SpecularColor, 0x00000000_rgbaf);
    }

    /* Larger shininess than the default to make things look less plastic */
    arrayAppend(nodeMaterialAttributes, Containers::InPlaceInit,
        Trade::MaterialAttribute::Shininess, 2000.0f);

    Trade::PhongMaterialData nodeMaterial{Trade::MaterialType::Phong, std::move(nodeMaterialAttributes)};

    if(convertTypes & ConvertShapeType::Material) {
        if(shape->getType() != dart::dynamics::MeshShape::getStaticType()) {
            shapeData.materials = Containers::Array<Trade::PhongMaterialData>(Containers::NoInit, 1);
            new(&shapeData.materials[0]) Trade::PhongMaterialData{std::move(nodeMaterial)};
        }
    }

    /* Box */
    if(shape->getType() == dart::dynamics::BoxShape::getStaticType()) {
        if(convertTypes & ConvertShapeType::Primitive) {
            auto boxShape = std::static_pointer_cast<dart::dynamics::BoxShape>(shape);
            Eigen::Vector3d size = boxShape->getSize();

            /* Multiplying by 0.5f because the Primitives::Cube is 2x2x2 */
            shapeData.scaling = Vector3(size(0), size(1), size(2))*0.5f;
        }

        if(convertTypes & ConvertShapeType::Mesh) {
            shapeData.meshes = Containers::Array<Trade::MeshData>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData{Primitives::cubeSolid()};
        }

    /* Capsule */
    } else if(shape->getType() == dart::dynamics::CapsuleShape::getStaticType()) {
        auto capsuleShape = std::static_pointer_cast<dart::dynamics::CapsuleShape>(shape);

        Float r = Float(capsuleShape->getRadius());

        if(convertTypes & ConvertShapeType::Primitive)
            shapeData.scaling = Vector3{r};

        if(convertTypes & ConvertShapeType::Mesh) {
            Float h(capsuleShape->getHeight());
            Float halfLength = 0.5f*h/r;

            shapeData.meshes = Containers::Array<Trade::MeshData>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData{Primitives::capsule3DSolid(32, 32, 32, halfLength)};

            Matrix4 rot = Matrix4::rotationX(90.0_degf);
            MeshTools::transformVectorsInPlace(rot, shapeData.meshes[0].mutableAttribute<Vector3>(Trade::MeshAttribute::Position));
            MeshTools::transformVectorsInPlace(rot, shapeData.meshes[0].mutableAttribute<Vector3>(Trade::MeshAttribute::Normal));
        }

    /* Cone */
    } else if(shape->getType() == dart::dynamics::ConeShape::getStaticType()) {
        auto coneShape = std::static_pointer_cast<dart::dynamics::ConeShape>(shape);

        Float r = Float(coneShape->getRadius());

        if(convertTypes & ConvertShapeType::Primitive)
            shapeData.scaling = Vector3{r};

        if(convertTypes & ConvertShapeType::Mesh) {
            Float h(coneShape->getHeight());
            Float halfLength = 0.5f*h/r;

            shapeData.meshes = Containers::Array<Trade::MeshData>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData{Primitives::coneSolid(32, 32, halfLength, Primitives::ConeFlag::CapEnd)};

            Matrix4 rot = Matrix4::rotationX(90.0_degf);
            MeshTools::transformVectorsInPlace(rot, shapeData.meshes[0].mutableAttribute<Vector3>(Trade::MeshAttribute::Position));
            MeshTools::transformVectorsInPlace(rot, shapeData.meshes[0].mutableAttribute<Vector3>(Trade::MeshAttribute::Normal));
        }

    /* Cylinder */
    } else if(shape->getType() == dart::dynamics::CylinderShape::getStaticType()) {
        auto cylinderShape = std::static_pointer_cast<dart::dynamics::CylinderShape>(shape);

        Float r = Float(cylinderShape->getRadius());

        if(convertTypes & ConvertShapeType::Primitive)
            shapeData.scaling = Vector3{r};

        if(convertTypes & ConvertShapeType::Mesh) {
            Float h(cylinderShape->getHeight());
            Float halfLength = 0.5f*h/r;

            shapeData.meshes = Containers::Array<Trade::MeshData>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData{Primitives::cylinderSolid(32, 32, halfLength, Primitives::CylinderFlag::CapEnds)};

            Matrix4 rot = Matrix4::rotationX(90.0_degf);
            MeshTools::transformVectorsInPlace(rot, shapeData.meshes[0].mutableAttribute<Vector3>(Trade::MeshAttribute::Position));
            MeshTools::transformVectorsInPlace(rot, shapeData.meshes[0].mutableAttribute<Vector3>(Trade::MeshAttribute::Normal));
        }

    /* Ellipsoid */
    } else if(shape->getType() == dart::dynamics::EllipsoidShape::getStaticType()) {
        if(convertTypes & ConvertShapeType::Primitive) {
            auto ellipsoidShape = std::static_pointer_cast<dart::dynamics::EllipsoidShape>(shape);

            Eigen::Vector3d size = ellipsoidShape->getDiameters().array() * 0.5;
            shapeData.scaling = Vector3(size(0), size(1), size(2));
        }

        if(convertTypes & ConvertShapeType::Mesh) {
            shapeData.meshes = Containers::Array<Trade::MeshData>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData{Primitives::icosphereSolid(5)};
        }

    /* Generic mesh */
    } else if(shape->getType() == dart::dynamics::MeshShape::getStaticType()) {
        if(!importer) {
            Error{} << "DartIntegration::convertShapeNode(): AssimpImporter is not available and you are trying to load a dart::dynamics::MeshShape";
            return Containers::NullOpt;
        }
        auto meshShape = std::static_pointer_cast<dart::dynamics::MeshShape>(shape);

        if(convertTypes & ConvertShapeType::Primitive) {
            /** @todo check if scaling can be per mesh */
            Eigen::Vector3d scale = meshShape->getScale();
            shapeData.scaling = Vector3(scale(0), scale(1), scale(2));
        }

        const aiScene* aiMesh = meshShape->getMesh();
        std::string meshPath = Utility::Directory::path(meshShape->getMeshPath());

        bool loaded = importer->openState(aiMesh, meshPath);
        if(!loaded || importer->meshCount() < 1) {
            Error{} << "DartIntegration::convertShapeNode(): could not load aiScene or there is no mesh in it";
            return Containers::NullOpt;
        }

        /* Count mesh objects */
        UnsignedInt meshesCount = 0;
        for(UnsignedInt i = 0; i < importer->object3DCount(); i++) {
            auto objectData = importer->object3D(i);
            if(objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh) meshesCount++;
        }

        Containers::Array<Containers::Optional<Trade::MeshData>> meshes(meshesCount);
        Containers::Array<Containers::Optional<Trade::PhongMaterialData>> materials(meshesCount);

        UnsignedInt j = 0;
        for(UnsignedInt i = 0; i < importer->object3DCount(); i++) {
            auto objectData = importer->object3D(i);
            if(objectData->instanceType() != Trade::ObjectInstanceType3D::Mesh)
                continue;

            Trade::MeshObjectData3D& meshObjectData = static_cast<Trade::MeshObjectData3D&>(*objectData);
            Containers::Optional<Trade::MeshData> meshData = importer->mesh(meshObjectData.instance());
            if(!meshData) {
                Error{} << "DartIntegration::convertShapeNode(): could not load mesh with index" << meshObjectData.instance();
                return Containers::NullOpt;
            }

            if(convertTypes & ConvertShapeType::Material) {
                auto colorMode = meshShape->getColorMode();
                /* Only get materials from mesh if the appropriate color mode */
                if(importer->materialCount() && convertTypes & ConvertShapeType::Material) {
                    if(colorMode == dart::dynamics::MeshShape::ColorMode::MATERIAL_COLOR) {
                        Containers::Optional<Trade::MaterialData> material = importer->material(meshObjectData.material());
                        if(material) {
                            materials[j] = std::move(*material).as<Trade::PhongMaterialData>();
                        } else {
                            Warning{} << "DartIntegration::convertShapeNode(): could not load material with index" << meshObjectData.material() << Debug::nospace << ". Falling back to SHAPE_COLOR mode";
                            materials[j] = std::move(nodeMaterial);
                        }

                    } else if(colorMode == dart::dynamics::MeshShape::ColorMode::COLOR_INDEX) {
                        /* Get diffuse color from Mesh color */
                        if(meshData->hasAttribute(Trade::MeshAttribute::Color)) {
                            /* Use max index if MeshShape color index is bigger
                               than available; this is the behavior described
                               in DART */
                            Int colorIndex = (UnsignedInt(meshShape->getColorIndex()) >= meshData->vertexCount()) ? meshData->vertexCount() - 1 : meshShape->getColorIndex();
                            /* Assuming AssimpImporter, which always returns
                               colors as Color4 */
                            CORRADE_INTERNAL_ASSERT(meshData->attributeFormat(Trade::MeshAttribute::Color) == VertexFormat::Vector4);
                            Color4 meshColor = meshData->attribute<Color4>(Trade::MeshAttribute::Color)[colorIndex];
                            /* default colors for ambient (black) and specular
                               (white) */
                            materials[j] = Trade::PhongMaterialData{
                                Trade::MaterialType::Phong,
                                {{Trade::MaterialAttribute::DiffuseColor, meshColor},
                                 {Trade::MaterialAttribute::Shininess, 2000.0f}}};

                        /* Fallback to SHAPE_COLOR if MeshData has no colors */
                        } else {
                            Warning{} << "DartIntegration::convertShapeNode(): Assimp mesh has no colors. Falling back to SHAPE_COLOR mode";
                            materials[j] = std::move(nodeMaterial);
                        }

                    } else if (colorMode == dart::dynamics::MeshShape::ColorMode::SHAPE_COLOR) {
                        materials[j] = std::move(nodeMaterial);
                    }
                }
            }

            if(convertTypes & ConvertShapeType::Mesh) {
                meshes[j] = std::move(*meshData);
            }

            ++j;
        }

        Containers::Array<Containers::Optional<Trade::TextureData>> textures(importer->textureCount());
        Containers::Array<Containers::Optional<Trade::ImageData2D>> images(importer->textureCount());

        if(convertTypes & ConvertShapeType::Material) {
            for(UnsignedInt i = 0; i < importer->textureCount(); ++i) {
                /* Cannot load, leave this element set to NullOpt */
                Containers::Optional<Trade::TextureData> textureData = importer->texture(i);
                if(!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
                    Warning{} << "DartIntegration::convertShapeNode(): cannot load texture, skipping";
                    continue;
                }

                /* Cannot load, leave this element set to NullOpt */
                Containers::Optional<Trade::ImageData2D> imageData = importer->image2D(textureData->image());
                if(!imageData) {
                    Warning{} << "DartIntegration::convertShapeNode(): cannot load texture image, skipping";
                    continue;
                }

                textures[i] = std::move(textureData);
                images[i] = std::move(imageData);
            }
        }

        if(convertTypes & ConvertShapeType::Mesh) {
            shapeData.meshes = Containers::Array<Trade::MeshData>(Containers::NoInit, meshes.size());
            for(UnsignedInt m = 0; m < meshes.size(); m++)
                new(&shapeData.meshes[m]) Trade::MeshData{std::move(*meshes[m])};
        }

        if(convertTypes & ConvertShapeType::Material) {
            shapeData.materials = Containers::Array<Trade::PhongMaterialData>(Containers::NoInit, materials.size());
            for(UnsignedInt m = 0; m < materials.size(); m++)
                new(&shapeData.materials[m]) Trade::PhongMaterialData{std::move(*materials[m])};
            shapeData.images = std::move(images);
            shapeData.textures = std::move(textures);
        }

        /* Close any file if opened */
        importer->close();

    /* Soft mesh */
    } else if((convertTypes & ConvertShapeType::Mesh) && shape->getType() == dart::dynamics::SoftMeshShape::getStaticType()) {
        auto meshShape = std::static_pointer_cast<dart::dynamics::SoftMeshShape>(shape);

        const dart::dynamics::SoftBodyNode* bn = meshShape->getSoftBodyNode();

        /* Prepare vertex data */
        struct Vertex {
            Vector3 position;
            Vector3 normal;
        };
        Containers::Array<char> vertexData{Containers::NoInit,
            sizeof(Vertex)*bn->getNumPointMasses()};
        auto vertices = Containers::arrayCast<Vertex>(vertexData);
        Containers::StridedArrayView1D<Vector3> positions{vertexData,
            &vertices[0].position, bn->getNumPointMasses(), sizeof(Vertex)};
        Containers::StridedArrayView1D<Vector3> normals{vertexData,
            &vertices[0].normal, bn->getNumPointMasses(), sizeof(Vertex)};

        /* Get vertex positions from SoftBody */
        for(UnsignedInt i = 0; i < bn->getNumPointMasses(); ++i) {
            const Eigen::Vector3d& pos = bn->getPointMass(i)->getLocalPosition();
            positions[i] = Vector3{Vector3d{pos}};
        }

        /* Create indices */
        Containers::Array<char> indexData{Containers::NoInit,
            sizeof(UnsignedInt)*bn->getNumFaces()*3};
        auto indices = Containers::arrayCast<UnsignedInt>(indexData);
        for(UnsignedInt i = 0; i < bn->getNumFaces(); ++i) {
            const Eigen::Vector3i& F = bn->getFace(i);
            indices[i*3 + 0] = F[0];
            indices[i*3 + 1] = F[1];
            indices[i*3 + 2] = F[2];
        }

        /* Generate smooth normals */
        MeshTools::generateSmoothNormalsInto(Containers::StridedArrayView1D<const UnsignedInt>{indices}, positions, normals);

        /* Create the mesh data */
        shapeData.meshes = Containers::Array<Trade::MeshData>(Containers::NoInit, 1);
        new(&shapeData.meshes[0]) Trade::MeshData{MeshPrimitive::Triangles,
            std::move(indexData), Trade::MeshIndexData{indices},
            std::move(vertexData), {
                Trade::MeshAttributeData{Trade::MeshAttribute::Position, positions},
                Trade::MeshAttributeData{Trade::MeshAttribute::Normal, normals},
            }};

    /* Sphere */
    } else if(shape->getType() == dart::dynamics::SphereShape::getStaticType()) {
        if(convertTypes & ConvertShapeType::Primitive) {
            auto sphereShape = std::static_pointer_cast<dart::dynamics::SphereShape>(shape);

            Float r = Float(sphereShape->getRadius());
            shapeData.scaling = Vector3{r};
        }

        if(convertTypes & ConvertShapeType::Mesh) {
            shapeData.meshes = Containers::Array<Trade::MeshData>(Containers::NoInit, 1);
            new(&shapeData.meshes[0]) Trade::MeshData{Primitives::icosphereSolid(4)};
        }
    }

    /* Needs to be explicit on GCC 4.8 and Clang 3.8 so it can properly upcast
       the pointer. Just std::move() works as well, but that gives a warning
       on GCC 9. */
    return Containers::optional(std::move(shapeData));
}

}}
