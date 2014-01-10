#ifndef Magnum_BulletIntegration_MotionState_h
#define Magnum_BulletIntegration_MotionState_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2013 Jan Dupal <dupal.j@gmail.com>

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
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/AbstractTranslationRotation3D.h>

#include "magnumBulletIntegrationVisibility.h"

namespace Magnum { namespace BulletIntegration {

/**
@brief Bullet Physics motion state

Encapsulates `btMotionState` as @ref SceneGraph feature.

@todoc Usage...
*/
class MAGNUM_BULLETINTEGRATION_EXPORT MotionState: public SceneGraph::AbstractBasicFeature3D<btScalar>, private btMotionState {
    public:
        /**
         * @brief Constructor
         * @param object    Object this motion state belongs to
         */
        template<class T> MotionState(T& object);

        /** @brief Motion state */
        inline btMotionState& btMotionState() { return *this; }

    private:
        void MAGNUM_BULLETINTEGRATION_LOCAL getWorldTransform(btTransform& worldTrans) const override;
        void MAGNUM_BULLETINTEGRATION_LOCAL setWorldTransform(const btTransform& worldTrans) override;

        SceneGraph::AbstractBasicTranslationRotation3D<btScalar>& transformation;
};

template<class T> MotionState::MotionState(T& object): SceneGraph::AbstractBasicFeature3D<btScalar>(object), transformation(object) {}

}}

#endif
