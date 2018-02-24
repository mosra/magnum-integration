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

#include <assimp/defs.h> /* in assimp 3.0, version.h is missing this include for ASSIMP_API */
#include <assimp/version.h>
#include <dart/dynamics/BallJoint.hpp>
#include <dart/dynamics/BodyNode.hpp>
#include <dart/dynamics/BoxShape.hpp>
#include <dart/dynamics/CapsuleShape.hpp>
#include <dart/dynamics/ConeShape.hpp>
#include <dart/dynamics/CylinderShape.hpp>
#include <dart/dynamics/EllipsoidShape.hpp>
#include <dart/dynamics/FreeJoint.hpp>
#include <dart/dynamics/LineSegmentShape.hpp>
#include <dart/dynamics/MeshShape.hpp>
#include <dart/dynamics/MultiSphereConvexHullShape.hpp>
#include <dart/dynamics/PlaneShape.hpp>
#include <dart/dynamics/RevoluteJoint.hpp>
#include <dart/dynamics/ShapeNode.hpp>
#include <dart/dynamics/SoftBodyNode.hpp>
#include <dart/dynamics/SoftMeshShape.hpp>
#include <dart/dynamics/SphereShape.hpp>
#include <dart/dynamics/Skeleton.hpp>
#include <dart/dynamics/WeldJoint.hpp>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Directory.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/PhongMaterialData.h>

#include "Magnum/DartIntegration/ConvertShapeNode.h"
#include "Magnum/DartIntegration/Test/configure.h"

#define DART_URDF (MAGNUM_DART_URDF_FOUND > 0 && DART_MAJOR_VERSION >= 6)
#if DART_URDF
    #if DART_MAJOR_VERSION == 6
        #include <dart/utils/urdf/urdf.hpp>
    #else
        #include <dart/io/urdf/urdf.hpp>
    #endif
#endif

