#ifndef Magnum_DartIntegration_World_h
#define Magnum_DartIntegration_World_h
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
 * @brief Class @ref Magnum::DartIntegration::World
 */

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <dart/dynamics/BodyNode.hpp>
#include <dart/dynamics/ShapeNode.hpp>
#include <dart/simulation/World.hpp>

#include <Corrade/PluginManager/Manager.h>

#include <Magnum/SceneGraph/AbstractFeature.h>
#include <Magnum/SceneGraph/AbstractObject.h>
#include <Magnum/Trade/AbstractImporter.h>

#include "Magnum/DartIntegration/Object.h"

namespace Magnum { namespace DartIntegration {

/**
@brief DART Physics World

Parses a `dart::simulation::World` for easy usage in Magnum. It basically parses all
the `dart::dynamics::Skeleton` objects in the World and keeps track of a list of
@ref DartIntegration::Object instances. It automatically captures any change that
ocurres in the `dart::simulation::World`. After each graphics @ref DartIntegration::World::refresh
the user has access to the deleted shapes via @ref DartIntegration::World::unusedObjects and
can get the updated shapes (material or mesh) via @ref DartIntegration::World::updatedShapeObjects.

@section DartIntegration-World-usage Usage

Common usage is to create a `dart::simulation::WorldPtr` and then instantiate
a `DartIntegration::World` by passing a parent @ref SceneGraph::AbstractFeature and
the `dart::simulation::WorldPtr` to its constructor:

@code{.cpp}
dart::simulation::WorldPtr dartWorld = createWorldInDart();
addDartSkeletonsToDartWorld();
SceneGraph::Scene<SceneGraph::MatrixTransformation3D> scene;
SceneGraph::Object<SceneGraph::MatrixTransformation3D> object{&scene};
// object should never be a @ref SceneGraph::Scene
auto world = std::make_shared<DartIntegration::World>(object, dartWorld);
@endcode

Only the `dart::simulation::World` can affect the transformation of the Magnum
@ref DartIntegration::World and not the other way around. Of course, one can
change things directly in `dart::simulation::World` and observe the changes in
@ref DartIntegration::World. To update the world and get the updated shapes you
can do the following:

@code{.cpp}
std::shared_ptr<DartIntegration::World> world = createDartIntegrationWorld();
// Simulate with time step of 0.001 seconds
world->world()->setTimeStep(0.001);

for(UnsignedInt i = 0; i < simulationSteps; i++) {
    world->step();
    // update graphics at ~60Hz
    // 15*0.001 ~= 60Hz
    if(i%15 == 0) {
        world->refresh();

        // get unused/deleted shapes
        std::vector<std::shared_ptr<Object>> unusedObjects = world->unusedObjects();

        // the user is expected to handle unused objects
        // one possible handling would be to remove them
        // from the parent scene
        deleteObjectsFromScene(unusedObjects);

        // get updated shapes;
        // ones that either the materials or the meshes have changed
        std::vector<std::shared_ptr<Object>> updatedObjects = world->updatedShapeObjects();

        updateMeshesAndMaterials(updatedObjects);

        // clear list of updated objects
        world->clearUpdatedShapeObjects();
    }
}
@endcode

@experimental
*/
class MAGNUM_DARTINTEGRATION_EXPORT World {
    public:
         /**
         * @brief Constructor
         * @param object    Parent object
         * @param skeleton  DART World shared pointer to parse
         */
        template<class T> explicit World(T& object, std::shared_ptr<dart::simulation::World> world): World(static_cast<SceneGraph::AbstractBasicObject3D<Float>&>(object), world) {
            objectCreator = [](SceneGraph::AbstractBasicObject3D<Float>& parent) -> SceneGraph::AbstractBasicObject3D<Float>* {
                return new T{static_cast<T*>(&parent)};
            };
            dartObjectCreator = [](SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode* body) -> std::unique_ptr<Object> {
                return std::unique_ptr<Object>(new Object{static_cast<T&>(parent), body});
            };
            dartShapeObjectCreator = [](SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::ShapeNode* node) -> std::unique_ptr<Object> {
                return std::unique_ptr<Object>(new Object{static_cast<T&>(parent), node});
            };

            refresh();
        }

        ~World();

        /** @brief Refresh/regenerate meshes for all bodies in DART world */
        World& refresh();

        /** @brief Do a DART world step */
        World& step();

        /** @brief Cleared all objects that were not updated during the last refresh call */
        World& clearUnusedObjects();

        /** @brief Get unused objects; all objects that were not updated during the last refresh call
         * Note: this list will be cleared once a new refresh call is made
         */
        std::vector<std::reference_wrapper<Object>> unusedObjects();

        /** @brief Get all @ref Objects */
        std::vector<std::reference_wrapper<Object>> objects();

        /** @brief Get all @ref Objects that have shapes */
        std::vector<std::reference_wrapper<Object>> shapeObjects();

        /** @brief Get all @ref Objects that have updated meshes */
        std::vector<Object*> updatedShapeObjects();

        /** @brief Clear list of updated shape @ref Objects */
        World& clearUpdatedShapeObjects();

        /** @brief Get all @ref Objects that do not have shapes */
        std::vector<std::reference_wrapper<Object>> bodyObjects();

        /** @brief Helper function to get @ref Object from a DART Frame/BodyNode/ShapeNode
         * if the DART frame is not part of the World, the function throws an std::out_of_range
         * exception.
        */
        Object& objectFromDartFrame(dart::dynamics::Frame* frame) { return *_dartToMagnum.at(frame); }

        /** @brief Get the dart::simulation::World object
         * for making DART specific changes/updates
         */
        dart::simulation::World& world() { return _dartWorld; }

    private:
        /** @brief Non-templated constructor */
        explicit World(SceneGraph::AbstractBasicObject3D<Float>& object, std::shared_ptr<dart::simulation::World> world);

        /** @brief Function to create new @ref SceneGraph::AbstractBasicObject3D of correct type */
        SceneGraph::AbstractBasicObject3D<Float>*(*objectCreator)(SceneGraph::AbstractBasicObject3D<Float>& parent);

        /** @brief Function to create new @ref Object without shape with correct parent type */
        std::unique_ptr<Object>(*dartObjectCreator)(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode* body);

        /** @brief Function to create new @ref Object with shape with correct parent type */
        std::unique_ptr<Object>(*dartShapeObjectCreator)(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::ShapeNode* node);

        /** @brief Parse DART Skeleton and create/update shapes */
        void parseSkeleton(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::Skeleton& skel);

        /** @brief Recursively parse DART BodyNode and all of its children */
        void parseBodyNodeRecursive(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode& bn);

        SceneGraph::AbstractBasicObject3D<Float>& _object;
        PluginManager::Manager<Trade::AbstractImporter> _manager;
        std::unique_ptr<Trade::AbstractImporter> _importer;
        dart::simulation::World& _dartWorld;
        std::unordered_map<dart::dynamics::Frame*, std::unique_ptr<Object>> _dartToMagnum;
        std::vector<std::unique_ptr<Object>> _toRemove;
        std::unordered_set<Object*> _updatedShapeObjects;
};

}}

#endif
