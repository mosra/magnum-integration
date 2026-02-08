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

#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/Function.h> /* for debugIntegration() */
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Ui/AbstractUserInterface.h>
#include <Magnum/Ui/DebugLayer.h> /* for debugIntegration() */
#include <Magnum/Ui/Handle.h>
#include <Magnum/Ui/NodeFlags.h>

/** @todo Newer versions of Yoga are split into several headers and it might be
    useful to include just a subset, 2.0.1 has the C-compatible API just in
    Yoga.h. Unfortunately the damn thing doesn't expose any version macro so
    this is impossible to detect. */
#include <yoga/Yoga.h>

#include "Magnum/YogaIntegration/Layouter.h"
#include "Magnum/YogaIntegration/configureInternal.h" /* YOGA_VERSION */

namespace Magnum { namespace YogaIntegration { namespace Test { namespace {

struct LayouterTest: TestSuite::Tester {
    explicit LayouterTest();

    void debugFlag();
    void debugFlagPacked();
    void debugFlags();
    void debugFlagsPacked();
    void debugFlagsSupersets();
    void debugFlexDirection();
    void debugFlexDirectionPacked();
    void debugNodeOffsetType();
    void debugNodeOffsetTypePacked();

    void construct();
    void constructCopy();
    void constructMove();

    void addRemove();
    void addRemoveHandleRecycle();
    void addRemoveInvalid();

    void flexDirection();
    void nodeOffsetType();

    void invalidHandle();

    void clean();
    void updateEmpty();
    void updateDataOrder();
    void updateFlags();

    void updateModifyNodeOffsetSize();
    void updateModifyNodeVisibility();
    void updateModifyLayoutFlexDirection();
    void updateModifyLayoutNodeOffsetType();
    void updateModifyYogaConfigDirectly();
    void updateModifyYogaNodeDirectly();

    void debugIntegration();
    void debugIntegrationNoCallback();
};

const struct {
    const char* name;
    bool layoutsInNodeOrder;
} UpdateDataOrderData[]{
    {"layouts created in node order", true},
    {"layouts created randomly", false}
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    Flags flags;
    Containers::Optional<FlexDirection> parentFlexDirection;
    Containers::Optional<NodeOffsetType> childNodeOffsetType;
    Vector2 offset, size;
    void(*call)(YGNodeRef parent, YGNodeRef child);
    /* Outer node size is {200, 50} */
    Vector2 expectedOffset, expectedSize;
} UpdateFlagsData[]{
    {"", {}, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}},
    {"percentage X offset",
        Flag::PercentageNodeOffsetX, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* 15% out of 200 */
        {30.0f, 5.0f}, {25.0f, 35.0f}},
    {"percentage Y offset",
        Flag::PercentageNodeOffsetY, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* 5% out of 50 */
        {15.0f, 2.5f}, {25.0f, 35.0f}},
    {"percentage XY offset",
        Flag::PercentageNodeOffset, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* {15%, 5%} out of {200, 50} */
        {30.0f, 2.5f}, {25.0f, 35.0f}},
    /* By default, with relative position, setting the right edge will move the
       offset by that amount to the left, etc
        https://www.yogalayout.dev/docs/styling/insets */
    {"offset from the right instead of left",
        Flag::NodeOffsetFromRight, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        {-15.0f, 5.0f}, {25.0f, 35.0f}},
    {"offset from the bottom instead of right",
        Flag::NodeOffsetFromBottom, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        {15.0f, -5.0f}, {25.0f, 35.0f}},
    {"percentage offset from bottom right instead of top left",
        Flag::PercentageNodeOffset|Flag::NodeOffsetFromRightBottom, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* {15%, 5%} out of {200, 50} */
        {-30.0f, -2.5f}, {25.0f, 35.0f}},
    {"percentage X size",
        Flag::PercentageNodeSizeX, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* 25% out of 200 */
        {15.0f, 5.0f}, {50.0f, 35.0f}},
    {"percentage Y size",
        Flag::PercentageNodeSizeY, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* 35% out of 50 */
        {15.0f, 5.0f}, {25.0f, 17.5f}},
    {"percentage XY size",
        Flag::PercentageNodeSize, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* {25%, 35%} out of {200, 50} */
        {15.0f, 5.0f}, {50.0f, 17.5f}},
    {"percentage X offset and size",
        Flag::PercentageNodeOffsetX|Flag::PercentageNodeSizeX, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* 15%, 25% out of 200 */
        {30.0f, 5.0f}, {50.0f, 35.0f}},
    {"percentage Y offset and size",
        Flag::PercentageNodeOffsetY|Flag::PercentageNodeSizeY, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* 5%, 35% out of 50 */
        {15.0f, 2.5f}, {25.0f, 17.5f}},
    {"percentage XY offset and size",
        Flag::PercentageNodeOffset|Flag::PercentageNodeSize, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* {15%, 5%}, {25%, 35%} out of 200, 50} */
        {30.0f, 2.5f}, {50.0f, 17.5f}},
    /* The Ignore* flags are superset of Percentage*, so they should get a
       precedence in the code */
    {"ignore X offset",
        Flag::IgnoreNodeOffsetX, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        { 0.0f, 5.0f}, {25.0f, 35.0f}},
    {"ignore Y offset",
        Flag::IgnoreNodeOffsetY, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        {15.0f, 0.0f}, {25.0f, 35.0f}},
    {"ignore XY offset",
        Flag::IgnoreNodeOffset, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        { 0.0f, 0.0f}, {25.0f, 35.0f}},
    /* The default is a flex column layout, which apparently sets the nodes to
       match parent width if not specified explicitly */
    {"ignore X size",
        Flag::IgnoreNodeSizeX, {}, {},
        {15.0f, 5.0f}, { 25.0f, 35.0f}, {},
        {15.0f, 5.0f}, {200.0f, 35.0f}},
    {"ignore Y size",
        Flag::IgnoreNodeSizeY, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f}, {},
        {15.0f, 5.0f}, {25.0f,  0.0f}},
    {"ignore XY size",
        Flag::IgnoreNodeSize, {}, {},
        {15.0f, 5.0f}, { 25.0f, 35.0f}, {},
        {15.0f, 5.0f}, {200.0f,  0.0f}},
    {"ignore XY offset + size",
        Flag::IgnoreNodeOffset|Flag::IgnoreNodeSize, {}, {},
        {15.0f, 5.0f}, { 25.0f, 35.0f}, {},
        { 0.0f, 0.0f}, {200.0f,  0.0f}},
    /* Switching to row layout makes it fill the whole height instead */
    {"row layout, ignore XY size",
        Flag::IgnoreNodeSize, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f},
        [](YGNodeRef parent, YGNodeRef) {
            CORRADE_VERIFY(YGNodeStyleGetFlexDirection(parent) == YGFlexDirectionColumn);
            YGNodeStyleSetFlexDirection(parent, YGFlexDirectionRow);
        }, {15.0f, 5.0f}, {0.0f, 50.0f}},
    /* Swapping direction and ignoring XY offset makes the layout fill from
       bottom / right */
    {"reverse column layout, ignore Y offset",
        Flag::IgnoreNodeOffsetY, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f},
        [](YGNodeRef parent, YGNodeRef) {
            CORRADE_VERIFY(YGNodeStyleGetFlexDirection(parent) == YGFlexDirectionColumn);
            YGNodeStyleSetFlexDirection(parent, YGFlexDirectionColumnReverse);
        }, {15.0f, 50.0f - 35.0f}, {25.0f, 35.0f}},
    {"reverse row layout, ignore X offset",
        Flag::IgnoreNodeOffsetX, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f},
        [](YGNodeRef parent, YGNodeRef) {
            CORRADE_VERIFY(YGNodeStyleGetFlexDirection(parent) == YGFlexDirectionColumn);
            YGNodeStyleSetFlexDirection(parent, YGFlexDirectionRowReverse);
        }, {200.0f - 25.0f, 5.0f}, {25.0f, 35.0f}},
    /* Properties being set directly shouldn't get overwritten when they're
       meant to ignored */
    {"ignore XY offset, set directly instead",
        Flag::IgnoreNodeOffset, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f},
        [](YGNodeRef, YGNodeRef child) {
            YGNodeStyleSetPosition(child, YGEdgeTop, 13.5f);
            YGNodeStyleSetPosition(child, YGEdgeLeft, 3.5f);
        }, {3.5f, 13.5f}, {25.0f, 35.0f}},
    {"ignore XY size, set directly instead",
        Flag::IgnoreNodeSize, {}, {},
        {15.0f, 5.0f}, {25.0f, 35.0f},
        [](YGNodeRef, YGNodeRef child) {
            YGNodeStyleSetWidth(child, 13.5f);
            YGNodeStyleSetHeight(child, 3.5f);
        }, {15.0f, 5.0f}, {13.5f, 3.5f}},
    {"ignore X offset, absolute, set from the right edge",
        Flag::IgnoreNodeOffsetX, {}, NodeOffsetType::Absolute,
        {15.0f, 5.0f}, {25.0f, 35.0f},
        [](YGNodeRef, YGNodeRef child) {
            YGNodeStyleSetPosition(child, YGEdgeRight, 3.5f);
        }, {200.0f - 25.0f - 3.5f, 5.0f}, {25.0f, 35.0f}},
    /* Behavior with offset / size left zero. It should make Yoga use some
       "reasonable default" instead of forcing a zero-sized rect at origin. */
    {"auto offset", {}, {}, {},
        {}, {25.0f, 35.0f}, {},
        {}, {25.0f, 35.0f}},
    {"auto offset, reverse column direction",
        {}, FlexDirection::ColumnReverse, {},
        {}, {25.0f, 35.0f}, {},
        {0.0f, 15.0f}, {25.0f, 35.0f}},
    {"auto offset, reverse row direction",
        {}, FlexDirection::RowReverse, {},
        {}, {25.0f, 35.0f}, {},
        {175.0f, 0.0f}, {25.0f, 35.0f}},
    {"auto offset, RTL direction", {}, {}, {},
        {}, {25.0f, 35.0f},
        [](YGNodeRef parent, YGNodeRef) {
            CORRADE_VERIFY(YGNodeStyleGetDirection(parent) == YGDirectionInherit);
            YGNodeStyleSetDirection(parent, YGDirectionRTL);
        }, {175.0f, 0.0f}, {25.0f, 35.0f}},
    {"auto offset from right instead of left, absolute",
        Flag::NodeOffsetFromRight, {}, NodeOffsetType::Absolute,
        {0.0f, 5.0f}, {25.0f, 35.0f}, {},
        /* Width is 200, so this is 200 - 25 */
        {175.0f, 5.0f}, {25.0f, 35.0f}},
    {"auto offset from bottom instead of top, absolute",
        Flag::NodeOffsetFromBottom, {}, NodeOffsetType::Absolute,
        {15.0f, 0.0f}, {25.0f, 35.0f}, {},
        /* Height is 50, so this is 50 - 35 */
        {15.0f, 15.0f}, {25.0f, 35.0f}},
    {"auto offset from bottom right instead of top left, absolute",
        Flag::NodeOffsetFromRightBottom, {}, NodeOffsetType::Absolute,
        {}, {25.0f, 35.0f}, {},
        /* Size is {200, 50}, so this is {200, 50} - {25, 35} */
        {175.0f, 15.0f}, {25.0f, 35.0f}},
    {"auto size", {}, {}, {},
        {15.0f, 5.0f}, {}, {},
        {15.0f, 5.0f}, {200.0f, 0.0f}},
    {"auto size, row direction",
        {}, FlexDirection::Row, {},
        {15.0f, 5.0f}, {}, {},
        {15.0f, 5.0f}, {0.0f, 50.0f}},
};