namespace Magnum { namespace DartIntegration { namespace Test {

namespace {

using namespace Math::Literals;

constexpr Double DefaultHeight = 1.0; // m
constexpr Double DefaultWidth = 0.2; // m
constexpr Double DefaultDepth = 0.2; // m

constexpr Double DefaultRestPosition = 0.0;

constexpr Double DefaultStiffness = 0.0;

constexpr Double DefaultDamping = 5.0;

void setGeometry(const dart::dynamics::BodyNodePtr& bn) {
    /* Create a BoxShape to be used for both visualization and collision checking */
    std::shared_ptr<dart::dynamics::BoxShape> box(new dart::dynamics::BoxShape(
        Eigen::Vector3d(DefaultWidth, DefaultDepth, DefaultHeight)));

    /* Create a shape node for visualization and collision checking */
    auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(box);
    shapeNode->getVisualAspect()->setColor(dart::Color::Blue());

    /* Set the location of the shape node */
    Eigen::Isometry3d box_tf(Eigen::Isometry3d::Identity());
    Eigen::Vector3d center = Eigen::Vector3d(0, 0, DefaultHeight/2.0);
    box_tf.translation() = center;
    shapeNode->setRelativeTransform(box_tf);

    /* Move the center of mass to the center of the object */
    bn->setLocalCOM(center);
}

dart::dynamics::BodyNode* makeRootBody(const dart::dynamics::SkeletonPtr& pendulum, std::string name) {
    dart::dynamics::BallJoint::Properties properties;
    properties.mName = name + "_joint";
    properties.mRestPositions = Eigen::Vector3d::Constant(DefaultRestPosition);
    properties.mSpringStiffnesses = Eigen::Vector3d::Constant(DefaultStiffness);
    properties.mDampingCoefficients = Eigen::Vector3d::Constant(DefaultDamping);

    dart::dynamics::BodyNodePtr bn = pendulum->createJointAndBodyNodePair<dart::dynamics::BallJoint>(
        nullptr, properties, dart::dynamics::BodyNode::AspectProperties(name)).second;

    /* Make a shape for the Joint */
    auto ball = std::make_shared<dart::dynamics::EllipsoidShape>(Math::sqrt(2.0)*
        Eigen::Vector3d(DefaultWidth, DefaultWidth, DefaultWidth));
    auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect>(ball);
    auto va = shapeNode->getVisualAspect();
    CORRADE_INTERNAL_ASSERT(va);
    va->setColor(dart::Color::Blue());

    /* Set the geometry of the Body */
    setGeometry(bn);
    return bn;
}

dart::dynamics::BodyNode* addBody(const dart::dynamics::SkeletonPtr& pendulum, dart::dynamics::BodyNode* parent, std::string name) {
    /* Set up the properties for the Joint */
    dart::dynamics::RevoluteJoint::Properties properties;
    properties.mName = name + "_joint";
    properties.mAxis = Eigen::Vector3d::UnitY();
    properties.mT_ParentBodyToJoint.translation() = Eigen::Vector3d(0, 0, DefaultHeight);
    properties.mRestPositions[0] = DefaultRestPosition;
    properties.mSpringStiffnesses[0] = DefaultStiffness;
    properties.mDampingCoefficients[0] = DefaultDamping;

    /* Create a new BodyNode, attached to its parent by a RevoluteJoint */
    dart::dynamics::BodyNodePtr bn = pendulum->createJointAndBodyNodePair<dart::dynamics::RevoluteJoint>(
        parent, properties, dart::dynamics::BodyNode::AspectProperties(name)).second;

    /* Make a shape for the Joint */
    auto cyl = std::make_shared<dart::dynamics::CylinderShape>(DefaultWidth/2.0, DefaultDepth);

    /* Line up the cylinder with the Joint axis */
    Eigen::Isometry3d tf(Eigen::Isometry3d::Identity());
    tf.linear() = dart::math::eulerXYZToMatrix(
        Eigen::Vector3d(Double(Radd(90.0_deg)), 0, 0));

    auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect>(cyl);
    shapeNode->getVisualAspect()->setColor(dart::Color::Blue());
    shapeNode->setRelativeTransform(tf);

    /* Set the geometry of the Body */
    setGeometry(bn);
    return bn;
}

/* Add a soft body with the specified Joint type to a chain */
template<class JointType>
dart::dynamics::BodyNode* addSoftBody(const dart::dynamics::SkeletonPtr& chain, const std::string& name, dart::dynamics::BodyNode* parent = nullptr)
{
    constexpr double default_shape_density = 1000; // kg/m^3
    constexpr double default_shape_height  = 0.1;  // m
    constexpr double default_shape_width   = 0.03; // m
    constexpr double default_skin_thickness = 1e-3; // m

    constexpr double default_vertex_stiffness = 1000.0;
    constexpr double default_edge_stiffness = 1.0;
    constexpr double default_soft_damping = 5.0;

    /* Set the Joint properties */
    typename JointType::Properties joint_properties;
    joint_properties.mName = name+"_joint";
    if(parent)
    {
        /* If the body has a parent, we should position the joint to be in the
         * middle of the centers of the two bodies */
        Eigen::Isometry3d tf(Eigen::Isometry3d::Identity());
        tf.translation() = Eigen::Vector3d(0, 0, default_shape_height / 2.0);
        joint_properties.mT_ParentBodyToJoint = tf;
        joint_properties.mT_ChildBodyToJoint = tf.inverse();
    }

    /* Set the properties of the soft body */
    dart::dynamics::SoftBodyNode::UniqueProperties soft_properties;
    /* Make a wide and short box */
    double width = default_shape_height, height = 2*default_shape_width;
    Eigen::Vector3d dims(width, width, height);

    double mass = 2*dims[0]*dims[1] + 2*dims[0]*dims[2] + 2*dims[1]*dims[2];
    mass *= default_shape_density * default_skin_thickness;
    soft_properties = dart::dynamics::SoftBodyNodeHelper::makeBoxProperties(dims, Eigen::Isometry3d::Identity(), Eigen::Vector3i(4,4,4), mass);

    soft_properties.mKv = default_vertex_stiffness;
    soft_properties.mKe = default_edge_stiffness;
    soft_properties.mDampCoeff = default_soft_damping;

    /* Create the Joint and Body pair */
    dart::dynamics::SoftBodyNode::Properties body_properties(dart::dynamics::BodyNode::AspectProperties(name), soft_properties);
    dart::dynamics::SoftBodyNode* bn = chain->createJointAndBodyNodePair<JointType, dart::dynamics::SoftBodyNode>(parent, joint_properties, body_properties).second;

    /* Zero out the inertia for the underlying BodyNode */
    dart::dynamics::Inertia inertia;
    inertia.setMoment(1e-8*Eigen::Matrix3d::Identity());
    inertia.setMass(1e-8);
    bn->setInertia(inertia);

    return bn;
}

}

struct ConvertShapeNodeTest: TestSuite::Tester {
    explicit ConvertShapeNodeTest();

