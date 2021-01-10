/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
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

#include "World.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <Corrade/PluginManager/Manager.h>
#include <dart/dynamics/BodyNode.hpp>
#include <dart/dynamics/ShapeNode.hpp>
#include <dart/simulation/World.hpp>

namespace Magnum { namespace DartIntegration {

struct World::State {
    State(SceneGraph::AbstractBasicObject3D<Float>& object, dart::simulation::World& dartWorld): object(object), dartWorld(dartWorld) {}

    SceneGraph::AbstractBasicObject3D<Float>& object;
    Containers::Optional<PluginManager::Manager<Trade::AbstractImporter>> managerStorage;
    PluginManager::Manager<Trade::AbstractImporter>* manager;
    Containers::Pointer<Trade::AbstractImporter> importer;
    dart::simulation::World& dartWorld;
    std::unordered_map<dart::dynamics::Frame*, std::unique_ptr<Object>> dartToMagnum;
    std::vector<std::unique_ptr<Object>> toRemove;
    std::unordered_set<Object*> updatedShapeObjects;
};

World::World(PluginManager::Manager<Trade::AbstractImporter>* manager, SceneGraph::AbstractBasicObject3D<Float>& object, dart::simulation::World& world): _state{new State{object, world}} {
    /* If the manager is not passed from outside, maintain our own instance */
    if(!manager) {
        _state->managerStorage.emplace();
        _state->manager = &*_state->managerStorage;
    } else _state->manager = manager;

    /* Load Assimp importer */
    _state->importer = _state->manager->loadAndInstantiate("AssimpImporter");
}

World::~World() = default;

World& World::refresh() {
    /* Clear update flags */
    for(auto& obj: _state->dartToMagnum)
        obj.second->clearUpdateFlag();

    /* Parse all skeletons in _dartWorld */
    for(size_t i = 0; i < _state->dartWorld.getNumSkeletons(); ++i) {
        for(size_t j = 0; j < _state->dartWorld.getSkeleton(i)->getNumTrees(); ++j)
            parseBodyNodeRecursive(_state->object, *_state->dartWorld.getSkeleton(i)->getRootBodyNode(j));
    }

    /* Find unused objects */
    std::vector<dart::dynamics::Frame*> unusedFrames;
    for(auto& object_pair: _state->dartToMagnum) {
        if(object_pair.second && !object_pair.second->isUpdated())
            unusedFrames.push_back(object_pair.first);
    }

    /* Clear unused objects */
    _state->toRemove.clear();
    for(dart::dynamics::Frame* frame: unusedFrames) {
        auto it = _state->dartToMagnum.find(frame);
        _state->toRemove.emplace_back(std::move(it->second));
        _state->dartToMagnum.erase(it);
    }

    return *this;
}

World& World::step(bool resetCommand) {
    _state->dartWorld.step(resetCommand);
    return *this;
}

std::vector<std::unique_ptr<Object>>& World::unusedObjects() {
    return _state->toRemove;
}

std::vector<std::reference_wrapper<Object>> World::objects() {
    std::vector<std::reference_wrapper<Object>> objs;
    objs.reserve(_state->dartToMagnum.size());
    for(auto& obj: _state->dartToMagnum)
        objs.emplace_back(*obj.second);

    return objs;
}

std::vector<std::reference_wrapper<Object>> World::shapeObjects() {
    std::vector<std::reference_wrapper<Object>> objs;
    for(auto& obj: _state->dartToMagnum)
        if(obj.second->shapeNode())
            objs.emplace_back(*obj.second);

    return objs;
}

std::vector<std::reference_wrapper<Object>> World::bodyObjects() {
    std::vector<std::reference_wrapper<Object>> objs;
    for(auto& obj: _state->dartToMagnum)
        if(obj.second->bodyNode())
            objs.emplace_back(std::ref(*obj.second));

    return objs;
}

std::vector<std::reference_wrapper<Object>> World::updatedShapeObjects() {
    std::vector<std::reference_wrapper<Object>> objs;
    objs.reserve(_state->updatedShapeObjects.size());
    for(auto& obj: _state->updatedShapeObjects)
        objs.emplace_back(*obj);
    return objs;
}

World& World::clearUpdatedShapeObjects() {
    _state->updatedShapeObjects.clear();
    return *this;
}

Object& World::objectFromDartFrame(dart::dynamics::Frame* frame) {
    return *_state->dartToMagnum.at(frame);
}

dart::simulation::World& World::world() { return _state->dartWorld; }

void World::parseBodyNodeRecursive(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode& bn) {
    /* Parse the BodyNode -- we care only about visuals */
    auto& visualShapes = bn.getShapeNodesWith<dart::dynamics::VisualAspect>();

    /* Create an object of the BodyNode to keep track of transformations */
    SceneGraph::AbstractBasicObject3D<Float>* object = nullptr;
    auto it = _state->dartToMagnum.emplace(static_cast<dart::dynamics::Frame*>(&bn), nullptr);
    if(it.second) {
        object = objectCreator(parent);
        it.first->second = dartObjectCreator(*object, &bn);
    } else
        object = static_cast<SceneGraph::AbstractBasicObject3D<Float>*>(&it.first->second->object());

    it.first->second->update();

    for(auto& shape: visualShapes) {
        auto it = _state->dartToMagnum.emplace(static_cast<dart::dynamics::Frame*>(shape), nullptr);
        if(it.second) {
            /* create object for the ShapeNode to keep track of inner transformations */
            auto shapeObj = objectCreator(*object);
            it.first->second = dartShapeObjectCreator(*shapeObj, shape);
        }

        it.first->second->update(_state->importer.get());
        if(it.first->second->hasUpdatedMesh())
            _state->updatedShapeObjects.insert(it.first->second.get());
    }

    /* Parse the children recursively, pass the newly created object as parent */
    for(std::size_t i = 0; i < bn.getNumChildBodyNodes(); ++i)
        parseBodyNodeRecursive(*object, *bn.getChildBodyNode(i));
}

}}
