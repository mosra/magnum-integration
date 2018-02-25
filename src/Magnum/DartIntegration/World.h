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

#include <Corrade/PluginManager/Manager.h>

#include <Magnum/SceneGraph/AbstractFeature.h>
#include <Magnum/SceneGraph/AbstractObject.h>
#include <Magnum/Trade/AbstractImporter.h>

#include "Magnum/DartIntegration/Object.h"

namespace dart {

namespace simulation {
    class World;
}

namespace dynamics {
    class BodyNode;
    class ShapeNode;
    class Frame;
}

}

namespace Magnum { namespace DartIntegration {

/**
@brief DART physics world

Parses a `dart::simulation::World` for easy usage in Magnum. It basically
parses all the `dart::dynamics::Skeleton` objects in the DART world and keeps
track of a list of @ref DartIntegration::Object instances. It automatically
captures any change that ocurrs in the DART world. After each @ref refresh()
the user has access to deleted shapes via @ref unusedObjects() and can get
updated shapes (material or mesh) via @ref updatedShapeObjects().

@section DartIntegration-World-usage Usage

Common usage is to create a `dart::simulation::World` and then instantiate
this class by passing a parent @ref SceneGraph::Object and the DART world to
its constructor:

@code{.cpp}
dart::simulation::WorldPtr dartWorld = createWorldInDart();
addDartSkeletonsToDartWorld();
SceneGraph::Scene<SceneGraph::MatrixTransformation3D> scene;
SceneGraph::Object<SceneGraph::MatrixTransformation3D> object{&scene};
// object should never be a @ref SceneGraph::Scene
auto world = std::make_shared<DartIntegration::World>(object, dartWorld);
@endcode

Only the DART world can affect the transformation of the Magnum world and not
the other way around. Of course, one can change things directly in the DART
world and observe the changes in Magnum world. To update the world and get the
updated shapes you can do the following:

@code{.cpp}
DartIntegration::World world{...};

// Simulate with time step of 0.001 seconds
world.world()->setTimeStep(0.001);

for(UnsignedInt i = 0; i < simulationSteps; ++i) {
    world.step();
    // update graphics at ~60Hz
    // 15*0.001 ~= 60Hz
    if(i%15 == 0) {
        world.refresh();

        // Get unused/deleted shapes
        std::vector<std::shared_ptr<Object>> unusedObjects = world.unusedObjects();

        // The user is expected to handle unused objects. One possible handling
        // would be to remove them from the parent scene
        deleteObjectsFromScene(unusedObjects);

        // Get updated shapes -- ones that either the materials or the meshes
        // have changed
        std::vector<std::shared_ptr<Object>> updatedObjects = world.updatedShapeObjects();

        updateMeshesAndMaterials(updatedObjects);

        // Clear list of updated objects
        world.clearUpdatedShapeObjects();
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
         * @param world     DART world instance
         */
        template<class T> explicit World(T& object, dart::simulation::World& world): World(static_cast<SceneGraph::AbstractBasicObject3D<Float>&>(object), world) {
            objectCreator = [](SceneGraph::AbstractBasicObject3D<Float>& parent) -> SceneGraph::AbstractBasicObject3D<Float>* {
                return new T{static_cast<T*>(&parent)};
            };
            dartObjectCreator = [](SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode* body) {
                return std::unique_ptr<Object>(new Object{static_cast<T&>(parent), body});
            };
            dartShapeObjectCreator = [](SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::ShapeNode* node) {
                return std::unique_ptr<Object>(new Object{static_cast<T&>(parent), node});
            };

            refresh();
        }

        ~World();

        /** @brief Refresh/regenerate meshes for all bodies in DART world */
        World& refresh();

        /** @brief Do a DART world step */
        World& step();

        /**
         * @brief Get unused objects
         *
         * Returns all objects that were not updated during the last
         * @ref refresh() call. This list gets cleared during the next
         * @ref refresh() call, which means also all @ref Object instances in
         * the list get deleted. In case you need to handle the @ref Object
         * instance deletion yourself, you're free to do anything with the
         * returned list --- move instances out, release them, erase the
         * elements etc.
         */
        std::vector<std::unique_ptr<Object>>& unusedObjects() { return _toRemove; }

        /** @brief All objects managed by the world */
        std::vector<std::reference_wrapper<Object>> objects();

        /** @brief All objects that have shapes */
        std::vector<std::reference_wrapper<Object>> shapeObjects();

        /** @brief All objects that do not have shapes */
        std::vector<std::reference_wrapper<Object>> bodyObjects();

        /**
         * @brief All objects that have updated shapes
         *
         * This list gets appended to after every @ref refresh() call, you can
         * use it to update your rendering structures. Be sure to call
         * @ref clearUpdatedShapeObjects() once you have your rendering
         * structures updated to avoid updating objects that didn't change
         * again.
         */
        std::vector<std::reference_wrapper<Object>> updatedShapeObjects();

        /**
         * @brief Clear list of updated shape objects
         * @return Reference to self (for method chaining)
         *
         * See @ref updatedShapeObjects() for more information.
         */
        World& clearUpdatedShapeObjects();

        /**
         * @brief Get object instance corresponding to a DART frame
         *
         * Returns @ref Object instance associated with a DART `Frame` /
         * `BodyNode` / `ShapeNode`. Expects that the DART frame is part of the
         * world.
        */
        Object& objectFromDartFrame(dart::dynamics::Frame* frame);

        /** @brief Underlying DART world object */
        dart::simulation::World& world() { return _dartWorld; }

    private:
        explicit World(SceneGraph::AbstractBasicObject3D<Float>& object, dart::simulation::World& world);

        SceneGraph::AbstractBasicObject3D<Float>*(*objectCreator)(SceneGraph::AbstractBasicObject3D<Float>& parent);
        std::unique_ptr<Object>(*dartObjectCreator)(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode* body);
        std::unique_ptr<Object>(*dartShapeObjectCreator)(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::ShapeNode* node);

        void MAGNUM_DARTINTEGRATION_LOCAL parseBodyNodeRecursive(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode& bn);

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
