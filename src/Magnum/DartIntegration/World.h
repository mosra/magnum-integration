#ifndef Magnum_DartIntegration_World_h
#define Magnum_DartIntegration_World_h
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

/** @file
 * @brief Class @ref Magnum::DartIntegration::World
 */

#include <functional>
#include <memory>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/Utility/StlForwardVector.h>
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
captures any change that occurs in the DART world. After each @ref refresh()
the user has access to deleted shapes via @ref unusedObjects() and can get
updated shapes (material or mesh) via @ref updatedShapeObjects().

@section DartIntegration-World-usage Usage

Common usage is to create a `dart::simulation::World` and then instantiate
this class by passing a parent @ref SceneGraph::Object and the DART world to
its constructor:

@snippet DartIntegration.cpp World-init

Only the DART world can affect the transformation of the Magnum world and not
the other way around. Of course, one can change things directly in the DART
world and observe the changes in Magnum world. To update the world and get the
updated shapes you can do the following:

@snippet DartIntegration.cpp World-loop

@experimental
*/
class MAGNUM_DARTINTEGRATION_EXPORT World {
    public:
         /**
         * @brief Constructor
         * @param object    Parent object
         * @param world     DART world instance
         *
         * This constructor creates a private instance of
         * @ref Trade::AbstractImporter plugin manager for importing model
         * data. If you plan to use importer plugins elsewhere in your
         * application, it's advised to keep the plugin manager on the app side
         * and construct the world using @ref World(PluginManager::Manager<Trade::AbstractImporter>&, T&, dart::simulation::World&)
         * instead.
         */
        template<class T> explicit World(T& object, dart::simulation::World& world): World(nullptr, static_cast<SceneGraph::AbstractBasicObject3D<Float>&>(object), world) {
            initializeCreators<T>();
        }

         /**
         * @brief Construct with an external importer plugin manager
         * @param importerManager   Importer plugin manager
         * @param object            Parent object
         * @param world             DART world instance
         *
         * The @p importerManager is expected to be in scope for the whole
         * lifetime of the @ref World instance.
         */
        template<class T> explicit World(PluginManager::Manager<Trade::AbstractImporter>& importerManager, T& object, dart::simulation::World& world): World(&importerManager, static_cast<SceneGraph::AbstractBasicObject3D<Float>&>(object), world) {
            initializeCreators<T>();
        }

        ~World();

        /** @brief Refresh/regenerate meshes for all bodies in DART world */
        World& refresh();

        /**
         * @brief Do a DART world step
         *
         * The @p resetCommand parameter is passed to
         * @cpp dart::simulation::World::step() @ce.
         */
        World& step(bool resetCommand = true);

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
        std::vector<std::unique_ptr<Object>>& unusedObjects();

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
        dart::simulation::World& world();

    private:
        struct State;

        explicit World(PluginManager::Manager<Trade::AbstractImporter>* importerManager, SceneGraph::AbstractBasicObject3D<Float>& object, dart::simulation::World& world);

        SceneGraph::AbstractBasicObject3D<Float>*(*objectCreator)(SceneGraph::AbstractBasicObject3D<Float>& parent);
        std::unique_ptr<Object>(*dartObjectCreator)(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode* body);
        std::unique_ptr<Object>(*dartShapeObjectCreator)(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::ShapeNode* node);

        void MAGNUM_DARTINTEGRATION_LOCAL parseBodyNodeRecursive(SceneGraph::AbstractBasicObject3D<Float>& parent, dart::dynamics::BodyNode& bn);

        template<class T> void initializeCreators();

        Containers::Pointer<State> _state;
};

template<class T> void World::initializeCreators() {
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

}}

#endif
