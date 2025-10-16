/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "Layouter.h"

#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Ui/Handle.h>

namespace Magnum { namespace YogaIntegration {

struct Layouter::State {
    Containers::Array<YGNodeRef> data;
};

Layouter::Layouter(const Ui::LayouterHandle handle): Ui::AbstractLayouter{handle}, _state{InPlaceInit} {}

Layouter::Layouter(Layouter&&) noexcept = default;

Layouter::~Layouter() = default;

Layouter& Layouter::operator=(Layouter&&) noexcept = default;

Ui::LayoutHandle Layouter::add(const Ui::NodeHandle node, const Ui::LayoutHandle parent) {
    State& state = *_state;

    CORRADE_INTERNAL_ASSERT(parent == Ui::LayoutHandle::Null || isHandleValid(parent));

    Ui::LayoutHandle handle = Ui::AbstractLayouter::add(node);
    const UnsignedInt id = layoutHandleId(handle);
    if(id >= state.data.size()) {
        arrayResize(state.data, ValueInit, id + 1);
    }

    YGNodeRef ygNode;

    if(parent == Ui::LayoutHandle::Null) {
        YGConfigRef config = YGConfigNew();
        ygNode = YGNodeNewWithConfig(config);
        YGNodeStyleSetFlexDirection(ygNode, YGFlexDirectionRow);
        YGNodeStyleSetPadding(ygNode, YGEdgeAll, 20);
        YGNodeStyleSetMargin(ygNode, YGEdgeAll, 20);
    } else {
        ygNode = YGNodeNew();
        YGNodeStyleSetAlignSelf(ygNode, YGAlignCenter);
        YGNodeStyleSetMargin(ygNode, YGEdgeAll, 10);

        YGNodeRef ygParent = state.data[layoutHandleId(parent)];

        YGNodeInsertChild(ygParent, ygNode, YGNodeGetChildCount(ygParent));
    }

    state.data[id] = ygNode;
    return handle;
}

YGNodeRef Layouter::yogaNode(const Ui::LayoutHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::yogaNode(): invalid handle" << handle, {});
    return _state->data[layoutHandleId(handle)];
}

YGNodeRef Layouter::yogaNode(const Ui::LayouterDataHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::yogaNode(): invalid handle" << handle, {});
    return _state->data[layouterDataHandleId(handle)];
}

void Layouter::doUpdate(const Containers::BitArrayView layoutIdsToUpdate, const Containers::StridedArrayView1D<const UnsignedInt>& topLevelDataIds, const Containers::StridedArrayView1D<const Ui::NodeHandle>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) {
    State& state = *_state;
    const Containers::StridedArrayView1D<const Ui::NodeHandle> nodes = this->nodes();

    for(std::size_t id = 0; id != layoutIdsToUpdate.size(); ++id) {
        /** @todo some way to iterate set bits */
        if(!layoutIdsToUpdate[id])
            continue;
        const UnsignedInt nodeId = nodeHandleId(nodes[id]);
        const YGNodeRef yogaNode = state.data[id];
        /* Don't set the size on the root items */
        if(YGNodeGetParent(yogaNode)) {
            if(const Float x = nodeSizes[nodeId].x())
                YGNodeStyleSetWidth(yogaNode, x);
            else
                YGNodeStyleSetWidthAuto(yogaNode);
            if(const Float y = nodeSizes[nodeId].y())
                YGNodeStyleSetHeight(yogaNode, y);
            else
                YGNodeStyleSetHeightAuto(yogaNode);
        }
    }

    for(UnsignedInt topLevelDataId: topLevelDataIds) {
        const UnsignedInt nodeId = nodeHandleId(nodes[topLevelDataId]);
        YGNodeCalculateLayout(state.data[topLevelDataId], nodeSizes[nodeId].x(), nodeSizes[nodeId].y(), YGDirectionLTR);
    }

    for(std::size_t id = 0; id != layoutIdsToUpdate.size(); ++id) {
        /** @todo some way to iterate set bits */
        if(!layoutIdsToUpdate[id])
            continue;
        const UnsignedInt nodeId = nodeHandleId(nodes[id]);
        const YGNodeRef yogaNode = state.data[id];
        nodeOffsets[nodeId] = {YGNodeLayoutGetLeft(yogaNode),
                               YGNodeLayoutGetTop(yogaNode)};
        nodeSizes[nodeId] = {YGNodeLayoutGetWidth(yogaNode),
                             YGNodeLayoutGetHeight(yogaNode)};
    }
}

}}