    void basicShapes();
    void assimpImporter();
    void unsupportedShapes();
    void pendulum();
    void soft();
    void urdf();
    void multiMesh();
    void texture();
};

ConvertShapeNodeTest::ConvertShapeNodeTest() {
    addTests({&ConvertShapeNodeTest::basicShapes,
              &ConvertShapeNodeTest::assimpImporter,
              &ConvertShapeNodeTest::unsupportedShapes,
              &ConvertShapeNodeTest::pendulum,
              &ConvertShapeNodeTest::soft,
              #if DART_URDF
              &ConvertShapeNodeTest::urdf,
              &ConvertShapeNodeTest::multiMesh,
              &ConvertShapeNodeTest::texture
              #endif
              });
}

void ConvertShapeNodeTest::basicShapes() {
    {
        /* BoxShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("BoxShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("BoxShapeBody")).second;

        std::shared_ptr<dart::dynamics::BoxShape> box(new dart::dynamics::BoxShape(Eigen::Vector3d(1., 1., 1.)));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(box);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(shapeDataAll);
    }
    {
        /* CapsuleShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("CapsuleShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("CapsuleShapeBody")).second;

        std::shared_ptr<dart::dynamics::CapsuleShape> capsule(new dart::dynamics::CapsuleShape(1., 1.));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(capsule);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(shapeDataAll);
    }
    {
        /* CylinderShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("CylinderShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("CylinderShapeBody")).second;

        std::shared_ptr<dart::dynamics::CylinderShape> cylinder(new dart::dynamics::CylinderShape(1., 1.));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(cylinder);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(shapeDataAll);
    }
    {
        /* EllipsoidShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("EllipsoidShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("EllipsoidShapeBody")).second;

        std::shared_ptr<dart::dynamics::EllipsoidShape> ellipsoid(new dart::dynamics::EllipsoidShape(Eigen::Vector3d(1., 1., 1.)));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(ellipsoid);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(shapeDataAll);
    }
    {
        /* SphereShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("SphereShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("SphereShapeBody")).second;

        std::shared_ptr<dart::dynamics::SphereShape> sphere(new dart::dynamics::SphereShape(1.));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(sphere);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(shapeDataAll);
    }
}

void ConvertShapeNodeTest::assimpImporter() {
    {
        /* MeshShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("MeshShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("MeshShapeBody")).second;

        /* pass nullptr as the mesh because we are not going to use it */
        std::shared_ptr<dart::dynamics::MeshShape> mesh(new dart::dynamics::MeshShape(Eigen::Vector3d(1., 1., 1.), nullptr));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(mesh);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All, nullptr);
        CORRADE_VERIFY(!shapeDataAll);
    }
    {
        /* load AssimpImporter */
        PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
        std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AssimpImporter");
        CORRADE_VERIFY(importer);
        /* MeshShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("MeshShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("MeshShapeBody")).second;

        aiScene* noMeshScene = new aiScene;
        std::shared_ptr<dart::dynamics::MeshShape> mesh(new dart::dynamics::MeshShape(Eigen::Vector3d(1., 1., 1.), noMeshScene));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(mesh);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All, importer.get());
        CORRADE_VERIFY(!shapeDataAll);
    }
    #if DART_URDF
    #if DART_MAJOR_VERSION == 6
    dart::utils::DartLoader loader;
    #else
    dart::io::DartLoader loader;
    #endif
    {
        PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
        std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AssimpImporter");
        CORRADE_VERIFY(importer);

        const std::string filename = Utility::Directory::join(DARTINTEGRATION_TEST_DIR, "urdf/test.urdf");
        auto tmp_skel = loader.parseSkeleton(filename);

        auto shapeNode = tmp_skel->getBodyNode(0)->getShapeNodesWith<dart::dynamics::VisualAspect>()[0];
        const aiScene* assimpScene = std::static_pointer_cast<dart::dynamics::MeshShape>(shapeNode->getShape())->getMesh();
        CORRADE_VERIFY(assimpScene);

        /* change mesh primitive type in order to produce failure */
        aiMesh* mesh = assimpScene->mMeshes[0];
        mesh->mPrimitiveTypes = aiPrimitiveType::aiPrimitiveType_POLYGON;

        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All, importer.get());
        CORRADE_VERIFY(!shapeDataAll);
    }
    {
        PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
        std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AssimpImporter");
        CORRADE_VERIFY(importer);

        const std::string filename = Utility::Directory::join(DARTINTEGRATION_TEST_DIR, "urdf/test.urdf");
        auto tmp_skel = loader.parseSkeleton(filename);

        auto shapeNode = tmp_skel->getBodyNode(0)->getShapeNodesWith<dart::dynamics::VisualAspect>()[0];
        auto meshShape = std::static_pointer_cast<dart::dynamics::MeshShape>(shapeNode->getShape());

        /* check dart::dynamics::MeshShape::SHAPE_COLOR */
        meshShape->setColorMode(dart::dynamics::MeshShape::SHAPE_COLOR);

        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All, importer.get());
        CORRADE_VERIFY(shapeDataAll);