const struct {
    const char* name;
    Containers::Optional<Vector2> updateOffset, updateSize;
    Vector2 expectedOffset, expectedSize;
} UpdateModifyNodeOffsetSizeData[]{
    {"different non-zero offset",
        {{30.0f, 20.0f}}, {},
        {30.0f, 20.0f}, {40.0f, 50.0f}},
    {"zero X offset",
        {{0.0f, 30.0f}}, {},
        {0.0f, 30.0f}, {40.0f, 50.0f}},
    {"zero Y offset",
        {{20.0f, 0.0f}}, {},
        {20.0f, 0.0f}, {40.0f, 50.0f}},
    {"different non-zero size",
        {}, {{50.0f, 40.0f}},
        {20.0f, 30.0f}, {50.0f, 40.0f}},
    {"zero X size",
        {}, {{0.0f, 50.0f}},
        /* The default flex direction is column, so X size expands to the full
           parent width */
        {20.0f, 30.0f}, {100.0f, 50.0f}},
    {"zero Y size",
        {}, {{40.0f, 0.0f}},
        /* The default flex direction is column, so Y size shrinks to the
           minimum that covers all child nodes */
        {20.0f, 30.0f}, {40.0f, 25.0f}},
};

const struct {
    const char* name;
    bool explicitDisplayNone, xfail;
} UpdateModifyNodeVisiblityData[]{
    {"", false, true},
    {"explicit display none", true, false}
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    bool layouterName;
    Flags flags;
    Containers::Optional<FlexDirection> flexDirection;
    Containers::Optional<NodeOffsetType> nodeOffsetType;
    const char* expected;
} DebugIntegrationData[]{
    {"defaults", false,
        {}, {}, {},
        "Node {0x1, 0x1}\n"
        "  Layout {0x6, 0x2} from layouter {0x0, 0x3}\n"
        "    Flex direction: Column"},
    {"defaults, layouter name", true,
        {}, {}, {},
        "Node {0x1, 0x1}\n"
        "  Layout {0x6, 0x2} from layouter {0x0, 0x3} WebScaleTech!!\n"
        "    Flex direction: Column"},
    {"flags", false,
        Flag::NodeOffsetFromRight|Flag::PercentageNodeOffsetY|Flag::IgnoreNodeSize, {}, {},
        "Node {0x1, 0x1}\n"
        "  Layout {0x6, 0x2} from layouter {0x0, 0x3}\n"
        "    Flags: NodeOffsetFromRight|PercentageNodeOffsetY|IgnoreNodeSize\n"
        "    Flex direction: Column"},
    {"different flex direction", false,
        {}, FlexDirection::RowReverse, {},
        "Node {0x1, 0x1}\n"
        "  Layout {0x6, 0x2} from layouter {0x0, 0x3}\n"
        "    Flex direction: RowReverse"},
    {"absolute node offset type", false,
        {}, {}, NodeOffsetType::Absolute,
        "Node {0x1, 0x1}\n"
        "  Layout {0x6, 0x2} from layouter {0x0, 0x3}\n"
        "    Flex direction: Column\n"
        "    Node offset type: Absolute"},
    {"flags, different flex direction, absolute node offset type", true,
        Flag::NodeOffsetFromRight|Flag::PercentageNodeOffsetY|Flag::IgnoreNodeSize, FlexDirection::ColumnReverse, NodeOffsetType::Absolute,
        "Node {0x1, 0x1}\n"
        "  Layout {0x6, 0x2} from layouter {0x0, 0x3} WebScaleTech!!\n"
        "    Flags: NodeOffsetFromRight|PercentageNodeOffsetY|IgnoreNodeSize\n"
        "    Flex direction: ColumnReverse\n"
        "    Node offset type: Absolute"},
    /* The last case here is used in debugIntegrationNoCallback() to verify
       output w/o a callback and for visual color verification, it's expected
       to be the most complete, executing all coloring code paths */
};

LayouterTest::LayouterTest() {
    addTests({&LayouterTest::debugFlag,
              &LayouterTest::debugFlagPacked,
              &LayouterTest::debugFlags,
              &LayouterTest::debugFlagsPacked,
              &LayouterTest::debugFlagsSupersets,
              &LayouterTest::debugFlexDirection,
              &LayouterTest::debugFlexDirectionPacked,
              &LayouterTest::debugNodeOffsetType,
              &LayouterTest::debugNodeOffsetTypePacked,

              &LayouterTest::construct,
              &LayouterTest::constructCopy,
              &LayouterTest::constructMove,

              &LayouterTest::addRemove,
              &LayouterTest::addRemoveHandleRecycle,
              &LayouterTest::addRemoveInvalid,

              &LayouterTest::flexDirection,
              &LayouterTest::nodeOffsetType,

              &LayouterTest::invalidHandle,

              &LayouterTest::clean,
              &LayouterTest::updateEmpty});

    addInstancedTests({&LayouterTest::updateDataOrder},
        Containers::arraySize(UpdateDataOrderData));

    addInstancedTests({&LayouterTest::updateFlags},
        Containers::arraySize(UpdateFlagsData));

    addInstancedTests({&LayouterTest::updateModifyNodeOffsetSize},
        Containers::arraySize(UpdateModifyNodeOffsetSizeData));

    addInstancedTests({&LayouterTest::updateModifyNodeVisibility},
        Containers::arraySize(UpdateModifyNodeVisiblityData));

    addTests({&LayouterTest::updateModifyLayoutFlexDirection,
              &LayouterTest::updateModifyLayoutNodeOffsetType,
              &LayouterTest::updateModifyYogaConfigDirectly,
              &LayouterTest::updateModifyYogaNodeDirectly});

    addInstancedTests({&LayouterTest::debugIntegration},
        Containers::arraySize(DebugIntegrationData));

    addTests({&LayouterTest::debugIntegrationNoCallback});
}

void LayouterTest::debugFlag() {
    Containers::String out;
    Debug{&out} << Flag::IgnoreNodeSize << Flag(0xbeef);
    CORRADE_COMPARE(out, "YogaIntegration::Flag::IgnoreNodeSize YogaIntegration::Flag(0xbeef)\n");
}

void LayouterTest::debugFlagPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << Flag::IgnoreNodeSizeY << Debug::packed << Flag(0xbeef) << Flag::IgnoreNodeOffsetX;
    CORRADE_COMPARE(out, "IgnoreNodeSizeY 0xbeef YogaIntegration::Flag::IgnoreNodeOffsetX\n");
}

void LayouterTest::debugFlags() {
    Containers::String out;
    Debug{&out} << (Flag::IgnoreNodeOffsetX|Flag::PercentageNodeSizeX|Flag(0x8000)) << Flags{};
    CORRADE_COMPARE(out, "YogaIntegration::Flag::IgnoreNodeOffsetX|YogaIntegration::Flag::PercentageNodeSizeX|YogaIntegration::Flag(0x8000) YogaIntegration::Flags{}\n");
}

void LayouterTest::debugFlagsPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << (Flag::PercentageNodeOffsetY|Flag::IgnoreNodeSize|Flag(0x8000)) << Debug::packed << Flags{} << (Flag::IgnoreNodeOffsetY|Flag::PercentageNodeSizeX);
    CORRADE_COMPARE(out, "PercentageNodeOffsetY|IgnoreNodeSize|0x8000 {} YogaIntegration::Flag::IgnoreNodeOffsetY|YogaIntegration::Flag::PercentageNodeSizeX\n");
}

