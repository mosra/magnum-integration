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

/* Using anonymous namespace as this is only used privately */

namespace Magnum { namespace DartIntegration { namespace Test { namespace {

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
    using namespace Math::Literals;

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
template<class JointType> dart::dynamics::BodyNode* addSoftBody(const dart::dynamics::SkeletonPtr& chain, const std::string& name, dart::dynamics::BodyNode* parent = nullptr) {
    constexpr Double default_shape_density = 1000; // kg/m^3
    constexpr Double default_shape_height  = 0.1;  // m
    constexpr Double default_shape_width   = 0.03; // m
    constexpr Double default_skin_thickness = 1e-3; // m

    constexpr Double default_vertex_stiffness = 1000.0;
    constexpr Double default_edge_stiffness = 1.0;
    constexpr Double default_soft_damping = 5.0;

    /* Set the Joint properties */
    typename JointType::Properties joint_properties;
    joint_properties.mName = name + "_joint";
    if(parent) {
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
    Double width = default_shape_height, height = 2*default_shape_width;
    Eigen::Vector3d dims(width, width, height);

    Double mass = 2*dims[0]*dims[1] + 2*dims[0]*dims[2] + 2*dims[1]*dims[2];
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

}}}}
