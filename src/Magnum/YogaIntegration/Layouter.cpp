/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StringView.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Ui/AbstractUserInterface.h>
#include <Magnum/Ui/Handle.h>

/** @todo Newer versions of Yoga are split into several headers and it might be
    useful to include just a subset, 2.0.1 has the C-compatible API just in
    Yoga.h. Unfortunately the damn thing doesn't expose any version macro so
    this is impossible to detect. (Also the layouter will likely need most of
    the headers anyway.) */
#include <yoga/Yoga.h>

#include "Magnum/YogaIntegration/configureInternal.h"
#if YOGA_VERSION < 300000
#include <Magnum/Math/Functions.h> /* lerp(), isNan(), see below */
#endif

namespace Magnum { namespace YogaIntegration {

Debug& operator<<(Debug& debug, const Flag value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "YogaIntegration::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Flag::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(PercentageNodeOffsetX)
        _c(PercentageNodeOffsetY)
        _c(PercentageNodeOffset)
        _c(NodeOffsetFromBottom)
        _c(NodeOffsetFromRight)
        _c(NodeOffsetFromRightBottom)
        _c(IgnoreNodeOffsetX)
        _c(IgnoreNodeOffsetY)
        _c(IgnoreNodeOffset)
        _c(PercentageNodeSizeX)
        _c(PercentageNodeSizeY)
        _c(PercentageNodeSize)
        _c(IgnoreNodeSizeX)
        _c(IgnoreNodeSizeY)
        _c(IgnoreNodeSize)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedShort(value) << Debug::nospace << (packed ? "" : ")");
}

Debug& operator<<(Debug& debug, const Flags value) {
    return Containers::enumSetDebugOutput(debug, value, debug.immediateFlags() >= Debug::Flag::Packed ? "{}" : "YogaIntegration::Flags{}", {
        /* Implies X/Y, has to be before. The "Ignore" then implies
           "Percentage" and "Right" / "Bottom", so it also has to be before. */
        Flag::IgnoreNodeOffset,
        Flag::IgnoreNodeOffsetX,
        Flag::IgnoreNodeOffsetY,
        /* Implies Right/Bottom, has to be before */
        Flag::NodeOffsetFromRightBottom,
        Flag::NodeOffsetFromRight,
        Flag::NodeOffsetFromBottom,
        /* Implies X/Y, has to be before */
        Flag::PercentageNodeOffset,
        Flag::PercentageNodeOffsetX,
        Flag::PercentageNodeOffsetY,
        /* Implies X/Y, has to be before. The "Ignore" then implies
           "Percentage", so it also has to be before. */
        Flag::IgnoreNodeSize,
        Flag::IgnoreNodeSizeX,
        Flag::IgnoreNodeSizeY,
        /* Implies X/Y, has to be before */
        Flag::PercentageNodeSize,
        Flag::PercentageNodeSizeX,
        Flag::PercentageNodeSizeY,
    });
}

/* For simplicity we expect the numeric ID match between our enums and Yoga's
   and then just cast the value. Don't want to bring the full Yoga headers in
   the public APIs so can't just use the constants. */
static_assert(UnsignedInt(FlexDirection::Column) == UnsignedInt(YGFlexDirectionColumn),
    "FlexDirection::Column value doesn't match Yoga");
static_assert(UnsignedInt(FlexDirection::ColumnReverse) == UnsignedInt(YGFlexDirectionColumnReverse),
    "FlexDirection::ColumnReverse value doesn't match Yoga");
static_assert(UnsignedInt(FlexDirection::Row) == UnsignedInt(YGFlexDirectionRow),
    "FlexDirection::Row value doesn't match Yoga");
static_assert(UnsignedInt(FlexDirection::RowReverse) == UnsignedInt(YGFlexDirectionRowReverse),
    "FlexDirection::RowReverse value doesn't match Yoga");

Debug& operator<<(Debug& debug, const FlexDirection value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "YogaIntegration::FlexDirection" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case FlexDirection::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Row)
        _c(RowReverse)
        _c(Column)
        _c(ColumnReverse)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

/* For simplicity we expect the numeric ID match between our enums and Yoga's
   and then just cast the value. Don't want to bring the full Yoga headers in
   the public APIs so can't just use the constants. */
static_assert(UnsignedInt(NodeOffsetType::Relative) == UnsignedInt(YGPositionTypeRelative),
    "NodeOffsetType::Relative value doesn't match Yoga");
static_assert(UnsignedInt(NodeOffsetType::Absolute) == UnsignedInt(YGPositionTypeAbsolute),
    "NodeOffsetType::Absolute value doesn't match Yoga");

Debug& operator<<(Debug& debug, const NodeOffsetType value) {
    const bool packed = debug.immediateFlags() >= Debug::Flag::Packed;

    if(!packed)
        debug << "YogaIntegration::NodeOffsetType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case NodeOffsetType::value: return debug << (packed ? "" : "::") << Debug::nospace << #value;
        _c(Relative)
        _c(Absolute)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << (packed ? "" : "(") << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << (packed ? "" : ")");
}

namespace {

struct Layout {
    /*implicit*/ Layout() = default;
    Layout(const Layout&) = delete;
    Layout(Layout&& other) noexcept: yogaNode{other.yogaNode} {
        other.yogaNode = nullptr;
    }
    ~Layout() {
        /* YGNodeFree() disconnects itself from its parent and disconnects all
           its children first, so it's safe to call in any order without
           causing the other node references to become dangling. It however
           crashes when passed a null pointer. LOL. */
        if(yogaNode)
            YGNodeFree(yogaNode);
    }
    Layout& operator=(const Layout&) = delete;
    Layout& operator=(Layout&& other) noexcept {
        Utility::swap(other.yogaNode, yogaNode);
        return *this;
    }