void LayouterTest::debugFlagsSupersets() {
    /* PercentageNodeOffset is a superset of PercentageNodeOffsetX/Y, so only
       one should be printed */
    {
        Containers::String out;
        Debug{&out} << (Flag::PercentageNodeOffset|Flag::PercentageNodeOffsetX|Flag::PercentageNodeOffsetY);
        CORRADE_COMPARE(out, "YogaIntegration::Flag::PercentageNodeOffset\n");

    /* IgnoreNodeOffset is a superset of IgnoreNodeOffsetX/Y, so only one
       should be printed */
    } {
        Containers::String out;
        Debug{&out} << (Flag::IgnoreNodeOffset|Flag::IgnoreNodeOffsetX|Flag::IgnoreNodeOffsetY);
        CORRADE_COMPARE(out, "YogaIntegration::Flag::IgnoreNodeOffset\n");

    /* IgnoreNodeOffset is also a superset of PercentageNodeOffset, so only one
       should be printed */
    } {
        Containers::String out;
        Debug{&out} << (Flag::IgnoreNodeOffset|Flag::PercentageNodeOffset);
        CORRADE_COMPARE(out, "YogaIntegration::Flag::IgnoreNodeOffset\n");

    /* PercentageNodeSize is a superset of PercentageNodeSizeX/Y, so only one
       should be printed */
    } {
        Containers::String out;
        Debug{&out} << (Flag::PercentageNodeSize|Flag::PercentageNodeSizeX|Flag::PercentageNodeSizeY);
        CORRADE_COMPARE(out, "YogaIntegration::Flag::PercentageNodeSize\n");

    /* IgnoreNodeSize is a superset of IgnoreNodeSizeX/Y, so only one should be
       printed */
    } {
        Containers::String out;
        Debug{&out} << (Flag::IgnoreNodeSize|Flag::IgnoreNodeSizeX|Flag::IgnoreNodeSizeY);
        CORRADE_COMPARE(out, "YogaIntegration::Flag::IgnoreNodeSize\n");

    /* IgnoreNodeSize is also a superset of PercentageNodeSize, so only one
       should be printed */
    } {
        Containers::String out;
        Debug{&out} << (Flag::IgnoreNodeSize|Flag::PercentageNodeSize);
        CORRADE_COMPARE(out, "YogaIntegration::Flag::IgnoreNodeSize\n");

    /* NodeOffsetFromRightBottom is a superset of NodeOffsetFromRight/Bottom,
       so only one should be printed */
    } {
        Containers::String out;
        Debug{&out} << (Flag::NodeOffsetFromRightBottom|Flag::NodeOffsetFromRight|Flag::NodeOffsetFromBottom);
        CORRADE_COMPARE(out, "YogaIntegration::Flag::NodeOffsetFromRightBottom\n");

    /* IgnoreNodeOffsetX is a superset of NodeOffsetFromRight, so only one
       should be printed */
    } {
        Containers::String out;
        Debug{&out} << (Flag::NodeOffsetFromRight|Flag::IgnoreNodeOffsetX);
        CORRADE_COMPARE(out, "YogaIntegration::Flag::IgnoreNodeOffsetX\n");

    /* IgnoreNodeOffsetY is a superset of NodeOffsetFromBottom, so only one
       should be printed */
    } {
        Containers::String out;
        Debug{&out} << (Flag::NodeOffsetFromBottom|Flag::IgnoreNodeOffsetY);
        CORRADE_COMPARE(out, "YogaIntegration::Flag::IgnoreNodeOffsetY\n");
    }
}

void LayouterTest::debugFlexDirection() {
    Containers::String out;
    Debug{&out} << FlexDirection::ColumnReverse << FlexDirection(0xdeadbeef);
    CORRADE_COMPARE(out, "YogaIntegration::FlexDirection::ColumnReverse YogaIntegration::FlexDirection(0xdeadbeef)\n");
}

void LayouterTest::debugFlexDirectionPacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << FlexDirection::RowReverse << Debug::packed << FlexDirection(0xdeadbeef) << FlexDirection::Column;
    CORRADE_COMPARE(out, "RowReverse 0xdeadbeef YogaIntegration::FlexDirection::Column\n");
}

void LayouterTest::debugNodeOffsetType() {
    Containers::String out;
    Debug{&out} << NodeOffsetType::Relative << NodeOffsetType(0xdeadbeef);
    CORRADE_COMPARE(out, "YogaIntegration::NodeOffsetType::Relative YogaIntegration::NodeOffsetType(0xdeadbeef)\n");
}

void LayouterTest::debugNodeOffsetTypePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug{&out} << Debug::packed << NodeOffsetType::Absolute << Debug::packed << NodeOffsetType(0xdeadbeef) << NodeOffsetType::Relative;
    CORRADE_COMPARE(out, "Absolute 0xdeadbeef YogaIntegration::NodeOffsetType::Relative\n");
}

void LayouterTest::construct() {
    Layouter layouter{Ui::layouterHandle(0xab, 0x12)};
    const Layouter& clayouter = layouter;
    CORRADE_COMPARE(layouter.handle(), Ui::layouterHandle(0xab, 0x12));
    CORRADE_VERIFY(layouter.yogaConfig());
    /* Const overloads should return the same Yoga config reference */
    CORRADE_COMPARE(clayouter.yogaConfig(), layouter.yogaConfig());
    CORRADE_COMPARE(YGConfigGetPointScaleFactor(layouter.yogaConfig()), 0.0f);
}

void LayouterTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<Layouter>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Layouter>{});
}

void LayouterTest::constructMove() {
    Layouter a{Ui::layouterHandle(0xab, 0x12)};
    YGConfigSetPointScaleFactor(a.yogaConfig(), 1.25f);

    Layouter b{Utility::move(a)};
    CORRADE_COMPARE(b.handle(), Ui::layouterHandle(0xab, 0x12));
    CORRADE_VERIFY(b.yogaConfig());
    CORRADE_COMPARE(YGConfigGetPointScaleFactor(b.yogaConfig()), 1.250f);

    Layouter c{Ui::layouterHandle(3, 5)};
    c = Utility::move(b);
    CORRADE_COMPARE(c.handle(), Ui::layouterHandle(0xab, 0x12));
    CORRADE_VERIFY(c.yogaConfig());
    CORRADE_COMPARE(YGConfigGetPointScaleFactor(c.yogaConfig()), 1.250f);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Layouter>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Layouter>::value);
}

void LayouterTest::addRemove() {
    Ui::AbstractUserInterface ui{{100, 100}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));
    const Layouter& clayouter = layouter;

    /* Create a bunch of nodes to have a non-trivial handle */
    ui.createNode({}, {});
    ui.createNode({}, {});
    ui.removeNode(ui.createNode({}, {}));
    Ui::NodeHandle node = ui.createNode({}, {});

    /* Add a root node layout */
    Ui::LayoutHandle layout = layouter.add(node);
    CORRADE_COMPARE(layouter.usedCount(), 1);
    CORRADE_VERIFY(layouter.yogaNode(layout));
    /* Const overloads should return the same Yoga node reference */
    CORRADE_COMPARE(clayouter.yogaNode(layout), layouter.yogaNode(layout));
    CORRADE_COMPARE(layouter.flags(layout), Flags{});
    CORRADE_COMPARE(layouter.flexDirection(layout), FlexDirection::Column);
    CORRADE_COMPARE(layouter.nodeOffsetType(layout), NodeOffsetType::Relative);
    /* The config used for this node should be the global one */
    CORRADE_COMPARE(YGNodeGetConfig(layouter.yogaNode(layout)), layouter.yogaConfig());
    /* No parent, no children */
    CORRADE_COMPARE(YGNodeGetParent(layouter.yogaNode(layout)), nullptr);
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(layout)), 0);
    /* The layout is accessible also from the UI itself */
    CORRADE_COMPARE(ui.nodeUniqueLayout(node, layouter), layoutHandleData(layout));

    /* Child node with flags */
    Ui::NodeHandle child1 = ui.createNode(node, {}, {});
    Ui::LayoutHandle childLayout1 = layouter.add(child1, Flag::IgnoreNodeOffset);
    CORRADE_COMPARE(layouter.usedCount(), 2);
    CORRADE_COMPARE(layouter.flags(childLayout1), Flag::IgnoreNodeOffset);
    /* The config used should again be the global one */
    CORRADE_COMPARE(YGNodeGetConfig(layouter.yogaNode(layout)), layouter.yogaConfig());
    /* Parent is now specified, and the parent has the first child which is
       this one */
    CORRADE_COMPARE(YGNodeGetParent(layouter.yogaNode(childLayout1)), layouter.yogaNode(layout));
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(layout)), 1);
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 0), layouter.yogaNode(childLayout1));
    CORRADE_COMPARE(ui.nodeUniqueLayout(child1, layouter), layoutHandleData(childLayout1));

    /* Adding a layout to a node that has a parent node but no parent layout
       should have no parent Yoga node but still be accessible through the UI
       itself */
    Ui::NodeHandle another = ui.createNode({}, {});
    Ui::NodeHandle anotherChild = ui.createNode(another, {}, {});
    Ui::LayoutHandle anotherChildLayout = layouter.add(anotherChild);
    CORRADE_COMPARE(layouter.usedCount(), 3);
    CORRADE_COMPARE(YGNodeGetParent(layouter.yogaNode(anotherChildLayout)), nullptr);
    CORRADE_COMPARE(ui.nodeUniqueLayout(anotherChild, layouter), layoutHandleData(anotherChildLayout));

    /* Another child node, inserted before the first, with other flags */
    Ui::NodeHandle child2 = ui.createNode(node, {}, {});
    Ui::LayoutHandle childLayout2 = layouter.add(child2, childLayout1, Flag::IgnoreNodeSizeX);
    CORRADE_COMPARE(layouter.usedCount(), 4);
    CORRADE_COMPARE(layouter.flags(childLayout2), Flag::IgnoreNodeSizeX);
    CORRADE_COMPARE(YGNodeGetParent(layouter.yogaNode(childLayout2)), layouter.yogaNode(layout));
    /* There are two children now and this one is first */
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(layout)), 2);
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 0), layouter.yogaNode(childLayout2));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 1), layouter.yogaNode(childLayout1));

    /* Yet another child node, inserted into the middle, LayouterDataHandle
       overloads */
    Ui::NodeHandle child3 = ui.createNode(node, {}, {});
    Ui::LayoutHandle childLayout3 = layouter.add(child3, layoutHandleData(childLayout1), Flag::PercentageNodeOffsetY);
    CORRADE_COMPARE(layouter.usedCount(), 5);
    /* The LayouterDataHandle overload, and the const variant of it, should
       return the same yoga node as the main one. Can't really verify much
       else. */
    CORRADE_COMPARE(layouter.yogaNode(layoutHandleData(childLayout3)), layouter.yogaNode(childLayout3));
    CORRADE_COMPARE(clayouter.yogaNode(layoutHandleData(childLayout3)), layouter.yogaNode(childLayout3));
    CORRADE_COMPARE(layouter.flags(layoutHandleData(childLayout3)), Flag::PercentageNodeOffsetY);
    CORRADE_COMPARE(layouter.flexDirection(layoutHandleData(childLayout3)), FlexDirection::Column);
    CORRADE_COMPARE(layouter.nodeOffsetType(layoutHandleData(childLayout3)), NodeOffsetType::Relative);
    /* There are three children now */
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(layoutHandleData(layout))), 3);
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layoutHandleData(layout)), 0), layouter.yogaNode(childLayout2));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layoutHandleData(layout)), 1), layouter.yogaNode(childLayout3));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layoutHandleData(layout)), 2), layouter.yogaNode(childLayout1));

    /* Overload with flags but no before */
    Ui::NodeHandle child4 = ui.createNode(node, {}, {});
    Ui::LayoutHandle childLayout4 = layouter.add(child4, Flag::IgnoreNodeOffset|Flag::IgnoreNodeSize);
    CORRADE_COMPARE(layouter.usedCount(), 6);
    CORRADE_COMPARE(layouter.flags(childLayout4), Flag::IgnoreNodeOffset|Flag::IgnoreNodeSize);
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(layout)), 4);
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 0), layouter.yogaNode(childLayout2));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 1), layouter.yogaNode(childLayout3));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 2), layouter.yogaNode(childLayout1));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 3), layouter.yogaNode(childLayout4));

    /* Overload with a before layout but no flags, LayouterDataHandle */
    Ui::NodeHandle child5 = ui.createNode(node, {}, {});
    Ui::LayoutHandle childLayout5 = layouter.add(child5, layoutHandleData(childLayout1));
    CORRADE_COMPARE(layouter.usedCount(), 7);
    CORRADE_COMPARE(layouter.flags(childLayout5), Flags{});
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(layout)), 5);
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 0), layouter.yogaNode(childLayout2));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 1), layouter.yogaNode(childLayout3));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 2), layouter.yogaNode(childLayout5));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 3), layouter.yogaNode(childLayout1));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 4), layouter.yogaNode(childLayout4));

    /* Explicitly passing a Null adds at the end, both overloads */
    Ui::NodeHandle child6 = ui.createNode(node, {}, {});
    Ui::NodeHandle child7 = ui.createNode(node, {}, {});
    Ui::LayoutHandle childLayout6 = layouter.add(child6, Ui::LayoutHandle::Null);
    Ui::LayoutHandle childLayout7 = layouter.add(child7, Ui::LayouterDataHandle::Null);
    CORRADE_COMPARE(layouter.usedCount(), 9);
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(layout)), 7);
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 0), layouter.yogaNode(childLayout2));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 1), layouter.yogaNode(childLayout3));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 2), layouter.yogaNode(childLayout5));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 3), layouter.yogaNode(childLayout1));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 4), layouter.yogaNode(childLayout4));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 5), layouter.yogaNode(childLayout6));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 6), layouter.yogaNode(childLayout7));

    /* Removing a layout clears it from the parent child list, and it's also
       no longer available from the UI. Test both overloads. */
    layouter.remove(childLayout5);
    layouter.remove(layoutHandleData(childLayout4));
    CORRADE_COMPARE(layouter.usedCount(), 7);
    CORRADE_VERIFY(!layouter.isHandleValid(childLayout5));
    CORRADE_VERIFY(!layouter.isHandleValid(childLayout4));
    CORRADE_COMPARE(ui.nodeUniqueLayout(child4, layouter), Ui::LayouterDataHandle::Null);
    CORRADE_COMPARE(ui.nodeUniqueLayout(child5, layouter), Ui::LayouterDataHandle::Null);
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(layout)), 5);
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 0), layouter.yogaNode(childLayout2));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 1), layouter.yogaNode(childLayout3));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 2), layouter.yogaNode(childLayout1));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 3), layouter.yogaNode(childLayout6));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(layout), 4), layouter.yogaNode(childLayout7));

    /* Removing a layout that has no parent works as well */
    layouter.remove(anotherChildLayout);
    CORRADE_COMPARE(layouter.usedCount(), 6);
    CORRADE_VERIFY(!layouter.isHandleValid(anotherChildLayout));
    CORRADE_COMPARE(ui.nodeUniqueLayout(anotherChild, layouter), Ui::LayouterDataHandle::Null);
}