        /* check dart::dynamics::MeshShape::COLOR_INDEX when no colors available */
        meshShape->setColorMode(dart::dynamics::MeshShape::COLOR_INDEX);

        shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All, importer.get());
        CORRADE_VERIFY(shapeDataAll);

        /* check dart::dynamics::MeshShape::COLOR_INDEX with colors */
        const aiScene* assimpScene = meshShape->getMesh();
        aiMesh* mesh = assimpScene->mMeshes[0];
        /* add one color to aiMesh */
        mesh->mColors[0] = new aiColor4D[mesh->mNumVertices];

        meshShape->setColorMode(dart::dynamics::MeshShape::COLOR_INDEX);
        /* set color index higher than the one available */
        meshShape->setColorIndex(1);

        shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All, importer.get());
        CORRADE_VERIFY(shapeDataAll);
    }
    #endif
}

void ConvertShapeNodeTest::unsupportedShapes() {
    {
        /* ConeShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("ConeShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("ConeShapeBody")).second;

        std::shared_ptr<dart::dynamics::ConeShape> cone(new dart::dynamics::ConeShape(1., 1.));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(cone);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(!shapeDataAll);
    }
    {
        /* LineSegmentShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("LineSegmentShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("LineSegmentShapeBody")).second;

        std::shared_ptr<dart::dynamics::LineSegmentShape> line(new dart::dynamics::LineSegmentShape());
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(line);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(!shapeDataAll);
    }
    {
        /* MultiSphereConvexHullShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("MultiSphereConvexHullShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("MultiSphereConvexHullShapeBody")).second;

        std::shared_ptr<dart::dynamics::MultiSphereConvexHullShape> multiSphere(new dart::dynamics::MultiSphereConvexHullShape(std::vector<std::pair<double, Eigen::Vector3d>>()));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(multiSphere);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(!shapeDataAll);
    }
    {
        /* PlaneShape */
        dart::dynamics::SkeletonPtr tmpSkel = dart::dynamics::Skeleton::create("PlaneShape");

        dart::dynamics::BodyNodePtr bn = tmpSkel->createJointAndBodyNodePair<dart::dynamics::WeldJoint>(
            nullptr, dart::dynamics::WeldJoint::Properties(), dart::dynamics::BodyNode::AspectProperties("PlaneShapeBody")).second;

        std::shared_ptr<dart::dynamics::PlaneShape> plane(new dart::dynamics::PlaneShape(Eigen::Vector3d(1., 1., 1.), 1.));
        auto shapeNode = bn->createShapeNodeWith<dart::dynamics::VisualAspect, dart::dynamics::CollisionAspect, dart::dynamics::DynamicsAspect>(plane);
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(!shapeDataAll);
    }
}

