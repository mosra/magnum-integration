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

#include <assimp/defs.h> /* in assimp 3.0, version.h is missing this include for ASSIMP_API */
#include <assimp/version.h>
#include <dart/dynamics/BallJoint.hpp>
#include <dart/dynamics/BodyNode.hpp>
#include <dart/dynamics/BoxShape.hpp>
#include <dart/dynamics/CylinderShape.hpp>
#include <dart/dynamics/EllipsoidShape.hpp>
#include <dart/dynamics/FreeJoint.hpp>
#include <dart/dynamics/MeshShape.hpp>
#include <dart/dynamics/RevoluteJoint.hpp>
#include <dart/dynamics/SoftBodyNode.hpp>
#include <dart/dynamics/Skeleton.hpp>
#include <dart/dynamics/WeldJoint.hpp>
#include <dart/simulation/World.hpp>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/OpenGLTester.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.hpp>
#include <Magnum/SceneGraph/SceneGraph.h>
#include <Magnum/Trade/PhongMaterialData.h>

#include "Magnum/DartIntegration/ConvertShapeNode.h"
#include "Magnum/DartIntegration/World.h"

#include "Magnum/DartIntegration/Test/common.h"
#include "Magnum/DartIntegration/Test/configure.h"

#define DART_URDF (MAGNUM_DART_URDF_FOUND > 0 && DART_MAJOR_VERSION >= 6)
#if DART_URDF
    #if DART_MAJOR_VERSION == 6
        #include <dart/utils/urdf/urdf.hpp>
    #else
        #include <dart/io/urdf/urdf.hpp>
    #endif
#endif

namespace Magnum { namespace DartIntegration { namespace Test { namespace {

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

struct DartIntegrationTest: GL::OpenGLTester {
    explicit DartIntegrationTest();

    void pendulum();
    void soft();
    void urdf();
    void multiMesh();
    void texture();