void LayouterTest::addRemoveHandleRecycle() {
    Ui::AbstractUserInterface ui{{100, 100}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    /* Just so we don't operate with the first ever layout */
    Ui::NodeHandle parent = ui.createNode({}, {});
    Ui::LayoutHandle parentLayout = layouter.add(parent);

    Ui::NodeHandle node = ui.createNode(parent, {}, {});
    Ui::LayoutHandle layout = layouter.add(node, Flag::IgnoreNodeOffset|Flag::PercentageNodeSize);
    /* Set also some custom properties to verify they're reset too, i.e. that
       it indeed isn't the same Yoga node reused */
    YGNodeStyleSetBorder(layouter.yogaNode(layout), YGEdgeAll, 13.37f);

    /* Layout that reuses a previous slot should have the previous Yoga node
       freed and all properties reset */
    layouter.remove(layout);
    Ui::LayoutHandle layout2 = layouter.add(node, Flag::PercentageNodeOffsetX);
    CORRADE_COMPARE(layoutHandleId(layout2), layoutHandleId(layout));
    CORRADE_COMPARE(layouter.flags(layout2), Flag::PercentageNodeOffsetX);
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(parentLayout)), 1);
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(parentLayout), 0), layouter.yogaNode(layout2));
    /* By default it's an "undefined" value, which is a NaN */
    CORRADE_COMPARE(YGNodeStyleGetBorder(layouter.yogaNode(layout2), YGEdgeAll), YGUndefined);
}

void LayouterTest::addRemoveInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Ui::AbstractUserInterface ui{{100, 100}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy. Create some more layouters before to have a non-trivial
       handle. */
    ui.createLayouter();
    ui.createLayouter();
    ui.removeLayouter(ui.createLayouter());
    ui.removeLayouter(ui.createLayouter());
    ui.removeLayouter(ui.createLayouter());
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    Ui::NodeHandle parentWithNoLayout = ui.createNode({}, {});

    /* Two parent nodes to test wrong parenting. Creating some extra nodes to
       have a non-trivial parent handles for both. */
    ui.createNode({}, {});
    ui.removeNode(ui.createNode({}, {}));
    ui.removeNode(ui.createNode({}, {}));
    Ui::NodeHandle parent1 = ui.createNode({}, {});
    Ui::LayoutHandle parentLayout1 = layouter.add(parent1);
    ui.createNode({}, {});
    ui.createNode({}, {});
    ui.createNode({}, {});
    ui.removeNode(ui.createNode({}, {}));
    Ui::NodeHandle parent2 = ui.createNode({}, {});
    Ui::LayoutHandle parentLayout2 = layouter.add(parent2);
    Ui::LayoutHandle childLayout1 = layouter.add(ui.createNode(parent1, {}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.remove(layouter.add(ui.createNode({}, {})));
    Ui::LayoutHandle childLayout2 = layouter.add(ui.createNode(parent2, {}, {}));
    /* Extra child layouts to verifying removing a parent with children */
    layouter.add(ui.createNode(parent1, {}, {}));
    layouter.add(ui.createNode(parent2, {}, {}));
    layouter.add(ui.createNode(parent2, {}, {}));
    layouter.add(ui.createNode(parent2, {}, {}));

    layouter.remove(layouter.add(ui.createNode({}, {})));
    layouter.remove(layouter.add(ui.createNode({}, {})));
    Ui::LayoutHandle layout = layouter.add(ui.createNode({}, {}));

    /* Using a new node for every call to avoid accidentally hitting the
       AbstractLayouter assert about unique layouts. Calling add() with a null
       before handle should be fine. */
    layouter.add(ui.createNode({}, {}), Ui::LayoutHandle::Null);
    layouter.add(ui.createNode({}, {}), Ui::LayouterDataHandle::Null);

    Containers::String out;
    Error redirectError{&out};
    /* Invalid before handle */
    layouter.add(ui.createNode({}, {}), Ui::layoutHandle(layouter.handle(), 0xc0ffe, 0x123));
    layouter.add(ui.createNode({}, {}), Ui::layouterDataHandle(0xc0ffe, 0x123));
    /* The `before` handle non-null but a root node */
    layouter.add(ui.createNode({}, {}), layout);
    layouter.add(ui.createNode({}, {}), layoutHandleData(layout));
    /* The `before` handle non-null and a node w/o parent layout */
    layouter.add(ui.createNode(parentWithNoLayout, {}, {}), layout);
    layouter.add(ui.createNode(parentWithNoLayout, {}, {}), layoutHandleData(layout));
    /* The `before` handle not having a common parent */
    layouter.add(ui.createNode(parent2, {}, {}), childLayout1);
    layouter.add(ui.createNode(parent1, {}, {}), layoutHandleData(childLayout2));
    /* Invalid node is checked in AbstractLayouter::add() already, so not
       repeating the assert here */
    /* Removing layouts with children */
    layouter.remove(parentLayout1);
    layouter.remove(Ui::layoutHandleData(parentLayout2));
    /* There's an assertion in AbstractLayouter::remove() as well, but we need
       to check for layout children first and so the assert has to be
       earlier */
    layouter.remove(Ui::LayoutHandle::Null);
    layouter.remove(Ui::LayouterDataHandle::Null);
    CORRADE_COMPARE_AS(out,
        "YogaIntegration::Layouter::add(): invalid before handle Ui::LayoutHandle({0x2, 0x4}, {0xc0ffe, 0x123})\n"
        "YogaIntegration::Layouter::add(): invalid before handle Ui::LayouterDataHandle(0xc0ffe, 0x123)\n"
        /* These two print the ~same message because the assert is done in the
           internal helper that unpacks LayoutHandle to LayouterDataHandle */
        "YogaIntegration::Layouter::add(): expected before to be null for Ui::NodeHandle(0x17, 0x1) without a parent layout but got Ui::LayouterDataHandle(0xa, 0x3)\n"
        "YogaIntegration::Layouter::add(): expected before to be null for Ui::NodeHandle(0x18, 0x1) without a parent layout but got Ui::LayouterDataHandle(0xa, 0x3)\n"
        /* Same in this case */
        "YogaIntegration::Layouter::add(): expected before to be null for Ui::NodeHandle(0x19, 0x1) without a parent layout but got Ui::LayouterDataHandle(0xa, 0x3)\n"
        "YogaIntegration::Layouter::add(): expected before to be null for Ui::NodeHandle(0x1a, 0x1) without a parent layout but got Ui::LayouterDataHandle(0xa, 0x3)\n"
        /* Here as well */
        "YogaIntegration::Layouter::add(): expected before to be a child of Ui::NodeHandle(0x6, 0x2) but Ui::LayouterDataHandle(0x2, 0x1) is a child of Ui::NodeHandle(0x2, 0x3)\n"
        "YogaIntegration::Layouter::add(): expected before to be a child of Ui::NodeHandle(0x2, 0x3) but Ui::LayouterDataHandle(0x5, 0x2) is a child of Ui::NodeHandle(0x6, 0x2)\n"
        "YogaIntegration::Layouter::remove(): cannot remove Ui::LayoutHandle({0x2, 0x4}, {0x0, 0x1}) with 2 children\n"
        "YogaIntegration::Layouter::remove(): cannot remove Ui::LayouterDataHandle(0x1, 0x1) with 4 children\n"
        "YogaIntegration::Layouter::remove(): invalid handle Ui::LayoutHandle::Null\n"
        "YogaIntegration::Layouter::remove(): invalid handle Ui::LayouterDataHandle::Null\n",
        TestSuite::Compare::String);
}

void LayouterTest::flexDirection() {
    Ui::AbstractUserInterface ui{{100, 100}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    /* Add some more layouts to verify it doesn't always pick the first. By
       default there's a column direction. */
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    Ui::LayoutHandle layout = layouter.add(ui.createNode({}, {}));
    CORRADE_COMPARE(layouter.flexDirection(layout), FlexDirection::Column);

    /* Clear the state flags */
    ui.update();
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});

    /* Updating a flex direction sets a state flag */
    layouter.setFlexDirection(layout, FlexDirection::RowReverse);
    CORRADE_COMPARE(layouter.flexDirection(layout), FlexDirection::RowReverse);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterState::NeedsUpdate);

    /* Clear the state flags */
    ui.update();
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});

    /* LayouterDataHandle overloads */
    layouter.setFlexDirection(layoutHandleData(layout), FlexDirection::ColumnReverse);
    CORRADE_COMPARE(layouter.flexDirection(layoutHandleData(layout)), FlexDirection::ColumnReverse);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterState::NeedsUpdate);

    /* Clear the state flags */
    ui.update();
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});

    /* Setting the same flex direction sets a state flag as well. We don't
       bother, Yoga likely caches something, or maybe it doesn't. */
    CORRADE_COMPARE(layouter.flexDirection(layout), FlexDirection::ColumnReverse);
    layouter.setFlexDirection(layout, FlexDirection::ColumnReverse);
    CORRADE_COMPARE(layouter.flexDirection(layout), FlexDirection::ColumnReverse);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterState::NeedsUpdate);
}

