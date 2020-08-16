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

#include "Object.h"

#include <dart/dynamics/BodyNode.hpp>
#include <dart/dynamics/MeshShape.hpp>
#include <dart/dynamics/ShapeNode.hpp>
#include <dart/dynamics/SoftMeshShape.hpp>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/TextureData.h>

#include "Magnum/DartIntegration/ConvertShapeNode.h"
#include "Magnum/EigenIntegration/GeometryIntegration.h"

namespace Magnum { namespace DartIntegration {

DrawData::DrawData(Containers::Array<GL::Mesh> meshes, Containers::Array<Trade::PhongMaterialData> materials, Containers::Array<Containers::Optional<GL::Texture2D>> textures, const Vector3& scaling): meshes{std::move(meshes)}, materials{std::move(materials)}, textures{std::move(textures)}, scaling(scaling) {}

DrawData::~DrawData() = default;

Object::Object(SceneGraph::AbstractBasicObject3D<Float>& object, SceneGraph::AbstractBasicTranslationRotation3D<Float>& transformation, dart::dynamics::ShapeNode* node, dart::dynamics::BodyNode* body): SceneGraph::AbstractBasicFeature3D<Float>{object}, _transformation(transformation), _node{node}, _body{body}, _updated(false), _updatedMesh(false) {}

Object& Object::update(Trade::AbstractImporter* importer) {
    using namespace Math::Literals;
    /* If the object is has a shape and could not extract the DrawData, do not
       update it; i.e., the user can choose to delete it */
    if(_node && !extractDrawData(importer))
        return *this;

    /* Get transform from DART */
    Matrix4 trans;
    if(!_node)
        trans = Matrix4(Matrix4d(_body->getRelativeTransform()));
    else
        trans = Matrix4(Matrix4d(_node->getRelativeTransform()));

    /* Check if any value is NaN */
    if(Math::isNan(trans.toVector()).any()) {
        Warning{} << "DartIntegration::Object::update(): Received NaN values from DART. Ignoring this update.";
        return *this;
    }

    Quaternion quat = Quaternion::fromMatrix(trans.rotationScaling());
    Vector3 axis = quat.axis();
    Rad angle = quat.angle();
    if(Math::abs(angle) <= 1e-5_radf) {
        axis = {1.f, 0.f, 0.f};
    }
    axis = axis.normalized();
    Vector3 t = trans.translation();

    /* Pass it to Magnum */
    _transformation.resetTransformation()
        .rotate(angle, axis)
        .translate(t);

    /* Set update flag */
    _updated = true;
    return *this;
}

bool Object::extractDrawData(Trade::AbstractImporter* importer) {
    _updatedMesh = false;

    /* This is not a valid object */
    if(!_node && !_body) return false;

    /* This object has no shape */
    if(!_node) return true;

    UnsignedInt dataVariance = _node->getShape()->getDataVariance();

    if(_drawData && dataVariance == dart::dynamics::Shape::DataVariance::STATIC)
        return true;

    bool firstTime = !_drawData;

    dart::dynamics::ShapeNode& shapeNode = *this->shapeNode();
    dart::dynamics::ShapePtr shape = shapeNode.getShape();

    ConvertShapeTypes loadType;
    if(firstTime)
        loadType = ConvertShapeType::All;
    else {
        if(shape->checkDataVariance(dart::dynamics::Shape::DataVariance::DYNAMIC_COLOR))
            loadType |= ConvertShapeType::Material;
        if(shape->checkDataVariance(dart::dynamics::Shape::DataVariance::DYNAMIC_PRIMITIVE))
            loadType |= ConvertShapeType::Primitive;
        if(shape->checkDataVariance(dart::dynamics::Shape::DataVariance::DYNAMIC_VERTICES) ||
           shape->checkDataVariance(dart::dynamics::Shape::DataVariance::DYNAMIC_ELEMENTS) ||
           shape->checkDataVariance(dart::dynamics::Shape::DataVariance::DYNAMIC))
            loadType |= ConvertShapeType::Mesh;
    }

    Containers::Optional<ShapeData> shapeData = convertShapeNode(shapeNode, loadType, importer);

    /* Could not convertShapeNode to ShapeData */
    if(!shapeData) return false;

    /* Create the DrawData structure, default scaling to identity */
    if(firstTime) _drawData = DrawData{{}, {}, {}, Vector3{1.0f}};

    /* Get the material */
    if(loadType & ConvertShapeType::Material) {
        /* Copy material data */
        _drawData->materials = std::move(shapeData->materials);

        /* Create textures */
        _drawData->textures = Containers::Array<Containers::Optional<GL::Texture2D>>(shapeData->textures.size());
        for(UnsignedInt i = 0; i < shapeData->textures.size(); i++) {
            /* This is to preserve indexing for materials */
            if(!shapeData->textures[i] || !shapeData->images[i]) continue;

            (*(_drawData->textures[i] = GL::Texture2D{}))
                .setMagnificationFilter(shapeData->textures[i]->magnificationFilter())
                .setMinificationFilter(shapeData->textures[i]->minificationFilter(), shapeData->textures[i]->mipmapFilter())
                .setWrapping(shapeData->textures[i]->wrapping().xy())
                .setStorage(1, GL::TextureFormat::RGB8, shapeData->images[i]->size())
                .setSubImage(0, {}, *shapeData->images[i])
                .generateMipmap();
        }
    }

    /* Get scaling */
    if(loadType & ConvertShapeType::Primitive)
        _drawData->scaling = shapeData->scaling;

    /* Get meshes */
    if(loadType & ConvertShapeType::Mesh) {
        _drawData->meshes = Containers::Array<GL::Mesh>(Containers::NoInit, shapeData->meshes.size());
        for(UnsignedInt i = 0; i < shapeData->meshes.size(); i++)
            new(&_drawData->meshes[i]) GL::Mesh{MeshTools::compile(shapeData->meshes[i])};
    }

    /* If we got here, everything went OK; update flag for rendering */
    _updatedMesh = !!loadType;
    return true;
}

}}
