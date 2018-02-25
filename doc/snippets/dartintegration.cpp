#include <dart/simulation/World.hpp>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include "Magnum/DartIntegration/World.h"

using namespace Magnum;

dart::simulation::WorldPtr createWorldInDart();
void addDartSkeletonsToDartWorld();
template<class T> void deleteObjectsFromScene(T&);
template<class T> void updateMeshesAndMaterials(T&);

void foo();
void foo() {

{
/* [world-init] */
dart::simulation::WorldPtr dartWorld = createWorldInDart();
addDartSkeletonsToDartWorld();
SceneGraph::Scene<SceneGraph::MatrixTransformation3D> scene;
SceneGraph::Object<SceneGraph::MatrixTransformation3D> object{&scene};

/* object should never be a SceneGraph::Scene */
DartIntegration::World world{object, *dartWorld};
/* [world-init] */

UnsignedInt simulationSteps{};
/* [world-loop] */
/* Simulate with time step of 0.001 seconds */
world.world().setTimeStep(0.001);

for(UnsignedInt i = 0; i < simulationSteps; ++i) {
    world.step();

    /* Update graphics at ~60Hz (15*0.001 ~= 60Hz) */
    if(i % 15 == 0) {
        world.refresh();

        /* Get unused/deleted shapes */
        std::vector<std::unique_ptr<DartIntegration::Object>>& unusedObjects = world.unusedObjects();

        /* The user is expected to handle unused objects. One possible handling
           would be to remove them from the parent scene. */
        deleteObjectsFromScene(unusedObjects);

        /* Get updated shapes -- ones that either the materials or the meshes
          have changed */
        std::vector<std::reference_wrapper<DartIntegration::Object>> updatedObjects = world.updatedShapeObjects();

        updateMeshesAndMaterials(updatedObjects);

        /* Clear list of updated objects */
        world.clearUpdatedShapeObjects();
    }
}
/* [world-loop] */
}

}