void LayouterTest::nodeOffsetType() {
    Ui::AbstractUserInterface ui{{100, 100}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    /* Add some more layouts to verify it doesn't always pick the first. By
       default there's a relative type. */
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    Ui::LayoutHandle layout = layouter.add(ui.createNode({}, {}));
    CORRADE_COMPARE(layouter.nodeOffsetType(layout), NodeOffsetType::Relative);

    /* Clear the state flags */
    ui.update();
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});

    /* Updating a type sets a state flag */
    layouter.setNodeOffsetType(layout, NodeOffsetType::Absolute);
    CORRADE_COMPARE(layouter.nodeOffsetType(layout), NodeOffsetType::Absolute);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterState::NeedsUpdate);

    /* Clear the state flags */
    ui.update();
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});

    /* LayouterDataHandle overloads */
    layouter.setNodeOffsetType(layoutHandleData(layout), NodeOffsetType::Relative);
    CORRADE_COMPARE(layouter.nodeOffsetType(layoutHandleData(layout)), NodeOffsetType::Relative);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterState::NeedsUpdate);

    /* Clear the state flags */
    ui.update();
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});

    /* Setting the same type sets a state flag as well. We don't bother, Yoga
       likely caches something, or maybe it doesn't. */
    CORRADE_COMPARE(layouter.nodeOffsetType(layout), NodeOffsetType::Relative);
    layouter.setNodeOffsetType(layout, NodeOffsetType::Relative);
    CORRADE_COMPARE(layouter.nodeOffsetType(layout), NodeOffsetType::Relative);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterState::NeedsUpdate);
}

void LayouterTest::invalidHandle() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Layouter layouter{Ui::layouterHandle(0, 1)};
    const Layouter& clayouter = layouter;

    Containers::String out;
    Error redirectError{&out};
    layouter.yogaNode(Ui::LayoutHandle::Null);
    clayouter.yogaNode(Ui::LayoutHandle::Null);
    layouter.yogaNode(Ui::LayouterDataHandle::Null);
    clayouter.yogaNode(Ui::LayouterDataHandle::Null);
    layouter.flags(Ui::LayoutHandle::Null);
    layouter.flags(Ui::LayouterDataHandle::Null);
    layouter.flexDirection(Ui::LayoutHandle::Null);
    layouter.flexDirection(Ui::LayouterDataHandle::Null);
    layouter.setFlexDirection(Ui::LayoutHandle::Null, {});
    layouter.setFlexDirection(Ui::LayouterDataHandle::Null, {});
    layouter.nodeOffsetType(Ui::LayoutHandle::Null);
    layouter.nodeOffsetType(Ui::LayouterDataHandle::Null);
    layouter.setNodeOffsetType(Ui::LayoutHandle::Null, {});
    layouter.setNodeOffsetType(Ui::LayouterDataHandle::Null, {});
    CORRADE_COMPARE_AS(out,
        "YogaIntegration::Layouter::yogaNode(): invalid handle Ui::LayoutHandle::Null\n"
        "YogaIntegration::Layouter::yogaNode(): invalid handle Ui::LayoutHandle::Null\n"
        "YogaIntegration::Layouter::yogaNode(): invalid handle Ui::LayouterDataHandle::Null\n"
        "YogaIntegration::Layouter::yogaNode(): invalid handle Ui::LayouterDataHandle::Null\n"
        "YogaIntegration::Layouter::flags(): invalid handle Ui::LayoutHandle::Null\n"
        "YogaIntegration::Layouter::flags(): invalid handle Ui::LayouterDataHandle::Null\n"
        "YogaIntegration::Layouter::flexDirection(): invalid handle Ui::LayoutHandle::Null\n"
        "YogaIntegration::Layouter::flexDirection(): invalid handle Ui::LayouterDataHandle::Null\n"
        "YogaIntegration::Layouter::setFlexDirection(): invalid handle Ui::LayoutHandle::Null\n"
        "YogaIntegration::Layouter::setFlexDirection(): invalid handle Ui::LayouterDataHandle::Null\n"
        "YogaIntegration::Layouter::nodeOffsetType(): invalid handle Ui::LayoutHandle::Null\n"
        "YogaIntegration::Layouter::nodeOffsetType(): invalid handle Ui::LayouterDataHandle::Null\n"
        "YogaIntegration::Layouter::setNodeOffsetType(): invalid handle Ui::LayoutHandle::Null\n"
        "YogaIntegration::Layouter::setNodeOffsetType(): invalid handle Ui::LayouterDataHandle::Null\n",
        TestSuite::Compare::String);
}

void LayouterTest::clean() {
    Ui::AbstractUserInterface ui{{100, 100}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    Ui::NodeHandle parent = ui.createNode({}, {});
    Ui::NodeHandle child0 = ui.createNode(parent, {}, {});
    Ui::NodeHandle child1 = ui.createNode(parent, {}, {});
    Ui::NodeHandle child11 = ui.createNode(child1, {}, {});
    Ui::NodeHandle child12 = ui.createNode(child1, {}, {});
    Ui::NodeHandle child2 = ui.createNode(parent, {}, {});
    Ui::LayoutHandle parentLayout = layouter.add(parent);
    Ui::LayoutHandle childLayout0 = layouter.add(child0);
    /*Ui::LayoutHandle childLayout1 =*/ layouter.add(child1);
    /*Ui::LayoutHandle childLayout11 =*/ layouter.add(child11);
    /*Ui::LayoutHandle childLayout12 =*/ layouter.add(child12);
    Ui::LayoutHandle childLayout2 = layouter.add(child2);
    CORRADE_COMPARE(ui.nodeUsedCount(), 6);
    CORRADE_COMPARE(layouter.usedCount(), 6);
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(parentLayout)), 3);

    /* Removing a node does nothing to layouters on its own */
    ui.removeNode(child1);
    CORRADE_COMPARE(ui.nodeUsedCount(), 5);
    CORRADE_COMPARE(layouter.usedCount(), 6);
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(parentLayout)), 3);

    /* Cleaning the UI causes the attached layouts as well as all child nodes
       and their layout assignments */
    ui.clean();
    CORRADE_COMPARE(ui.nodeUsedCount(), 3);
    CORRADE_COMPARE(layouter.usedCount(), 3);
    CORRADE_VERIFY(layouter.isHandleValid(parentLayout));
    CORRADE_VERIFY(layouter.isHandleValid(childLayout0));
    CORRADE_VERIFY(layouter.isHandleValid(childLayout2));
    CORRADE_COMPARE(YGNodeGetChildCount(layouter.yogaNode(parentLayout)), 2);
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(parentLayout), 0), layouter.yogaNode(childLayout0));
    CORRADE_COMPARE(YGNodeGetChild(layouter.yogaNode(parentLayout), 1), layouter.yogaNode(childLayout2));
}

void LayouterTest::updateEmpty() {
    Layouter layouter{Ui::layouterHandle(0, 1)};

    /* Required to be called before layout() (because AbstractUserInterface
       guarantees the same on a higher level), not needed for anything here */
    layouter.setSize({1, 1});

    /* It shouldn't crash or do anything weird */
    layouter.layout({}, {}, {}, {}, {}, {}, {}, {}, {});
    CORRADE_VERIFY(true);
}

