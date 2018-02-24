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

#include "World.h"

#include <Magnum/Buffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/Texture.h>
#include <Magnum/Trade/PhongMaterialData.h>

namespace Magnum { namespace DartIntegration {
World::World(SceneGraph::AbstractBasicObject3D<Float>& object, std::shared_ptr<dart::simulation::World> world): _object(object), _manager{MAGNUM_PLUGINS_IMPORTER_DIR}, _dartWorld(*world) {
    /* load Assimp importer */
    _importer = _manager.loadAndInstantiate("AssimpImporter");
}

World::~World() = default;

World& World::refresh() {
    /* clear update flags */
    for(auto& obj: _dartToMagnum)
        obj.second->clearUpdateFlag();

    /* parse all skeletons in _dartWorld */
    for(size_t i = 0; i < _dartWorld.getNumSkeletons(); i++) {
        parseSkeleton(_object, *_dartWorld.getSkeleton(i));
    }

    /* remove unused objects */
    clearUnusedObjects();

    return *this;
}

World& World::step() {
    _dartWorld.step();
    return *this;
}

World& World::clearUnusedObjects() {
    std::vector<dart::dynamics::Frame*> unusedFrames;

    /* Find unused objects */
    for(auto& object_pair: _dartToMagnum)
    {
        if(object_pair.second && !object_pair.second->isUpdated())
            unusedFrames.push_back(object_pair.first);
    }

    /* Clear unused Objects */
    _toRemove.clear();
    for(dart::dynamics::Frame* frame: unusedFrames)
    {
        auto it = _dartToMagnum.find(frame);
        _toRemove.emplace_back(std::move(it->second));
        _dartToMagnum.erase(it);
    }

    return *this;
}

std::vector<std::reference_wrapper<Object>> World::unusedObjects() {
    std::vector<std::reference_wrapper<Object>> objs;
    for(auto& obj: _toRemove)
        objs.emplace_back(std::ref(*obj));

    return objs;
}

std::vector<std::reference_wrapper<Object>> World::objects() {
    std::vector<std::reference_wrapper<Object>> objs;
    for(auto& obj: _dartToMagnum)
        objs.emplace_back(std::ref(*obj.second));

    return objs;
}

std::vector<std::reference_wrapper<Object>> World::shapeObjects() {
    std::vector<std::reference_wrapper<Object>> objs;
    for(auto& obj: _dartToMagnum)
        if(obj.second->shapeNode())
            objs.emplace_back(std::ref(*obj.second));

    return objs;
}

std::vector<Object*> World::updatedShapeObjects() {
    std::vector<Object*> objs;
    objs.insert(objs.end(), _updatedShapeObjects.begin(), _updatedShapeObjects.end());
    return objs;
}

World& World::clearUpdatedShapeObjects() {
    _updatedShapeObjects.clear();
    return *this;
}

std::vector<std::reference_wrapper<Object>> World::bodyObjects() {
    std::vector<std::reference_wrapper<Object>> objs;
    for(auto& obj: _dartToMagnum)
        if(obj.second->bodyNode())
            objs.emplace_back(std::ref(*obj.second));

    return objs;
}

void World::parseSkeleton(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::Skeleton& skel){
    for(size_t i = 0; i < skel.getNumTrees(); i++) {
        parseBodyNodeRecursive(parent, *skel.getRootBodyNode(i));
    }
}

void World::parseBodyNodeRecursive(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode& bn) {
    /** parse the BodyNode
     * we care only about visuals
     */
    auto& visualShapes = bn.getShapeNodesWith<dart::dynamics::VisualAspect>();

    /* create an object of the BodyNode to keep track of transformations */
    SceneGraph::AbstractBasicObject3D<Float>* object = nullptr;
    auto it = _dartToMagnum.insert(std::make_pair(static_cast<dart::dynamics::Frame*>(&bn), nullptr));
    if (it.second) {
        object = objectCreator(parent);
        it.first->second = dartObjectCreator(*object, &bn);
    }
    else
        object = static_cast<SceneGraph::AbstractBasicObject3D<Float>*>(&it.first->second->object());
    it.first->second->update();
    for (auto& shape: visualShapes) {
        auto it = _dartToMagnum.insert(std::make_pair(static_cast<dart::dynamics::Frame*>(shape), nullptr));
        if (it.second) {
            /* create object for the ShapeNode to keep track of inner transformations */
            auto shapeObj = objectCreator(*object);
            it.first->second = dartShapeObjectCreator(*shapeObj, shape);
        }
        it.first->second->update(_importer.get());
        if(it.first->second->hasUpdatedMesh())
            _updatedShapeObjects.insert(it.first->second.get());
    }

    /* parse the children recursively */
    std::size_t numChilds = bn.getNumChildBodyNodes();
    for (std::size_t i = 0; i < numChilds; i++) {
        /* pass as parent the newly created object */
        parseBodyNodeRecursive(*object, *bn.getChildBodyNode(i));
    }
}

}}