void ConvertShapeNodeTest::pendulum() {
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

    for(auto& shapeNode: bn->getShapeNodesWith<dart::dynamics::VisualAspect>()) {
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(shapeDataAll);
        auto shapeDataMaterial = convertShapeNode(*shapeNode, ConvertShapeType::Material);
        CORRADE_VERIFY(shapeDataMaterial);
        auto shapeDataPrimitive = convertShapeNode(*shapeNode, ConvertShapeType::Primitive);
        CORRADE_VERIFY(shapeDataPrimitive);
        auto shapeDataMesh = convertShapeNode(*shapeNode, ConvertShapeType::Mesh);
        CORRADE_VERIFY(shapeDataMesh);

        CORRADE_COMPARE(shapeDataAll->meshes.size(), 1);

        CORRADE_COMPARE(shapeDataMaterial->meshes.size(), 0);

        CORRADE_COMPARE(shapeDataPrimitive->meshes.size(), 0);
        CORRADE_COMPARE(shapeDataPrimitive->materials.size(), 0);

        CORRADE_COMPARE(shapeDataMesh->meshes.size(), 1);
        CORRADE_COMPARE(shapeDataMesh->materials.size(), 0);

        CORRADE_COMPARE(shapeDataAll->materials.size(), 1);
        CORRADE_COMPARE(shapeDataAll->materials.size(), shapeDataMaterial->materials.size());
        CORRADE_COMPARE(shapeDataAll->materials[0].diffuseColor(), shapeDataMaterial->materials[0].diffuseColor());

        CORRADE_COMPARE(shapeDataAll->scaling, shapeDataPrimitive->scaling);

        CORRADE_COMPARE(shapeDataAll->textures.size(), 0);
        CORRADE_COMPARE(shapeDataAll->textures.size(), shapeDataMaterial->textures.size());
    }
}

void ConvertShapeNodeTest::soft() {
    /* Create a soft body node */
    auto soft = dart::dynamics::Skeleton::create("soft");
    auto bn = addSoftBody<dart::dynamics::WeldJoint>(soft, "soft box");

    for(auto& shapeNode: bn->getShapeNodesWith<dart::dynamics::VisualAspect>()) {
        auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All);
        CORRADE_VERIFY(shapeDataAll);
        auto shapeDataMaterial = convertShapeNode(*shapeNode, ConvertShapeType::Material);
        CORRADE_VERIFY(shapeDataMaterial);
        auto shapeDataPrimitive = convertShapeNode(*shapeNode, ConvertShapeType::Primitive);
        CORRADE_VERIFY(shapeDataPrimitive);
        auto shapeDataMesh = convertShapeNode(*shapeNode, ConvertShapeType::Mesh);
        CORRADE_VERIFY(shapeDataMesh);

        CORRADE_COMPARE(shapeDataAll->meshes.size(), 1);

        CORRADE_COMPARE(shapeDataMaterial->meshes.size(), 0);

        CORRADE_COMPARE(shapeDataPrimitive->meshes.size(), 0);
        CORRADE_COMPARE(shapeDataPrimitive->materials.size(), 0);

        CORRADE_COMPARE(shapeDataMesh->meshes.size(), 1);
        CORRADE_COMPARE(shapeDataMesh->materials.size(), 0);

        CORRADE_COMPARE(shapeDataAll->materials.size(), 1);
        CORRADE_COMPARE(shapeDataAll->materials.size(), shapeDataMaterial->materials.size());
        CORRADE_COMPARE(shapeDataAll->materials[0].diffuseColor(), shapeDataMaterial->materials[0].diffuseColor());

        CORRADE_COMPARE(shapeDataAll->scaling, shapeDataPrimitive->scaling);

        CORRADE_COMPARE(shapeDataAll->textures.size(), 0);
        CORRADE_COMPARE(shapeDataAll->textures.size(), shapeDataMaterial->textures.size());
    }
}