void LayouterTest::updateDataOrder() {
    auto&& data = UpdateDataOrderData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* The UI size isn't used for anything in this test, so can be arbitrary,
       even less than how the nodes are actually large */
    Ui::AbstractUserInterface ui{{77, 33}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    /*  +-------+-- main ----------+ height: auto
        |       |+--------+        |
        |       ||middle1 |+-------+   ++-dialog-+----+ height: 100
        | left  |+--------+| right |   ||dialog1 |    |
        |  25%  ||  50%   ||  25%  |   |+--------+    |
        |       |+--------+|       |   |              |   +- not in --+
        |       ||middle3 |+-------+   |    +--------+|   | top level |
        |       |+--------+        |   |    |dialog2 ||   |   order   |
        +-------+- middle ---------+   +----+--------++   |+---------+|
                width: 360             width: auto        ||  child  ||
                                                          ++---------++ */

    Ui::NodeHandle main = ui.createNode({15.0f, 35.0f}, {360.0f, 0.0f});
    Ui::NodeHandle left = ui.createNode(main, {}, {25.0f, 0.0f});
    Ui::NodeHandle middle = ui.createNode(main, {}, {50.0f, 0.0f});
    Ui::NodeHandle middle1 = ui.createNode(middle, {}, {0.0f, 50.0f});
    Ui::NodeHandle middle2 = ui.createNode(middle, {}, {0.0f, 20.0f});
    Ui::NodeHandle middle3 = ui.createNode(middle, {}, {0.0f, 30.0f});
    Ui::NodeHandle right = ui.createNode(main, {}, {25.0f, 70.0f});

    Ui::NodeHandle notInTopLevelOrder = ui.createNode({2.5f, 3.5f}, {4.5f, 5.5f});
    Ui::NodeHandle child = ui.createNode(notInTopLevelOrder, {6.5f, 7.5f}, {8.5f, 9.5f});
    ui.clearNodeOrder(notInTopLevelOrder);

    Ui::NodeHandle dialog = ui.createNode({25.0f, 10.0f}, {0.0f, 100.0f});
    Ui::NodeHandle dialog1 = ui.createNode(dialog, {}, {50.0f, 25.0f});
    Ui::NodeHandle dialog2 = ui.createNode(dialog, {}, {50.0f, 25.0f});

    Ui::LayoutHandle mainLayout = layouter.add(main);
    Ui::LayoutHandle middleLayout, rightLayout;
    if(!data.layoutsInNodeOrder) {
        rightLayout = layouter.add(right, Flag::PercentageNodeSizeX);
        middleLayout = layouter.add(middle, rightLayout, Flag::PercentageNodeSizeX);
        layouter.add(middle1);
        Ui::LayoutHandle middle3Layout = layouter.add(middle3);
        layouter.add(left, middleLayout, Flag::PercentageNodeSizeX);
        layouter.add(middle2, middle3Layout);
    } else {
        layouter.add(left, Flag::PercentageNodeSizeX);
        middleLayout = layouter.add(middle, Flag::PercentageNodeSizeX);
        layouter.add(middle1);
        layouter.add(middle2);
        layouter.add(middle3);
        rightLayout = layouter.add(right, Flag::PercentageNodeSizeX);
    }

    /* Main layout is a row, width is specified and height should be
       auto-calculated */
    layouter.setFlexDirection(mainLayout, FlexDirection::Row);
    /* Middle layout is a column, which is the default, no need to set
       anything. The inner items have a 10-unit gap in between. */
    YGNodeStyleSetGap(layouter.yogaNode(middleLayout), YGGutterRow, 10.0f);
    /* The right layout has a fixed height and is centered */
    YGNodeStyleSetAlignSelf(layouter.yogaNode(rightLayout), YGAlignCenter);

    /* The not-in-top-level-order node has layouts but they should do
       nothing because they're not included in the update. Not even setting
       custom properties on those should affect anything. */
    Ui::LayoutHandle notInTopLevelOrderLayout = layouter.add(notInTopLevelOrder);
    YGNodeStyleSetWidth(layouter.yogaNode(notInTopLevelOrderLayout), 33.66f);
    Ui::LayoutHandle childLayout = layouter.add(child);
    YGNodeStyleSetHeightPercent(layouter.yogaNode(childLayout), 11.44f);

    /* Dialog is another top-level node with height specified and width
       auto-calculated. It's positioned relative to the UI, the first child has
       margin on the right and the second child is placed relative to the
       bottom right edge instead of top left. */
    layouter.add(dialog, Flag::PercentageNodeOffset);
    Ui::LayoutHandle dialog1Layout = layouter.add(dialog1);
    YGNodeStyleSetMargin(layouter.yogaNode(dialog1Layout), YGEdgeRight, 25.0f);
    Ui::LayoutHandle dialog2Layout = layouter.add(dialog2, Flag::NodeOffsetFromRightBottom);
    layouter.setNodeOffsetType(dialog2Layout, NodeOffsetType::Absolute);

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    /* Add a dummy second layouter because that's the easiest way verify the
       calculated node offsets / sizes */
    struct DummyLayouter: Ui::AbstractLayouter {
        using Ui::AbstractLayouter::AbstractLayouter;
        using Ui::AbstractLayouter::add;

        Ui::LayouterFeatures doFeatures() const override { return {}; }
        void doLayout(Containers::BitArrayView, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Float>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override {
            CORRADE_COMPARE_AS(nodeOffsets, Containers::stridedArrayView<Vector2>({
                /* The initial offset of main should not change */
                {15.0f, 35.0f},     /* main */
                {0.0f, 0.0f},       /* left */
                {90.0f, 0.0f},      /* middle */
                {0.0f, 0.0f},       /* middle1 */
                {0.0f, 60.0f},      /* middle2 */
                {0.0f, 90.0f},      /* middle3 */
                {270.0f, 25.0f},    /* right */

                /* These offsets shouldn't change as this hierarchy is not
                   visible */
                {2.5f, 3.5f},       /* notInTopLevelOrder */
                {6.5f, 7.5f},       /* child */

                /* The dialog is placed at origin */
                {0.0f, 0.0f},       /* dialog */
                {0.0f, 0.0f},       /* dialog1 */
                {25.0f, 75.0f},     /* dialog2 */
            }), TestSuite::Compare::Container);
            CORRADE_COMPARE_AS(nodeSizes, Containers::stridedArrayView<Vector2>({
                /* The main height should be the sum of the middle heights, the
                   left should match that too, the right should be vertically
                   centered with size preserved */
                {360.0f, 120.0f},   /* main */
                {90.0f, 120.0f},    /* left */
                {180.0f, 120.0f},   /* middle */
                {180.0f, 50.0f},    /* middle1 */
                {180.0f, 20.0f},    /* middle2 */
                {180.0f, 30.0f},    /* middle3 */
                {90.0f, 70.0f},     /* right */

                /* These sizes shouldn't change as this hierarchy is not
                   visible */
                {4.5f, 5.5f},       /* notInTopLevelOrder */
                {8.5f, 9.5f},       /* child */

                /* The dialog width should be calculated based on the margin,
                   height is given */
                {75.0f, 100.0f},    /* dialog */
                {50.0f, 25.0f},     /* dialog1 */
                {50.0f, 25.0f},     /* dialog2 */
            }), TestSuite::Compare::Container);
            ++called;
        }

        Int called = 0;
    };
    DummyLayouter& dummyLayouter = ui.setLayouterInstance(Containers::pointer<DummyLayouter>(ui.createLayouter()));
    dummyLayouter.add(main);
    ui.update();
    CORRADE_COMPARE(dummyLayouter.called, 1);

    /* Verify that the not-in-top-level-order nodes also didn't have their
       custom properties overriden by the update, apart from those properties
       not affecting the node positioning */
    CORRADE_VERIFY(YGNodeStyleGetWidth(layouter.yogaNode(notInTopLevelOrderLayout)) == YGValue{33.66f, YGUnitPoint});
    CORRADE_VERIFY(YGNodeStyleGetHeight(layouter.yogaNode(childLayout)) == YGValue{11.44f, YGUnitPercent});
}

void LayouterTest::updateFlags() {
    auto&& data = UpdateFlagsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* The UI size isn't currently used for anything, so can be arbitrary, even
       less than how the nodes are actually large */
    Ui::AbstractUserInterface ui{{77, 33}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    Ui::NodeHandle parent = ui.createNode({10, 20}, {200, 50});
    Ui::LayoutHandle parentLayout = layouter.add(parent);

    /* Capture correct function name */
    CORRADE_VERIFY(true);

    Ui::NodeHandle child = ui.createNode(parent, data.offset, data.size);
    Ui::LayoutHandle childLayout = layouter.add(child, data.flags);
    if(data.parentFlexDirection)
        layouter.setFlexDirection(parentLayout, *data.parentFlexDirection);
    if(data.childNodeOffsetType)
        layouter.setNodeOffsetType(childLayout, *data.childNodeOffsetType);
    if(data.call)
        data.call(layouter.yogaNode(parentLayout), layouter.yogaNode(childLayout));

    /* Add a dummy second layouter because that's the easiest way verify the
       calculated node offsets / sizes */
    struct DummyLayouter: Ui::AbstractLayouter {
        using Ui::AbstractLayouter::AbstractLayouter;
        using Ui::AbstractLayouter::add;

        Ui::LayouterFeatures doFeatures() const override { return {}; }
        void doLayout(Containers::BitArrayView, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Float>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override {
            /** @todo turn this into a regular container compare once we don't
                support Yoga before 3.0 */
            CORRADE_COMPARE(nodeOffsets.size(), 2);
            {
                /* https://github.com/facebook/yoga/issues/1208 */
                #if YOGA_VERSION < 300000
                CORRADE_EXPECT_FAIL_IF(xfail, "Yoga before version 3.0 swaps left and right side with reverse flex direction, causing the X or Y offset to become negative.");
                #endif
                CORRADE_COMPARE(nodeOffsets[0], (Vector2{10.0f, 20.0f}));
            }
            CORRADE_COMPARE(nodeOffsets[1], expectedOffset);

            CORRADE_COMPARE_AS(nodeSizes, Containers::stridedArrayView<Vector2>({
                {200.0f, 50.0f},
                expectedSize,
            }), TestSuite::Compare::Container);
            ++called;
        }

        Vector2 expectedOffset, expectedSize;
        #if YOGA_VERSION < 300000 /* see above */
        bool xfail;
        #endif
        Int called = 0;
    };
    DummyLayouter& dummyLayouter = ui.setLayouterInstance(Containers::pointer<DummyLayouter>(ui.createLayouter()));
    dummyLayouter.expectedOffset = data.expectedOffset;
    dummyLayouter.expectedSize = data.expectedSize;
    #if YOGA_VERSION < 300000 /* see above */
    FlexDirection direction = layouter.flexDirection(parentLayout);
    dummyLayouter.xfail = direction == FlexDirection::ColumnReverse ||
                          direction == FlexDirection::RowReverse;
    #endif
    dummyLayouter.add(child);
    ui.update();
    CORRADE_COMPARE(dummyLayouter.called, 1);
}

void LayouterTest::updateModifyNodeOffsetSize() {
    auto&& data = UpdateModifyNodeOffsetSizeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Verifies that changing node offsets or sizes results in these being
       correctly reflected in Yoga nodes (instead of done just once, or done
       just if the new value is nonzero, etc.) */

    /* The UI size isn't currently used for anything, so can be arbitrary, even
       less than how the nodes are actually large */
    Ui::AbstractUserInterface ui{{77, 33}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    Ui::NodeHandle root = ui.createNode({}, {100, 200});
    /* The node should either expand to the whole root size or be at least as
       large as the child */
    Ui::NodeHandle node = ui.createNode(root, {20, 30}, {40, 50});
    Ui::NodeHandle child = ui.createNode(node, {}, {15, 25});
    layouter.add(root);
    layouter.add(node);
    layouter.add(child);

    /* Add a dummy second layouter because that's the easiest way verify the
       calculated node offsets / sizes */
    struct DummyLayouter: Ui::AbstractLayouter {
        using Ui::AbstractLayouter::AbstractLayouter;
        using Ui::AbstractLayouter::add;

        Ui::LayouterFeatures doFeatures() const override { return {}; }
        void doLayout(Containers::BitArrayView, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Float>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override {
            CORRADE_COMPARE_AS(nodeOffsets, Containers::stridedArrayView<Vector2>({
                {},
                expectedOffset,
                {},
            }), TestSuite::Compare::Container);

            CORRADE_COMPARE_AS(nodeSizes, Containers::stridedArrayView<Vector2>({
                {100.0f, 200.0f},
                expectedSize,
                {15.0f, 25.0f}
            }), TestSuite::Compare::Container);
            ++called;
        }

        Vector2 expectedOffset, expectedSize;
        Int called = 0;
    };
    DummyLayouter& dummyLayouter = ui.setLayouterInstance(Containers::pointer<DummyLayouter>(ui.createLayouter()));
    dummyLayouter.add(node);
    dummyLayouter.expectedOffset = {20.0f, 30.0f};
    dummyLayouter.expectedSize = {40.0f, 50.0f};
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
    }
    CORRADE_COMPARE(dummyLayouter.called, 1);

    if(data.updateOffset)
        ui.setNodeOffset(node, *data.updateOffset);
    if(data.updateSize)
        ui.setNodeSize(node, *data.updateSize);
    dummyLayouter.expectedOffset = data.expectedOffset;
    dummyLayouter.expectedSize = data.expectedSize;
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
    }
    CORRADE_COMPARE(dummyLayouter.called, 2);
}

void LayouterTest::updateModifyNodeVisibility() {
    auto&& data = UpdateModifyNodeVisiblityData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* The UI size isn't currently used for anything, so can be arbitrary, even
       less than how the nodes are actually large */
    Ui::AbstractUserInterface ui{{77, 33}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    Ui::NodeHandle root = ui.createNode({}, {});
    Ui::NodeHandle child1 = ui.createNode(root, {}, {0.0f, 100.0f});
    Ui::NodeHandle childHidden = ui.createNode(root, {}, {0.0f, 150.0f});
    Ui::NodeHandle childNested = ui.createNode(childHidden, {}, {300.0f, 50.0f});
    Ui::NodeHandle child3 = ui.createNode(root, {}, {100.0f, 50.0f});
    layouter.add(root);
    layouter.add(child1);
    Ui::LayoutHandle childHiddenLayout = layouter.add(childHidden);
    layouter.add(childNested);
    layouter.add(child3);

    /* Add a dummy second layouter because that's the easiest way verify the
       calculated node offsets / sizes */
    struct DummyLayouter: Ui::AbstractLayouter {
        using Ui::AbstractLayouter::AbstractLayouter;
        using Ui::AbstractLayouter::add;

        Ui::LayouterFeatures doFeatures() const override { return {}; }
        void doLayout(Containers::BitArrayView, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Float>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override {
            CORRADE_COMPARE_AS(nodeOffsets, Containers::stridedArrayView<Vector2>({
                {0.0f, 0.0f},       /* root */
                {0.0f, 0.0f},       /* child1 */
                {0.0f, expect ? 100.0f : 0.0f}, /* childHidden */
                {0.0f, 0.0f},       /* childNested */
                {0.0f, expect ? 250.0f : 100.0f}, /* child3 */
            }), TestSuite::Compare::Container);
            CORRADE_COMPARE_AS(nodeSizes, Containers::stridedArrayView<Vector2>({
                {expect ? 300.0f : 100.0f,
                    expect ? 300.0f : 150.0f},   /* root */
                {expect ? 300.0f : 100.0f, 100.0f},   /* child1 */
                {expect ? 300.0f : 0.0f, 150.0f},     /* childHidden */
                {300.0f, 50.0f},    /* childNested */
                {100.0f, 50.0f}, /* child3 */
            }), TestSuite::Compare::Container);
            ++called;
        }

        bool expect;
        Int called = 0;
    };
    DummyLayouter& dummyLayouter = ui.setLayouterInstance(Containers::pointer<DummyLayouter>(ui.createLayouter()));
    dummyLayouter.add(root);

    /* Initially the child is shown and its properties are populated */
    dummyLayouter.expect = true;
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
    }
    CORRADE_COMPARE(dummyLayouter.called, 1);

    /* Once hidden, it should be excluded from the layout */
    ui.addNodeFlags(childHidden, Ui::NodeFlag::Hidden);
    if(data.explicitDisplayNone)
        YGNodeStyleSetDisplay(layouter.yogaNode(childHiddenLayout), YGDisplayNone);
    dummyLayouter.expect = false;
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        /** @todo implement this and then remove the XFAIL */
        CORRADE_EXPECT_FAIL_IF(data.xfail, "Node visibility isn't propagated to the layouters right now.");
        ui.update();
    }
    CORRADE_COMPARE(dummyLayouter.called, 2);
}

void LayouterTest::updateModifyLayoutFlexDirection() {
    /* Verifies that calling setFlexDirection() results in the change being
       correctly reflected in Yoga nodes. The API triggering NeedsUpdate is
       tested in flexDirection() above, but that alone doesn't ensure Yoga
       picks up the difference. */

    /* The UI size isn't currently used for anything, so can be arbitrary, even
       less than how the nodes are actually large */
    Ui::AbstractUserInterface ui{{77, 33}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    Ui::NodeHandle root = ui.createNode({}, {100, 200});
    Ui::NodeHandle node = ui.createNode(root, {20, 30}, {40, 50});
    Ui::LayoutHandle rootLayout = layouter.add(root);
    layouter.add(node);

    /* Add a dummy second layouter because that's the easiest way verify the
       calculated node offsets / sizes */
    struct DummyLayouter: Ui::AbstractLayouter {
        using Ui::AbstractLayouter::AbstractLayouter;
        using Ui::AbstractLayouter::add;

        Ui::LayouterFeatures doFeatures() const override { return {}; }
        void doLayout(Containers::BitArrayView, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Float>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override {
            CORRADE_COMPARE_AS(nodeOffsets, Containers::stridedArrayView<Vector2>({
                {},
                expectedOffset,
            }), TestSuite::Compare::Container);

            CORRADE_COMPARE_AS(nodeSizes, Containers::stridedArrayView<Vector2>({
                {100.0f, 200.0f},
                {40.0f, 50.0f},
            }), TestSuite::Compare::Container);
            ++called;
        }

        Vector2 expectedOffset;
        Int called = 0;
    };
    DummyLayouter& dummyLayouter = ui.setLayouterInstance(Containers::pointer<DummyLayouter>(ui.createLayouter()));
    dummyLayouter.add(root);
    dummyLayouter.expectedOffset = {20.0f, 30.0f};
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
    }
    CORRADE_COMPARE(dummyLayouter.called, 1);

    /* It should be enough to verify that the single node gets positioned
       elsewhere, not that others will appear to the left of it etc. We're not
       supplementing Yoga's test suite here. */
    layouter.setFlexDirection(rootLayout, FlexDirection::RowReverse);
    dummyLayouter.expectedOffset = {100.0f - 40.0f - 20.0f, 30.0f};
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
    }
    CORRADE_COMPARE(dummyLayouter.called, 2);
}

void LayouterTest::updateModifyLayoutNodeOffsetType() {
    /* Verifies that calling setNodeOffsetType() results in the change being
       correctly reflected in Yoga nodes. The API triggering NeedsUpdate is
       tested in nodeOffsetType() above, but that alone doesn't ensure Yoga
       picks up the difference. */

    /* The UI size isn't currently used for anything, so can be arbitrary, even
       less than how the nodes are actually large */
    Ui::AbstractUserInterface ui{{77, 33}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    Ui::NodeHandle root = ui.createNode({}, {100, 200});
    Ui::NodeHandle node1 = ui.createNode(root, {}, {40, 50});
    Ui::NodeHandle node2 = ui.createNode(root, {20, 30}, {60, 70});
    layouter.add(root);
    layouter.add(node1);
    Ui::LayoutHandle node2Layout = layouter.add(node2);

    /* Add a dummy second layouter because that's the easiest way verify the
       calculated node offsets / sizes */
    struct DummyLayouter: Ui::AbstractLayouter {
        using Ui::AbstractLayouter::AbstractLayouter;
        using Ui::AbstractLayouter::add;

        Ui::LayouterFeatures doFeatures() const override { return {}; }
        void doLayout(Containers::BitArrayView, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Float>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override {
            CORRADE_COMPARE_AS(nodeOffsets, Containers::stridedArrayView<Vector2>({
                {},
                {},
                expectedOffset,
            }), TestSuite::Compare::Container);

            CORRADE_COMPARE_AS(nodeSizes, Containers::stridedArrayView<Vector2>({
                {100.0f, 200.0f},
                {40.0f, 50.0f},
                {60.0f, 70.0f},
            }), TestSuite::Compare::Container);
            ++called;
        }

        Vector2 expectedOffset;
        Int called = 0;
    };
    DummyLayouter& dummyLayouter = ui.setLayouterInstance(Containers::pointer<DummyLayouter>(ui.createLayouter()));
    dummyLayouter.add(root);
    /* Initially the second node is placed below the first, with this offset in
       addition */
    dummyLayouter.expectedOffset = {20.0f, 50.0f + 30.0f};
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
    }
    CORRADE_COMPARE(dummyLayouter.called, 1);

    /* It should be enough to verify that the single node gets positioned
       elsewhere, not that others will appear to the left of it etc. We're not
       supplementing Yoga's test suite here. */
    layouter.setNodeOffsetType(node2Layout, NodeOffsetType::Absolute);
    /* Now the offset is relatively to the top left corner, without considering
       node1 */
    dummyLayouter.expectedOffset = {20.0f, 30.0f};
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
    }
    CORRADE_COMPARE(dummyLayouter.called, 2);
}

void LayouterTest::updateModifyYogaConfigDirectly() {
    Ui::AbstractUserInterface ui{{100, 100}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    /* Add a node with a layout */
    Ui::NodeHandle node = ui.createNode({3.5f, 7.4f}, {6.3f, 8.6f});
    layouter.add(node);

    /* Add a dummy second layouter because that's the easiest way verify the
       calculated node offsets / sizes */
    struct DummyLayouter: Ui::AbstractLayouter {
        using Ui::AbstractLayouter::AbstractLayouter;
        using Ui::AbstractLayouter::add;

        Ui::LayouterFeatures doFeatures() const override { return {}; }
        void doLayout(Containers::BitArrayView, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Float>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override {
            CORRADE_COMPARE_AS(nodeOffsets, Containers::stridedArrayView<Vector2>({
                expectedOffset,
            }), TestSuite::Compare::Container);
            CORRADE_COMPARE_AS(nodeSizes,Containers::stridedArrayView<Vector2>({
                expectedSize,
            }), TestSuite::Compare::Container);
            ++called;
        }

        Vector2 expectedOffset, expectedSize;
        Int called = 0;
    };
    DummyLayouter& dummyLayouter = ui.setLayouterInstance(Containers::pointer<DummyLayouter>(ui.createLayouter()));
    dummyLayouter.add(node);

    /* By default nothing is rounded */
    dummyLayouter.expectedOffset = {3.5f, 7.4f};
    dummyLayouter.expectedSize = {6.3f, 8.6f};
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
        CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});
        CORRADE_COMPARE(dummyLayouter.called, 1);
    }

    /* Querying the config doesn't set any state */
    YGConfigRef config = layouter.yogaConfig();
    CORRADE_COMPARE(YGConfigGetPointScaleFactor(config), 0.0f);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});

    /* Changing the config doesn't do that either, and so update() won't do
       anything */
    YGConfigSetPointScaleFactor(config, 1.0f);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
        CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});
        CORRADE_COMPARE(dummyLayouter.called, 1);
    }

    /* One has to explicitly call setNeedsUpdate() to actually propagate the
       changes. Now the positions are rounded. */
    layouter.setNeedsUpdate();
    /** @todo Yoga 3.2.0+ has an internal "config version" counter, if it'd be
        exposed in the public API the layouter could track it (assuming the
        interfaces would get a doState()) and request an update if the version
        changes */
    dummyLayouter.expectedOffset = {4.0f, 7.0f};
    dummyLayouter.expectedSize = {6.0f, 9.0f};
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        {
            #if YOGA_VERSION < 302000
            /* https://github.com/facebook/yoga/pull/1674 */
            CORRADE_EXPECT_FAIL("Yoga before version 3.2.0 doesn't correctly relayout on config change.");
            #endif
            ui.update();
        }
        CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});
        CORRADE_COMPARE(dummyLayouter.called, 2);
    }
}