    /* Yoga node reference. If nullptr, given layout is freed. Flags are stored
       in the "context" pointer accessible by YGNodeGetContext(). */
    YGNodeRef yogaNode = nullptr;
};

}

struct Layouter::State {
    explicit State();
    State(const State&) = delete;
    State(State&&) = delete;
    ~State() {
        /* The config is allocated always, so unlike with YGNodeFree() we don't
           need to check for it being null here */
        YGConfigFree(config);
    }
    State& operator=(const State&) = delete;
    State& operator=(State&&) = delete;

    Containers::Array<Layout> layouts;

    YGConfigRef config{};
};

Layouter::State::State(): config{YGConfigNew()} {
    /* Don't snap to a pixel grid by default */
    YGConfigSetPointScaleFactor(config, 0.0f);
}

Layouter::Layouter(const Ui::LayouterHandle handle): Ui::AbstractLayouter{handle}, _state{InPlaceInit} {}

Layouter::Layouter(Layouter&&) noexcept = default;

Layouter::~Layouter() = default;

Layouter& Layouter::operator=(Layouter&&) noexcept = default;

Ui::LayouterFeatures Layouter::doFeatures() const {
    return Ui::LayouterFeature::UniqueLayouts;
}

YGConfigRef Layouter::yogaConfig() {
    return _state->config;
}

YGConfigConstRef Layouter::yogaConfig() const {
    return _state->config;
}

Ui::LayoutHandle Layouter::add(const Ui::NodeHandle node, const Ui::LayoutHandle before, const Flags flags) {
    CORRADE_ASSERT(before == Ui::LayoutHandle::Null || isHandleValid(before),
        "YogaIntegration::Layouter::add(): invalid before handle" << before, {});
    return addInternal(node, layoutHandleData(before), flags);
}

Ui::LayoutHandle Layouter::add(const Ui::NodeHandle node, const Ui::LayouterDataHandle before, const Flags flags) {
    CORRADE_ASSERT(before == Ui::LayouterDataHandle::Null || isHandleValid(before),
        "YogaIntegration::Layouter::add(): invalid before handle" << before, {});
    return addInternal(node, before, flags);
}

Ui::LayoutHandle Layouter::addInternal(const Ui::NodeHandle node, const Ui::LayouterDataHandle before, const Flags flags) {
    State& state = *_state;

    /* Validity of `before` was checked in add() already, validity of `node` is
       and presence of ui() is checked inside AbstractLayouter::add() */
    const Ui::LayoutHandle handle = Ui::AbstractLayouter::add(node);
    const UnsignedInt id = layoutHandleId(handle);
    if(id >= state.layouts.size())
        arrayResize(state.layouts, ValueInit, id + 1);
    Layout& layout = state.layouts[id];

    /* Create a new Yoga node using the global config. The slot should have the
       Yoga node reference cleared and the Yoga node freed on removal, so it
       should be always null at this point. */
    CORRADE_INTERNAL_ASSERT(!layout.yogaNode);
    layout.yogaNode = YGNodeNewWithConfig(state.config);
    /* Store the flags in the (otherwise unused) context pointer. Because the
       flags are 2 bytes, storing them next to the YGNodeRef would waste 6
       padding bytes. OTOH, the YGNode itself is 584 bytes (!!), not counting
       whatever else it allocates internally for tracking children and such,
       so the 8 bytes saved feel like not much in comparison. */
    YGNodeSetContext(layout.yogaNode, reinterpret_cast<void*>(std::size_t(UnsignedShort(flags))));

    /* If there's no parent node or the parent node has no layout, `before` can
       only be null and we don't insert any child anywhere */
    const Ui::NodeHandle nodeParent = ui().nodeParent(node);
    const Ui::LayouterDataHandle layoutParent = nodeParent == Ui::NodeHandle::Null ?
        Ui::LayouterDataHandle::Null : ui().nodeUniqueLayout(nodeParent, *this);
    if(layoutParent == Ui::LayouterDataHandle::Null) {
        CORRADE_ASSERT(before == Ui::LayouterDataHandle::Null,
            "YogaIntegration::Layouter::add(): expected before to be null for" << node << "without a parent layout but got" << before, {});

    /* Otherwise it's expected to be either null or a sibling, and we add a
       child */
    } else {
        CORRADE_ASSERT(before == Ui::LayouterDataHandle::Null || ui().nodeParent(this->node(before)) == nodeParent,
            "YogaIntegration::Layouter::add(): expected before to be a child of" << nodeParent << "but" << before << "is a child of" << ui().nodeParent(this->node(before)), {});

        /* If we're adding at the end, the index is the current child count of
           the parent Yoga node. Otherwise go through all children of the
           parent Yoga node (yes, linearly, because there's no other way), and
           find the child by comparing the pointers.

           Maybe could avoid the linear lookup by maintaining current child
           index in our per-layout data, and update it every time an insertion
           or removal happens, but ultimately that is a linear operation as
           well, so it would only move the problem elsewhere. And since
           insertion in the middle of a std::vector (yes, that's what Yoga uses
           internally for storing children, LOL) is a linear operation anyway,
           there's not really a way to avoid that besides simply not inserting
           in the middle. Thus let's just hope the majority use case is the
           optimal one, i.e. appending at the end. */
        const YGNodeRef parentYgNode = state.layouts[Ui::layouterDataHandleId(layoutParent)].yogaNode;
        const UnsignedInt childCount = YGNodeGetChildCount(parentYgNode);
        UnsignedInt index;
        if(before == Ui::LayouterDataHandle::Null)
            index = childCount;
        else {
            const YGNodeConstRef desiredYgNode = state.layouts[Ui::layouterDataHandleId(before)].yogaNode;
            for(index = 0; index != childCount; ++index)
                if(YGNodeGetChild(parentYgNode, index) == desiredYgNode)
                    break;
            CORRADE_INTERNAL_ASSERT(index != childCount);
        }

        YGNodeInsertChild(parentYgNode, layout.yogaNode, index);
    }

    /* The default in 2.0.1 was static position, which (apparently?) behaved
       exactly the same as relative. In 3.0.0 real support for static position
       got implemented, and so the default got changed:
        https://github.com/facebook/yoga/pull/1469
       To make things sane for our API, change the default to relative for old
       versions as well. */
    #if YOGA_VERSION < 300000
    YGNodeStyleSetPositionType(layout.yogaNode, YGPositionTypeRelative);
    #endif

    return handle;
}

void Layouter::remove(const Ui::LayoutHandle handle) {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::remove(): invalid handle" << handle, );
    Layout& layout = _state->layouts[layoutHandleId(handle)];
    CORRADE_ASSERT(!YGNodeGetChildCount(layout.yogaNode),
        "YogaIntegration::Layouter::remove(): cannot remove" << handle << "with" << YGNodeGetChildCount(layout.yogaNode) << "children", );
    layout = {};
    AbstractLayouter::remove(handle);
}

void Layouter::remove(const Ui::LayouterDataHandle handle) {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::remove(): invalid handle" << handle, );
    Layout& layout = _state->layouts[layouterDataHandleId(handle)];
    CORRADE_ASSERT(!YGNodeGetChildCount(layout.yogaNode),
        "YogaIntegration::Layouter::remove(): cannot remove" << handle << "with" << YGNodeGetChildCount(layout.yogaNode) << "children", );
    layout = {};
    AbstractLayouter::remove(handle);
}

YGNodeRef Layouter::yogaNode(const Ui::LayoutHandle handle) {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::yogaNode(): invalid handle" << handle, {});
    return _state->layouts[layoutHandleId(handle)].yogaNode;
}

YGNodeConstRef Layouter::yogaNode(const Ui::LayoutHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::yogaNode(): invalid handle" << handle, {});
    return _state->layouts[layoutHandleId(handle)].yogaNode;
}

YGNodeRef Layouter::yogaNode(const Ui::LayouterDataHandle handle) {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::yogaNode(): invalid handle" << handle, {});
    return _state->layouts[layouterDataHandleId(handle)].yogaNode;
}

YGNodeConstRef Layouter::yogaNode(const Ui::LayouterDataHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::yogaNode(): invalid handle" << handle, {});
    return _state->layouts[layouterDataHandleId(handle)].yogaNode;
}

Flags Layouter::flags(const Ui::LayoutHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::flags(): invalid handle" << handle, {});
    return Flags(reinterpret_cast<std::size_t>(YGNodeGetContext(_state->layouts[layoutHandleId(handle)].yogaNode)));
}

Flags Layouter::flags(const Ui::LayouterDataHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::flags(): invalid handle" << handle, {});
    return Flags(reinterpret_cast<std::size_t>(YGNodeGetContext(_state->layouts[layouterDataHandleId(handle)].yogaNode)));
}

FlexDirection Layouter::flexDirection(const Ui::LayoutHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::flexDirection(): invalid handle" << handle, {});
    return FlexDirection(Int(YGNodeStyleGetFlexDirection(_state->layouts[layoutHandleId(handle)].yogaNode)));
}

FlexDirection Layouter::flexDirection(const Ui::LayouterDataHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::flexDirection(): invalid handle" << handle, {});
    return FlexDirection(Int(YGNodeStyleGetFlexDirection(_state->layouts[layouterDataHandleId(handle)].yogaNode)));
}

void Layouter::setFlexDirection(const Ui::LayoutHandle handle, const FlexDirection direction) {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::setFlexDirection(): invalid handle" << handle, );
    setFlexDirectionInternal(layoutHandleId(handle), direction);
}

void Layouter::setFlexDirection(const Ui::LayouterDataHandle handle, const FlexDirection direction) {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::setFlexDirection(): invalid handle" << handle, );
    setFlexDirectionInternal(layouterDataHandleId(handle), direction);
}

void Layouter::setFlexDirectionInternal(const UnsignedInt id, const FlexDirection direction) {
    YGNodeStyleSetFlexDirection(_state->layouts[id].yogaNode, YGFlexDirection(Int(direction)));
    setNeedsUpdate();
}

NodeOffsetType Layouter::nodeOffsetType(const Ui::LayoutHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::nodeOffsetType(): invalid handle" << handle, {});
    return NodeOffsetType(Int(YGNodeStyleGetPositionType(_state->layouts[layoutHandleId(handle)].yogaNode)));
}

NodeOffsetType Layouter::nodeOffsetType(const Ui::LayouterDataHandle handle) const {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::nodeOffsetType(): invalid handle" << handle, {});
    return NodeOffsetType(Int(YGNodeStyleGetPositionType(_state->layouts[layouterDataHandleId(handle)].yogaNode)));
}

void Layouter::setNodeOffsetType(const Ui::LayoutHandle handle, const NodeOffsetType type) {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::setNodeOffsetType(): invalid handle" << handle, );
    setNodeOffsetTypeInternal(layoutHandleId(handle), type);
}

void Layouter::setNodeOffsetType(const Ui::LayouterDataHandle handle, const NodeOffsetType type) {
    CORRADE_ASSERT(isHandleValid(handle),
        "YogaIntegration::Layouter::setNodeOffsetType(): invalid handle" << handle, );
    setNodeOffsetTypeInternal(layouterDataHandleId(handle), type);
}

void Layouter::setNodeOffsetTypeInternal(const UnsignedInt id, const NodeOffsetType type) {
    YGNodeStyleSetPositionType(_state->layouts[id].yogaNode, YGPositionType(Int(type)));
    setNeedsUpdate();
}

void Layouter::doClean(const Containers::BitArrayView layoutIdsToRemove) {
    State& state = *_state;
    /** @todo some way to iterate set bits */
    for(std::size_t i = 0; i != layoutIdsToRemove.size(); ++i) {
        /* Unlike in remove(), here we don't check for children as doClean() is
           only called in response to removing a node along with all its
           children (and thus also all child layouts) */
        if(layoutIdsToRemove[i])
            /** @todo YGNodeFinalize() doesn't disconnect parent and children
                and thus could be faster than YGNodeFree() when removing many
                nodes, however there still needs to be something that
                disconnect the top-most node from its parent */
            state.layouts[i] = {};
    }
}

void Layouter::doUpdate(const Containers::BitArrayView layoutIdsToUpdate, const Containers::StridedArrayView1D<const UnsignedInt>& topLevelDataIds, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Float>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) {
    State& state = *_state;
    const Containers::StridedArrayView1D<const Ui::NodeHandle> nodes = this->nodes();

    /* Sync the Yoga node tree with current node properties */
    for(std::size_t id = 0; id != layoutIdsToUpdate.size(); ++id) {
        /** @todo some way to iterate set bits */
        if(!layoutIdsToUpdate[id])
            continue;

        const UnsignedInt nodeId = nodeHandleId(nodes[id]);
        const YGNodeRef yogaNode = state.layouts[id].yogaNode;
        const Flags flags = Flags(reinterpret_cast<std::size_t>(YGNodeGetContext(yogaNode)));

        /* Node offset. For top/left edges a zero offset seems to be treated
           the same way as the (default) YGUndefined value, and staying at
           defaults is generally preferrable. However, specifying bottom/right
           edges with YGUndefined is ... also a default, and thus treated the
           same as not setting bottom/right at all, meaning that if we specify
           a zero offset from the bottom/right corner, nothing gets done. Thus,
           unlike with sizes below, even a zero offset is explicitly set. I
           hope I didn't miss some scenario where this blows up, worst case the
           user can use the IgnoreNodeOffset* flags.

           Yoga 3.2.0 then adds YGNodeStyleSetPositionAuto(), but according to
            https://github.com/facebook/react-native/issues/45817
            https://github.com/facebook/yoga/commit/6d6f69bee74f92e38d2a002d6ba20e25ef7c3dd7
           it only seems that "auto" gets now treated the same as undefined, so
           there's no real value in using this new API call compared to just
           passing YGUndefined. And given that YGUndefined is treated mostly
           the same as 0, why exactly are there three distinct ways to do the
           same thing, FFS?!

           We need to explicitly set even the zero value however, if we
           wouldn't do anything then the value would stay at whatever was set
           before. See the updateModifyNodeOffsetSize() test for a repro
           case. */
        if(!(flags >= Flag::IgnoreNodeOffsetX)) {
            const YGEdge edge = flags >= Flag::NodeOffsetFromRight ?
                YGEdge::YGEdgeRight : YGEdge::YGEdgeLeft;
            const Float x = nodeOffsets[nodeId].x();
            if(flags >= Flag::PercentageNodeOffsetX)
                YGNodeStyleSetPositionPercent(yogaNode, edge, x);
            else YGNodeStyleSetPosition(yogaNode, edge, x);
        }
        if(!(flags >= Flag::IgnoreNodeOffsetY)) {
            const YGEdge edge = flags >= Flag::NodeOffsetFromBottom ?
                YGEdge::YGEdgeBottom : YGEdge::YGEdgeTop;
            const Float y = nodeOffsets[nodeId].y();
            if(flags >= Flag::PercentageNodeOffsetY)
                YGNodeStyleSetPositionPercent(yogaNode, edge, y);
            else YGNodeStyleSetPosition(yogaNode, edge, y);
        }

        /* Node size. Similarly to the reasoning for auto/undefined offset
           above, it seems that YGNodeStyleSet{Width,Height}Auto() internally
           does the same as setting nothing at all, i.e. keeping the value
           undefined. Again we need to explicitly set the undefined value
           however, if we wouldn't do anything then the value would stay at
           whatever was set before. See the updateModifyNodeOffsetSize() test
           for a repro case. */
        if(!(flags >= Flag::IgnoreNodeSizeX)) {
            if(const Float x = nodeSizes[nodeId].x()) {
                if(flags >= Flag::PercentageNodeSizeX)
                    YGNodeStyleSetWidthPercent(yogaNode, x);
                else YGNodeStyleSetWidth(yogaNode, x);
            } else YGNodeStyleSetWidth(yogaNode, YGUndefined);
        }
        if(!(flags >= Flag::IgnoreNodeSizeY)) {
            if(const Float y = nodeSizes[nodeId].y()) {
                if(flags >= Flag::PercentageNodeSizeY)
                    YGNodeStyleSetHeightPercent(yogaNode, y);
                else YGNodeStyleSetHeight(yogaNode, y);
            } else YGNodeStyleSetHeight(yogaNode, YGUndefined);
        }
    }

    /* Calculate layouts for all top-level hierarchies */
    for(UnsignedInt topLevelDataId: topLevelDataIds) {
        YGNodeCalculateLayout(state.layouts[topLevelDataId].yogaNode,
            /* For some FUCKING reason there's no possibility to pass in the UI
               size while keeping the top-level node scaled down to just fit
               its contents, i.e. with its width/height kept at 0 so it picks
               YGNodeStyleSet{Width,Height}Auto() above. Passing the UI size
               causes the auto-sized node to fill given direction completely,
               always, with no way to override that. Things tried so far:

                - Using a non-zero offset, does nothing
                - Setting YGNodeSetAlignSelf() to YGAlignCenter (while the
                  default is YGAlignStretch), does nothing
                - Setting auto margin on all sides, does nothing

               Fucking CSS flexbox crap. Such a simple thing, impossible. The
               only way I can see this working is to have TWO STUPID NODES, the
               outer being stretched all over the place, and the inner using
               some cryptic incantation of item/content/flex/self alignment to
               center instead of stretch. This also means it's so far
               impossible to center top-level nodes in the UI because of this
               shitty thing. */
            /** @todo hm, so maybe do that? create an internal outer node that
                expands over the whole thing always, and the user-visible node
                then can be configured and it works reasonably? however it also
                means the UI size will go to the extra outer node, and not
                here, so this argument is ALSO a completely useless piece of
                crap, like the other one below */
            YGUndefined, YGUndefined,
            /* Direction is set to YGDirectionInherit, which defaults to LTR.
               Particular top-level nodes can then override it to e.g.
               YGDirectionRTL, which makes it pick up the other direction for a
               particular layout. In other words, there doesn't seem to be a
               reason to expose a direction setting because the same can be
               achieved with YGNodeStyleSetDirection() as well. */
            YGDirectionInherit);
    }

    /* Apply the Yoga output to node offsets and sizes */
    for(std::size_t id = 0; id != layoutIdsToUpdate.size(); ++id) {
        /** @todo some way to iterate set bits */
        if(!layoutIdsToUpdate[id])
            continue;

        const UnsignedInt nodeId = nodeHandleId(nodes[id]);
        const YGNodeRef yogaNode = state.layouts[id].yogaNode;
        nodeOffsets[nodeId] = {YGNodeLayoutGetLeft(yogaNode),
                               YGNodeLayoutGetTop(yogaNode)};
        nodeSizes[nodeId] = {YGNodeLayoutGetWidth(yogaNode),
                             YGNodeLayoutGetHeight(yogaNode)};
        /* In Yoga before version 3.0.0, NaNs sometimes leak through offset
           values. Not sure if sizes as well, not sure what exactly fixed this
           but the following commit seems to be related:
            https://github.com/facebook/yoga/commit/2734784ddb4469402cebcd37adb34b5b56be9483 */
        #if YOGA_VERSION < 300000
        /* Idiotic std::initializer_list that cannot preserve mutable
           references, fucking C++ just couldn't make plain C arrays work
           here?? */
        for(Vector2* i: {&nodeOffsets[nodeId], &nodeSizes[nodeId]})
            *i = Math::lerp(*i, Vector2{Math::ZeroInit}, Math::isNan(*i));
        #endif
        /** @todo anything to do with YGNodeLayoutGetHadOverflow()? */
    }
}

void Layouter::DebugIntegration::print(Debug& debug, const Layouter& layouter, const Containers::StringView& layouterName, const Ui::LayouterDataHandle layout) {
    debug << "  Layout" << Debug::packed << layout << "from layouter" << Debug::packed << layouter.handle();
    if(layouterName)
        debug << Debug::color(Debug::Color::Yellow) << layouterName << Debug::resetColor;
    debug << Debug::newline;

    const Flags flags = layouter.flags(layout);
    if(flags) {
        debug << "    Flags:" << Debug::color(Debug::Color::Cyan) << Debug::packed << flags << Debug::resetColor << Debug::newline;
    }

    debug << "    Flex direction:" << Debug::color(Debug::Color::Cyan) << Debug::packed << layouter.flexDirection(layout) << Debug::resetColor << Debug::newline;

    /* Print node offset type only if it's not the relative default, which is
       likely the case for majority of layouts and it'd be just extra noise */
    const NodeOffsetType nodeOffsetType = layouter.nodeOffsetType(layout);
    if(nodeOffsetType != NodeOffsetType::Relative)
        debug << "    Node offset type:" << Debug::color(Debug::Color::Cyan) << Debug::packed << nodeOffsetType << Debug::resetColor << Debug::newline;
}

}}
