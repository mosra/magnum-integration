#ifndef Magnum_DARTIntegration_DartSkeleton_h
#define Magnum_DARTIntegration_DartSkeleton_h
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
 * @brief Class @ref Magnum::DartIntegration::DartSkeleton
 */

#include <functional>
#include <memory>
#include <vector>

#include <dart/dynamics/BodyNode.hpp>
#include <dart/dynamics/ShapeNode.hpp>
#include <dart/dynamics/Skeleton.hpp>

#include <Magnum/DartIntegration/DartObject.h>

namespace Magnum { namespace DartIntegration {
/**
@brief Dart Physics Skeleton (i.e., Multi-Body entity)

Parses a Dart `Skeleton` for easy usage in Magnum. It basically
parses the `Skeleton` and keeps track of a list of @ref DartObject

## Usage

Common usage is to create a Dart `SkeletonPtr` and then instantiate 
a `DartSkeleton` by passing a parent @ref SceneGraph feature and 
the `SkeletonPtr` to its constructor:

@code
dart::dynamics::SkeletonPtr skel = createSkeletonInDart();
SceneGraph::Object<SceneGraph::MatrixTransformation3D> object;
DartSkeleton* dartSkel = new DartSkeleton{object, skel};

// only the Dart Skeleton can affect the transformation of the Magnum
// DartSkeleton and not the other way around
// to get the latest Dart transformations, you should update the objects
// of dartSkel
// dartSkel->updateObjects();
@endcode

## Limitations

- `SoftBodyNode`s are not supported yet
- When changing the structure of a `SkeletonPtr`, @ref DartObject will not automatically update
*/

class MAGNUM_DARTINTEGRATION_EXPORT DartSkeleton {
    public:
         /**
         * @brief Constructor
         * @param parent    parent Object
         * @param skeleton  Dart SkeletonPtr to parse
         */
        template <typename T> explicit DartSkeleton(T& parent, dart::dynamics::SkeletonPtr skeleton = nullptr) {
            if (skeleton)
                parseSkeleton(parent, skeleton);
        }

        /**
         * @brief parse a Dart Skeleton
         * @param parent    parent Object
         * @param skeleton  Dart SkeletonPtr to parse
         */
        template <typename T> DartSkeleton& parseSkeleton(T& parent, dart::dynamics::SkeletonPtr skeleton) {
            /* @todo Support more than one kinematic trees */
            if (skeleton) {
                _objects.clear();
                _parseBodyNodeRecursive(parent, *skeleton->getRootBodyNode());
            }

            return *this;
        }

        /** @brief update all child objects */
        DartSkeleton& updateObjects();

        /** @brief get all objects */
        std::vector<std::reference_wrapper<DartObject>> objects();
        /** @brief get all objects associated with `ShapeNode`s */
        std::vector<std::reference_wrapper<DartObject>> shapeObjects();
        /** @brief get all objects associated with `BodyNode`s */
        std::vector<std::reference_wrapper<DartObject>> bodyObjects();

    private:
        std::vector<std::unique_ptr<DartObject>> _objects;

        template <typename T> void _parseBodyNodeRecursive(T& parent, dart::dynamics::BodyNode& bn) {
            /** parse the BodyNode
             * we care only about visuals
             */
            auto visualShapes = bn.getShapeNodesWith<dart::dynamics::VisualAspect>();

            /* create an object of the BodyNode to keep track of transformations */
            auto object = new T{&parent};
            _objects.push_back(std::unique_ptr<DartObject>(new DartObject{*object, &bn}));
            for (auto& shape : visualShapes) {
                /* create objects for the ShapeNodes to keep track of inner transformations */
                auto shapeObj = new T{object};
                _objects.emplace_back(std::unique_ptr<DartObject>(new DartObject{*shapeObj, shape}));
            }

            /* parse the children recursively */
            std::size_t numChilds = bn.getNumChildBodyNodes();
            for (std::size_t i = 0; i < numChilds; i++) {
                /* pass as parent the newly created object */
                _parseBodyNodeRecursive(*object, *bn.getChildBodyNode(i));
            }
        }
};
}}

#endif