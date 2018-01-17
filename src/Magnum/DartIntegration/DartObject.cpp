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

#include <Magnum/DartIntegration/DartObject.h>

#include <dart/dynamics/BodyNode.hpp>
#include <dart/dynamics/ShapeNode.hpp>

namespace Magnum { namespace DartIntegration {

DartObject::DartObject(SceneGraph::AbstractBasicObject3D<Float>& object, SceneGraph::AbstractBasicTranslationRotation3D<Float>& transformation) : SceneGraph::AbstractBasicFeature3D<Float>(object), _transformation(transformation) {}

DartObject& DartObject::setShapeNode(dart::dynamics::ShapeNode* node) {
    _node = node;
    _body = nullptr;

    if (_node)
        this->update();

    return *this;
}

DartObject& DartObject::setBodyNode(dart::dynamics::BodyNode* body) {
    _node = nullptr;
    _body = body;

    if (_body)
        this->update();

    return *this;
}

DartObject& DartObject::update() {
    /** Get transform from DART
     * @todo Check if getting translation like this is correct
     */
    Eigen::Isometry3d trans;
    if (!_node)
        trans = _body->getRelativeTransform();
    else
        trans = _node->getRelativeTransform();

    Eigen::AngleAxisd R = Eigen::AngleAxisd(trans.linear());
    Eigen::Vector3d axis = R.axis();
    Eigen::Vector3d T = trans.translation();

    /* Convert it to axis-angle representation */
    Math::Vector3<Float> t(T[0], T[1], T[2]);
    Math::Vector3<Float> u(axis(0), axis(1), axis(2));
    Rad theta(R.angle());

    /* pass it to Magnum */
    _transformation.resetTransformation()
        .rotate(theta, u)
        .translate(t);

    return *this;
}

dart::dynamics::ShapeNode* DartObject::shapeNode() {
    return _node;
}

dart::dynamics::BodyNode* DartObject::bodyNode() {
    return _body;
}

}}