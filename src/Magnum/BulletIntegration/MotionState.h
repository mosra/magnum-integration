#ifndef Magnum_BulletIntegration_MotionState_h
#define Magnum_BulletIntegration_MotionState_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2013 Jan Dupal <dupal.j@gmail.com>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 Max Schwarz <max.schwarz@online.de>

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
 * @brief Class @ref Magnum::BulletIntegration::MotionState
 */

#include <LinearMath/btMotionState.h>
#include <Magnum/SceneGraph/AbstractFeature.h>
#include <Magnum/SceneGraph/AbstractTranslationRotation3D.h>

#include "Magnum/BulletIntegration/visibility.h"

namespace Magnum { namespace BulletIntegration {

/**
@brief Bullet Physics motion state

Encapsulates `btMotionState` as a @ref SceneGraph feature.

@section BulletIntegration-MotionState-usage Usage

Common usage is to either create a `btRigidBody` to share transformation with
a @ref SceneGraph::Object by passing the motion state in its constructor:

@snippet BulletIntegration.cpp MotionState-usage

This way, the scene graph will be affected automatically by the Bullet
simulation. It's also possible to attach the motion state afterwards:

@snippet BulletIntegration.cpp MotionState-usage-after

Note that in the other direction, the transform is propagated from the scene
graph to the rigid body only during the initial creation of the
@cpp btRigidBody @ce or when you explicitly set the body as kinematic (which
however makes it mostly a passive collision object):

@snippet BulletIntegration.cpp MotionState-kinematic

In order to update the transformation of a non-kinematic body, set the
transform directly on the @cpp btRigidBody @ce:

@snippet BulletIntegration.cpp MotionState-update

Keep in mind that hanges to a rigid body using
@cpp btRigidBody::setWorldTransform() @ce may only update the motion state of
non-static objects and while @cpp btDynamicsWorld::stepSimulation() @ce is
called.

@attention All objects with a @ref MotionState attached that are part of the
    same Bullet world need to have a single common parent object, otherwise the
    transformations will not propagate correctly.
*/
class MAGNUM_BULLETINTEGRATION_EXPORT MotionState: public SceneGraph::AbstractBasicFeature3D<btScalar>, private btMotionState {
    public:
        /**
         * @brief Constructor
         * @param object    Object this motion state belongs to
         */
        template<class T> MotionState(T& object): MotionState{object, object} {}

        ~MotionState();

        /** @brief Motion state */
        btMotionState& btMotionState() { return *this; }

    private:
        explicit MotionState(SceneGraph::AbstractBasicObject3D<btScalar>& object, SceneGraph::AbstractBasicTranslationRotation3D<btScalar>& transformation);

        void MAGNUM_BULLETINTEGRATION_LOCAL getWorldTransform(btTransform& worldTrans) const override;
        void MAGNUM_BULLETINTEGRATION_LOCAL setWorldTransform(const btTransform& worldTrans) override;

        SceneGraph::AbstractBasicTranslationRotation3D<btScalar>& _transformation;
        bool _broken{false};
};

}}

#endif