#if DART_URDF
void ConvertShapeNodeTest::urdf() {
    #if DART_MAJOR_VERSION == 6
    dart::utils::DartLoader loader;
    #else
    dart::io::DartLoader loader;
    #endif

    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
    std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AssimpImporter");
    CORRADE_VERIFY(importer);

    const UnsignedInt assimpVersion = aiGetVersionMajor()*100 + aiGetVersionMinor();

    const std::string filename = Utility::Directory::join(DARTINTEGRATION_TEST_DIR, "urdf/test.urdf");
    auto tmp_skel = loader.parseSkeleton(filename);
    CORRADE_VERIFY(tmp_skel);

    for(auto& bn: tmp_skel->getBodyNodes()) {
        for(auto& shapeNode: bn->getShapeNodesWith<dart::dynamics::VisualAspect>()) {
            auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All, importer.get());
            CORRADE_VERIFY(shapeDataAll);
            auto shapeDataMaterial = convertShapeNode(*shapeNode, ConvertShapeType::Material, importer.get());
            CORRADE_VERIFY(shapeDataMaterial);
            auto shapeDataPrimitive = convertShapeNode(*shapeNode, ConvertShapeType::Primitive, importer.get());
            CORRADE_VERIFY(shapeDataPrimitive);
            auto shapeDataMesh = convertShapeNode(*shapeNode, ConvertShapeType::Mesh, importer.get());
            CORRADE_VERIFY(shapeDataMesh);

            CORRADE_COMPARE(shapeDataAll->meshes.size(), 1);

            CORRADE_COMPARE(shapeDataMaterial->meshes.size(), 0);

            CORRADE_COMPARE(shapeDataPrimitive->meshes.size(), 0);
            CORRADE_COMPARE(shapeDataPrimitive->materials.size(), 0);

            CORRADE_COMPARE(shapeDataMesh->meshes.size(), 1);
            CORRADE_COMPARE(shapeDataMesh->materials.size(), 0);

            CORRADE_COMPARE(shapeDataAll->materials.size(), 1);
            CORRADE_COMPARE(shapeDataAll->materials.size(), shapeDataMaterial->materials.size());
            CORRADE_COMPARE(shapeDataAll->materials[0].diffuseColor(), shapeDataMaterial->materials[0].diffuseColor());
            {
                CORRADE_EXPECT_FAIL_IF(assimpVersion < 302,
                    "Old versions of Assimp do not load the material correctly");
                CORRADE_COMPARE(shapeDataAll->materials[0].diffuseColor(), (Vector3{0.6f, 0.6f, 0.6f}));
                CORRADE_COMPARE(shapeDataMaterial->materials[0].diffuseColor(), (Vector3{0.6f, 0.6f, 0.6f}));
            }

            CORRADE_COMPARE(shapeDataAll->scaling, shapeDataPrimitive->scaling);

            CORRADE_COMPARE(shapeDataAll->textures.size(), 0);
            CORRADE_COMPARE(shapeDataAll->textures.size(), shapeDataMaterial->textures.size());
        }
    }
}