void LayouterTest::updateModifyYogaNodeDirectly() {
    Ui::AbstractUserInterface ui{{100, 100}};
    /* The layouter has to be a part of user interface to have access to node
       hierarchy */
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));

    /* Add a node with a layout */
    Ui::NodeHandle parent = ui.createNode({}, {100.0f, 100.0f});
    Ui::NodeHandle child = ui.createNode(parent, {}, {0.0f, 25.0f});
    Ui::LayoutHandle parentLayout = layouter.add(parent);
    layouter.add(child);

    /* Add a dummy second layouter because that's the easiest way verify the
       calculated node offsets / sizes */
    struct DummyLayouter: Ui::AbstractLayouter {
        using Ui::AbstractLayouter::AbstractLayouter;
        using Ui::AbstractLayouter::add;

        Ui::LayouterFeatures doFeatures() const override { return {}; }
        void doLayout(Containers::BitArrayView, const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Vector2>&, const Containers::StridedArrayView1D<Float>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector4>&, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override {
            CORRADE_COMPARE_AS(nodeOffsets, Containers::stridedArrayView<Vector2>({
                {},
                expectedOffset,
            }), TestSuite::Compare::Container);
            CORRADE_COMPARE_AS(nodeSizes, Containers::stridedArrayView<Vector2>({
                {100.0f, 100.0f},
                expectedSize,
            }), TestSuite::Compare::Container);
            ++called;
        }

        Vector2 expectedOffset, expectedSize;
        Int called = 0;
    };
    DummyLayouter& dummyLayouter = ui.setLayouterInstance(Containers::pointer<DummyLayouter>(ui.createLayouter()));
    dummyLayouter.add(child);

    /* By default the child expands to the whole width and is at the top */
    dummyLayouter.expectedOffset = {};
    dummyLayouter.expectedSize = {100.0f, 25.0f};
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
        CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});
        CORRADE_COMPARE(dummyLayouter.called, 1);
    }

    /* Querying the yoga node doesn't set any state */
    YGNodeRef parentLayoutYogaNode = layouter.yogaNode(parentLayout);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});

    /* Changing the properties doesn't do that either, and so update() won't do
       anything. (Obviously not using setFlexDirection() because that one
       *does* set it.) */
    YGNodeStyleSetFlexDirection(parentLayoutYogaNode, YGFlexDirectionColumnReverse);
    YGNodeStyleSetPadding(parentLayoutYogaNode, YGEdgeAll, 10.0f);
    CORRADE_COMPARE(layouter.state(), Ui::LayouterStates{});
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
        CORRADE_COMPARE(dummyLayouter.called, 1);
    }

    /* One has to explicitly call setNeedsUpdate() to actually propagate the
       changes. Now the child is at the bottom and expands only until the
       padding. */
    /** @todo Yoga has "node changed" callbacks, we could possibly attach a
        callback to each node to call setNeedsUpdate() on the layouter in that
        case ... but it feels like a lot of extra function calls, especially
        given that they're all modified during layout() */
    layouter.setNeedsUpdate();
    dummyLayouter.expectedOffset = {10.0f, 65.0f};
    dummyLayouter.expectedSize = {80.0f, 25.0f};
    {
        CORRADE_ITERATION(__FILE__ ":" CORRADE_LINE_STRING);
        ui.update();
        CORRADE_COMPARE(dummyLayouter.called, 2);
    }
}