    void simpleSimulation();
    void softSimulation();
};

DartIntegrationTest::DartIntegrationTest() {
    addTests({&DartIntegrationTest::pendulum,
              &DartIntegrationTest::soft,
              #if DART_URDF
              &DartIntegrationTest::urdf,
              &DartIntegrationTest::multiMesh,
              &DartIntegrationTest::texture
              #endif
              });

    addBenchmarks({&DartIntegrationTest::simpleSimulation,
                   &DartIntegrationTest::softSimulation}, 3);
}

using namespace Math::Literals;

void DartIntegrationTest::pendulum() {
    /* Create an empty Skeleton with the name "pendulum" */
    std::string name = "pendulum";
    dart::dynamics::SkeletonPtr pendulum = dart::dynamics::Skeleton::create(name);

    /* Add each body to the last BodyNode in the pendulum */
    dart::dynamics::BodyNode* bn = makeRootBody(pendulum, "body1");
    bn = addBody(pendulum, bn, "body2");
    bn = addBody(pendulum, bn, "body3");
    bn = addBody(pendulum, bn, "body4");
    bn = addBody(pendulum, bn, "body5");

    /* Set the initial joint positions so that the pendulum
       starts to swing right away */
    pendulum->getDof(1)->setPosition(Double(Radd(120.0_deg)));
    pendulum->getDof(2)->setPosition(Double(Radd(20.0_deg)));
    pendulum->getDof(3)->setPosition(Double(Radd(-50.0_deg)));

    /* Create a world and add the pendulum to the world */
    dart::simulation::WorldPtr world(new dart::simulation::World);
    world->addSkeleton(pendulum);

    Scene3D scene;
    Object3D* obj = new Object3D{&scene};

    World dartWorld{*obj, *world};

    for(int i = 0; i < 10; ++i)
        dartWorld.step();
    dartWorld.refresh();

    auto objects = dartWorld.objects();
    Object& objTest = dartWorld.objectFromDartFrame(bn->getShapeNodesWith<dart::dynamics::VisualAspect>().back());

    Eigen::Isometry3d trans = bn->getShapeNodesWith<dart::dynamics::VisualAspect>().back()->getTransform();

    Eigen::AngleAxisd R = Eigen::AngleAxisd(trans.linear());
    Eigen::Vector3d axis = R.axis();
    Eigen::Vector3d T = trans.translation();

    /* Convert it to axis-angle representation */
    Vector3 t(T[0], T[1], T[2]);
    Vector3 u(axis(0), axis(1), axis(2));
    Rad theta(R.angle());

    Matrix4 transformation = Matrix4::translation(t)*Matrix4::rotation(theta, u);

    CORRADE_VERIFY(objects.size() == 15);
    CORRADE_COMPARE(objTest.object().absoluteTransformationMatrix(), transformation);
}

void DartIntegrationTest::soft() {
    /* Create a soft body node */
    auto soft = dart::dynamics::Skeleton::create("soft");
    addSoftBody<dart::dynamics::WeldJoint>(soft, "soft box");
    /* Create a world and add the body to the world */
    dart::simulation::WorldPtr world(new dart::simulation::World);
    world->addSkeleton(soft);

    Scene3D scene;
    Object3D* obj = new Object3D{&scene};

    World dartWorld{*obj, *world};

    for(int i = 0; i < 10; ++i)
        dartWorld.step();
    dartWorld.refresh();

    auto objects = dartWorld.objects();
    CORRADE_COMPARE(objects.size(), 2);
    CORRADE_COMPARE(dartWorld.shapeObjects().size(), 1);
}

#if DART_URDF
void DartIntegrationTest::urdf() {
    #if DART_MAJOR_VERSION == 6
    dart::utils::DartLoader loader;
    #else
    dart::io::DartLoader loader;
    #endif

    const UnsignedInt assimpVersion = aiGetVersionMajor()*100 + aiGetVersionMinor();

    const std::string filename = Utility::Directory::join(DARTINTEGRATION_TEST_DIR, "urdf/test.urdf");
    auto tmpSkel = loader.parseSkeleton(filename);
    CORRADE_VERIFY(tmpSkel);

    dart::simulation::WorldPtr world(new dart::simulation::World);
    world->addSkeleton(tmpSkel);

    Scene3D scene;
    Object3D* obj = new Object3D{&scene};

    World dartWorld{*obj, *world};
    for(Object& dartObj: dartWorld.shapeObjects()) {
        auto shape = dartObj.shapeNode()->getShape();
        CORRADE_COMPARE(shape->getType(), dart::dynamics::MeshShape::getStaticType());
        DrawData& mydata = dartObj.drawData();
        CORRADE_VERIFY(mydata.meshes.size());
        CORRADE_COMPARE(mydata.meshes.size(), mydata.materials.size());
        {
            CORRADE_EXPECT_FAIL_IF(assimpVersion < 302 || assimpVersion >= 400,
                "Assimp < 3.2 and Assimp >= 4.0 do not load the material correctly.");
            CORRADE_COMPARE(mydata.materials[0].diffuseColor(), (Vector3{0.6f, 0.6f, 0.6f}));
        }
        CORRADE_COMPARE(mydata.textures.size(), 0);
    }
}

void DartIntegrationTest::multiMesh() {
    #if DART_MAJOR_VERSION == 6
    dart::utils::DartLoader loader;
    #else
    dart::io::DartLoader loader;
    #endif

    const std::string filename = Utility::Directory::join(DARTINTEGRATION_TEST_DIR, "urdf/test_multi_mesh.urdf");
    auto tmpSkel = loader.parseSkeleton(filename);
    CORRADE_VERIFY(tmpSkel);

    dart::simulation::WorldPtr world(new dart::simulation::World);
    world->addSkeleton(tmpSkel);

    Scene3D scene;
    Object3D* obj = new Object3D{&scene};

    World dartWorld{*obj, *world};

    for(int i = 0; i < 1000; ++i) {
        dartWorld.step();
        /* refresh graphics at 60Hz */
        if(i%15 == 0) {
            dartWorld.refresh();
            dartWorld.clearUpdatedShapeObjects();
        }
    }

    CORRADE_COMPARE(dartWorld.shapeObjects().size(), 1);
    Object& dartObj = dartWorld.shapeObjects()[0];

    auto shape = dartObj.shapeNode()->getShape();
    CORRADE_COMPARE(shape->getType(), dart::dynamics::MeshShape::getStaticType());
    DrawData& mydata = dartObj.drawData();
    CORRADE_COMPARE(mydata.meshes.size(), 2);
    CORRADE_COMPARE(mydata.meshes.size(), mydata.materials.size());
    CORRADE_COMPARE(mydata.textures.size(), 0);
}

void DartIntegrationTest::texture() {
    /** @todo Possibly works with earlier versions (definitely not 3.0) */
    const UnsignedInt assimpVersion = aiGetVersionMajor()*100 + aiGetVersionMinor();
    if(assimpVersion < 302)
        CORRADE_SKIP("Current version of Assimp would not work on this test.");

    #if DART_MAJOR_VERSION == 6
    dart::utils::DartLoader loader;
    #else
    dart::io::DartLoader loader;
    #endif

    const std::string filename = Utility::Directory::join(DARTINTEGRATION_TEST_DIR, "urdf/test_texture.urdf");
    auto tmpSkel = loader.parseSkeleton(filename);
    CORRADE_VERIFY(tmpSkel);

    dart::simulation::WorldPtr world(new dart::simulation::World);
    world->addSkeleton(tmpSkel);

    Scene3D scene;
    Object3D* obj = new Object3D{&scene};

    World dartWorld{*obj, *world};
    for(int i = 0; i < 1000; ++i) {
        dartWorld.step();
        /* refresh graphics at 60Hz */
        if(i%15 == 0) {
            dartWorld.refresh();
            dartWorld.clearUpdatedShapeObjects();
        }
    }

    for(Object& dartObj: dartWorld.shapeObjects()) {
        auto shape = dartObj.shapeNode()->getShape();
        CORRADE_COMPARE(shape->getType(), dart::dynamics::MeshShape::getStaticType());
        DrawData& mydata = dartObj.drawData();
        CORRADE_VERIFY(mydata.meshes.size());
        CORRADE_COMPARE(mydata.meshes.size(), mydata.materials.size());
        CORRADE_COMPARE(mydata.textures.size(), 1);
        CORRADE_VERIFY(mydata.textures[0]);
    }
}
#endif

void DartIntegrationTest::simpleSimulation() {
    /* Create an empty Skeleton with the name "pendulum" */
    std::string name = "pendulum";
    dart::dynamics::SkeletonPtr pendulum = dart::dynamics::Skeleton::create(name);

    /* Add each body to the last BodyNode in the pendulum */
    dart::dynamics::BodyNode* bn = makeRootBody(pendulum, "body1");
    bn = addBody(pendulum, bn, "body2");
    bn = addBody(pendulum, bn, "body3");
    bn = addBody(pendulum, bn, "body4");
    bn = addBody(pendulum, bn, "body5");

    /* Set the initial joint positions so that the pendulum
    starts to swing right away */
    pendulum->getDof(1)->setPosition(Double(Radd(120.0_deg)));
    pendulum->getDof(2)->setPosition(Double(Radd(20.0_deg)));
    pendulum->getDof(3)->setPosition(Double(Radd(-50.0_deg)));

    /* Create a world and add the pendulum to the world */
    dart::simulation::WorldPtr world(new dart::simulation::World);
    world->addSkeleton(pendulum);

    Scene3D scene;
    Object3D* obj = new Object3D{&scene};

    World dartWorld{*obj, *world};
    CORRADE_BENCHMARK(2) {
        for(int i = 0; i < 250; ++i) {
            dartWorld.step();
            /* refresh graphics at 60Hz */
            if(i%15 == 0) {
                dartWorld.refresh();
                dartWorld.clearUpdatedShapeObjects();
            }
        }
    }
}

void DartIntegrationTest::softSimulation() {
    CORRADE_BENCHMARK(2) {
        /* Create a soft body node */
        auto soft = dart::dynamics::Skeleton::create("soft");
        addSoftBody<dart::dynamics::FreeJoint>(soft, "soft box");

        /* Create a second soft body node in different location */
        Eigen::Vector6d positions(Eigen::Vector6d::Zero());
        positions(5) = 0.7;
        positions(4) = 1.;
        auto soft2 = dart::dynamics::Skeleton::create("soft2");
        addSoftBody<dart::dynamics::FreeJoint>(soft2, "soft box 2");
        soft2->getJoint(0)->setPositions(positions);
        /* Create a world and add the body to the world */
        dart::simulation::WorldPtr world(new dart::simulation::World);
        world->addSkeleton(soft);
        world->addSkeleton(soft2);

        Scene3D scene;
        Object3D* obj = new Object3D{&scene};

        World dartWorld{*obj, *world};
        for(int i = 0; i < 250; ++i) {
            dartWorld.step();
            /* refresh graphics at 60Hz */
            if(i%15 == 0) {
                dartWorld.refresh();
                dartWorld.clearUpdatedShapeObjects();
            }
        }
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::DartIntegration::Test::DartIntegrationTest)