void ConvertShapeNodeTest::multiMesh() {
    /** @todo Possibly works with earlier versions (definitely not 3.0) */
    const UnsignedInt assimpVersion = aiGetVersionMajor()*100 + aiGetVersionMinor();
    if(assimpVersion < 302)
        CORRADE_SKIP("Current version of Assimp would not work on this test.");
    #if DART_MAJOR_VERSION == 6
    dart::utils::DartLoader loader;
    #else
    dart::io::DartLoader loader;
    #endif

    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
    std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AssimpImporter");
    CORRADE_VERIFY(importer);

    const std::string filename = Utility::Directory::join(DARTINTEGRATION_TEST_DIR, "urdf/test_multi_mesh.urdf");
    auto tmp_skel = loader.parseSkeleton(filename);
    CORRADE_VERIFY(tmp_skel);

    for(auto& bn: tmp_skel->getBodyNodes()) {
        for(auto& shapeNode: bn->getShapeNodesWith<dart::dynamics::VisualAspect>()) {
            auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All, importer.get());
            CORRADE_VERIFY(shapeDataAll);
            auto shapeDataMaterial = convertShapeNode(*shapeNode, ConvertShapeType::Material, importer.get());
            CORRADE_VERIFY(shapeDataMaterial);
            auto shapeDataPrimitive = convertShapeNode(*shapeNode, ConvertShapeType::Primitive, importer.get());
            CORRADE_VERIFY(shapeDataPrimitive);
            auto shapeDataMesh = convertShapeNode(*shapeNode, ConvertShapeType::Mesh, importer.get());
            CORRADE_VERIFY(shapeDataMesh);

            CORRADE_COMPARE(shapeDataAll->meshes.size(), 2);

            CORRADE_COMPARE(shapeDataMaterial->meshes.size(), 0);

            CORRADE_COMPARE(shapeDataPrimitive->meshes.size(), 0);
            CORRADE_COMPARE(shapeDataPrimitive->materials.size(), 0);

            CORRADE_COMPARE(shapeDataMesh->meshes.size(), 2);
            CORRADE_COMPARE(shapeDataMesh->materials.size(), 0);

            CORRADE_COMPARE(shapeDataAll->materials.size(), 2);
            CORRADE_COMPARE(shapeDataAll->materials.size(), shapeDataMaterial->materials.size());

            CORRADE_COMPARE(shapeDataAll->scaling, shapeDataPrimitive->scaling);

            CORRADE_COMPARE(shapeDataAll->textures.size(), 0);
            CORRADE_COMPARE(shapeDataAll->textures.size(), shapeDataMaterial->textures.size());
        }
    }
}

void ConvertShapeNodeTest::texture() {
    /** @todo Possibly works with earlier versions (definitely not 3.0) */
    const UnsignedInt assimpVersion = aiGetVersionMajor()*100 + aiGetVersionMinor();
    if(assimpVersion < 302)
        CORRADE_SKIP("Current version of Assimp would not work on this test.");

    #if DART_MAJOR_VERSION == 6
    dart::utils::DartLoader loader;
    #else
    dart::io::DartLoader loader;
    #endif

    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
    std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AssimpImporter");
    CORRADE_VERIFY(importer);

    const std::string filename = Utility::Directory::join(DARTINTEGRATION_TEST_DIR, "urdf/test_texture.urdf");
    auto tmp_skel = loader.parseSkeleton(filename);
    CORRADE_VERIFY(tmp_skel);

    for(auto& bn: tmp_skel->getBodyNodes()) {
        for(auto& shapeNode: bn->getShapeNodesWith<dart::dynamics::VisualAspect>()) {
            auto shapeDataAll = convertShapeNode(*shapeNode, ConvertShapeType::All, importer.get());
            CORRADE_VERIFY(shapeDataAll);
            auto shapeDataMaterial = convertShapeNode(*shapeNode, ConvertShapeType::Material, importer.get());
            CORRADE_VERIFY(shapeDataMaterial);
            auto shapeDataPrimitive = convertShapeNode(*shapeNode, ConvertShapeType::Primitive, importer.get());
            CORRADE_VERIFY(shapeDataPrimitive);
            auto shapeDataMesh = convertShapeNode(*shapeNode, ConvertShapeType::Mesh, importer.get());
            CORRADE_VERIFY(shapeDataMesh);

            CORRADE_COMPARE(shapeDataAll->meshes.size(), 1);

            CORRADE_COMPARE(shapeDataMaterial->meshes.size(), 0);

            CORRADE_COMPARE(shapeDataPrimitive->meshes.size(), 0);
            CORRADE_COMPARE(shapeDataPrimitive->materials.size(), 0);

            CORRADE_COMPARE(shapeDataMesh->meshes.size(), 1);
            CORRADE_COMPARE(shapeDataMesh->materials.size(), 0);

            CORRADE_COMPARE(shapeDataAll->materials.size(), 1);
            CORRADE_COMPARE(shapeDataAll->materials.size(), shapeDataMaterial->materials.size());

            CORRADE_COMPARE(shapeDataAll->scaling, shapeDataPrimitive->scaling);

            CORRADE_COMPARE(shapeDataAll->textures.size(), 1);
            CORRADE_COMPARE(shapeDataAll->textures.size(), shapeDataMaterial->textures.size());
        }
    }
}
#endif

}}}

CORRADE_TEST_MAIN(Magnum::DartIntegration::Test::ConvertShapeNodeTest)