void LayouterTest::debugIntegration() {
    auto&& data = DebugIntegrationData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Ui::AbstractUserInterface ui{{100, 100}};
    Ui::NodeHandle root = ui.createNode({}, {100, 100});
    Ui::NodeHandle node = ui.createNode(root, {}, {100, 100});

    /* Create and remove a bunch of layouters first to have the handle with a
       non-trivial value */
    ui.removeLayouter(ui.createLayouter());
    ui.removeLayouter(ui.createLayouter());
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));
    /* And also some more layouts to not list a trivial handle */
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.remove(layouter.add(ui.createNode({}, {})));
    Ui::LayoutHandle layout = layouter.add(node, data.flags);
    if(data.flexDirection)
        layouter.setFlexDirection(layout, *data.flexDirection);
    if(data.nodeOffsetType)
        layouter.setNodeOffsetType(layout, *data.nodeOffsetType);

    Ui::DebugLayer& debugLayer = ui.setLayerInstance(Containers::pointer<Ui::DebugLayer>(ui.createLayer(), Ui::DebugLayerSource::NodeLayoutDetails, Ui::DebugLayerFlag::NodeInspect));

    Containers::String out;
    debugLayer.setNodeInspectCallback([&out](Containers::StringView message) {
        out = message;
    });
    /* Each Yoga node allocation is 586 bytes! Web scale!! */
    debugLayer.setLayouterName(layouter, data.layouterName ? "WebScaleTech!!" : "");

    /* Make the debug layer aware of everything */
    ui.update();

    CORRADE_VERIFY(debugLayer.inspectNode(node));
    CORRADE_COMPARE_AS(out, data.expected, TestSuite::Compare::String);
}

void LayouterTest::debugIntegrationNoCallback() {
    Ui::AbstractUserInterface ui{{100, 100}};
    Ui::NodeHandle root = ui.createNode({}, {100, 100});
    Ui::NodeHandle node = ui.createNode(root, {}, {100, 100});

    /* Just to match the layouter handle in debugIntegration() above */
    ui.removeLayouter(ui.createLayouter());
    ui.removeLayouter(ui.createLayouter());
    Layouter& layouter = ui.setLayouterInstance(Containers::pointer<Layouter>(ui.createLayouter()));
    /* ... and the data handle also */
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.remove(layouter.add(ui.createNode({}, {})));
    Ui::LayoutHandle layout = layouter.add(node, Flag::IgnoreNodeSize|Flag::NodeOffsetFromRight|Flag::PercentageNodeOffsetY);
    layouter.setFlexDirection(layout, FlexDirection::ColumnReverse);
    layouter.setNodeOffsetType(layout, NodeOffsetType::Absolute);

    Ui::DebugLayer& debugLayer = ui.setLayerInstance(Containers::pointer<Ui::DebugLayer>(ui.createLayer(), Ui::DebugLayerSource::NodeLayoutDetails, Ui::DebugLayerFlag::NodeInspect));

    debugLayer.setLayouterName(layouter, "WebScaleTech!!");

    /* Make the debug layer aware of everything */
    ui.update();

    /* Inspect the node for visual color verification */
    {
        Debug{} << "======================== visual color verification start =======================";

        debugLayer.addFlags(Ui::DebugLayerFlag::ColorAlways);

        CORRADE_VERIFY(debugLayer.inspectNode(node));

        debugLayer.clearFlags(Ui::DebugLayerFlag::ColorAlways);

        Debug{} << "======================== visual color verification end =========================";
    }

    /* Do the same, but this time with output redirection to verify the
       contents. The internals automatically disable coloring if they detect
       the output isn't a TTY. */
    {
        Containers::String out;
        Debug redirectOutput{&out};
        CORRADE_VERIFY(debugLayer.inspectNode(node));
        /* The output always has a newline at the end which cannot be disabled
           so strip it to have the comparison match the debugIntegration()
           case */
        CORRADE_COMPARE_AS(out,
            "\n",
            TestSuite::Compare::StringHasSuffix);
        CORRADE_COMPARE_AS(out.exceptSuffix("\n"),
            Containers::arrayView(DebugIntegrationData).back().expected,
            TestSuite::Compare::String);
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::YogaIntegration::Test::LayouterTest)
