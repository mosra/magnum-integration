#ifndef Magnum_YogaIntegration_Layouter_h
#define Magnum_YogaIntegration_Layouter_h
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

/** @file
 * @brief Class @ref Magnum::YogaIntegration::Layouter, enum @ref Magnum::YogaIntegration::Flag, @ref Magnum::YogaIntegration::FlexDirection, @ref Magnum::YogaIntegration::NodeOffsetType, enum set @ref Magnum::YogaIntegration::Flags
 * @m_since_latest_{integration}
 */

#include <Magnum/Ui/AbstractLayouter.h>

#include "Magnum/YogaIntegration/visibility.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
/* To not have to include the whole of Yoga */
typedef struct YGConfig* YGConfigRef;
typedef const struct YGConfig* YGConfigConstRef;
typedef struct YGNode* YGNodeRef;
typedef const struct YGNode* YGNodeConstRef;
#endif

namespace Magnum { namespace YogaIntegration {

/**
@brief Layout flag
@m_since_latest_{integration}

@see @ref Flags, @ref Layouter::add(), @ref Layouter::flags()
*/
enum class Flag: UnsignedShort {
    /**
     * Interpret X node offset coming from
     * @ref Ui::AbstractUserInterface::nodeOffset() as a percentage value
     * instead of UI units. If enabled together with
     * @ref Flag::IgnoreNodeOffsetX, @relativeref{Flag,IgnoreNodeOffsetX} gets
     * a precedence.
     */
    PercentageNodeOffsetX = 1 << 0,

    /**
     * Interpret Y node offset coming from
     * @ref Ui::AbstractUserInterface::nodeOffset() as a percentage value
     * instead of UI units. If enabled together with
     * @ref Flag::IgnoreNodeOffsetY, @relativeref{Flag,IgnoreNodeOffsetY} gets
     * a precedence.
     */
    PercentageNodeOffsetY = 1 << 1,

    /**
     * Interpret X and Y node offset coming from
     * @ref Ui::AbstractUserInterface::nodeOffset() as a percentage value
     * instead of UI units. Shorthand for a combination of
     * @ref Flag::PercentageNodeOffsetX and
     * @relativeref{Flag,PercentageNodeOffsetY}. If enabled together with
     * @ref Flag::IgnoreNodeOffset, @relativeref{Flag,IgnoreNodeOffset} gets a
     * precedence.
     */
    PercentageNodeOffset = PercentageNodeOffsetX|PercentageNodeOffsetY,

    /**
     * Interpret X node offset coming from
     * @ref Ui::AbstractUserInterface::nodeOffset() as being from the right,
     * with positive values in a right-to-left direction, instead of from the
     * left with positive values in a left-to-right direction. If enabled
     * together with @ref Flag::IgnoreNodeOffsetX,
     * @relativeref{Flag,IgnoreNodeOffsetX} gets a precedence.
     */
    NodeOffsetFromRight = 1 << 2,

    /** @todo Start/End offset as well, once LTR/RTL direction switching is
        exposed as well */

    /**
     * Interpret Y node offset coming from
     * @ref Ui::AbstractUserInterface::nodeOffset() as being from the bottom,
     * with positive values in a bottom-to-top direction, instead of from the
     * top with positive values in a top-to-bottom direction. If enabled
     * together with @ref Flag::IgnoreNodeOffsetY,
     * @relativeref{Flag,IgnoreNodeOffsetY} gets a precedence.
     */
    NodeOffsetFromBottom = 1 << 3,

    /**
     * @brief Interpret X and Y node offset coming from
     * @ref Ui::AbstractUserInterface::nodeOffset() as being from the right
     * bottom instead of left top. Shorthand for a combination of
     * @ref Flag::NodeOffsetFromRight and
     * @relativeref{Flag,NodeOffsetFromBottom}. If enabled together with
     * @ref Flag::IgnoreNodeOffset, @relativeref{Flag,IgnoreNodeOffset} gets a
     * precedence.
     */
    NodeOffsetFromRightBottom = NodeOffsetFromRight|NodeOffsetFromBottom,

    /**
     * Ignore X node offset coming from
     * @ref Ui::AbstractUserInterface::nodeOffset() and use the value specified
     * directly using Yoga APIs instead. If specified together with
     * @ref Flag::PercentageNodeOffsetX or
     * @relativeref{Flag,NodeOffsetFromRight}, this flag gets a precedence.
     */
    IgnoreNodeOffsetX = PercentageNodeOffsetX|NodeOffsetFromRight|(1 << 4),

    /**
     * Ignore Y node offset coming from
     * @ref Ui::AbstractUserInterface::nodeOffset() and use the value specified
     * directly using Yoga APIs instead. If specified together with
     * @ref Flag::PercentageNodeOffsetY or
     * @relativeref{Flag,NodeOffsetFromBottom}, this flag gets a precedence.
     */
    IgnoreNodeOffsetY = PercentageNodeOffsetY|NodeOffsetFromBottom|(1 << 5),

    /**
     * Ignore X and Y node offset coming from
     * @ref Ui::AbstractUserInterface::nodeOffset() and use the value specified
     * directly using Yoga APIs instead. Shorthand for a combination of
     * @ref Flag::IgnoreNodeOffsetX and @relativeref{Flag,IgnoreNodeOffsetY}.
     * If specified together with @ref Flag::PercentageNodeOffset or
     * @relativeref{Flag,NodeOffsetFromRightBottom}, this flag gets a
     * precedence.
     */
    IgnoreNodeOffset = IgnoreNodeOffsetX|IgnoreNodeOffsetY,

    /**
     * Interpret X node size coming from
     * @ref Ui::AbstractUserInterface::nodeSize() as a percentage value instead
     * of UI units. If enabled together with @ref Flag::IgnoreNodeSizeX,
     * @relativeref{Flag,IgnoreNodeSizeX} gets a precedence.
     */
    PercentageNodeSizeX = 1 << 6,

    /**
     * Interpret Y node size coming from
     * @ref Ui::AbstractUserInterface::nodeSize() as a percentage value instead
     * of UI units. If enabled together with @ref Flag::IgnoreNodeSizeY,
     * @relativeref{Flag,IgnoreNodeSizeY} gets a precedence.
     */
    PercentageNodeSizeY = 1 << 7,

    /**
     * Interpret X and Y node size coming from
     * @ref Ui::AbstractUserInterface::nodeSize() as a percentage value instead
     * of UI units. Shorthand for a combination of
     * @ref Flag::PercentageNodeOffsetX and
     * @relativeref{Flag,PercentageNodeOffsetY}. If enabled together with
     * @ref Flag::IgnoreNodeSize, @relativeref{Flag,IgnoreNodeSize} gets a
     * precedence.
     */
    PercentageNodeSize = PercentageNodeSizeX|PercentageNodeSizeY,

    /**
     * Ignore X node size coming from
     * @ref Ui::AbstractUserInterface::nodeSize() and use the value specified
     * directly using Yoga APIs instead. If specified together with
     * @ref Flag::PercentageNodeSizeX, this flag gets a precedence.
     */
    IgnoreNodeSizeX = PercentageNodeSizeX|(1 << 8),

    /**
     * Ignore Y node size coming from
     * @ref Ui::AbstractUserInterface::nodeSize() and use the value specified
     * directly using Yoga APIs instead. If specified together with
     * @ref Flag::PercentageNodeSizeY, this flag gets a precedence.
     */
    IgnoreNodeSizeY = PercentageNodeSizeY|(1 << 9),

    /**
     * Ignore X and Y node size coming from
     * @ref Ui::AbstractUserInterface::nodeSize() and use the value specified
     * directly using Yoga APIs instead. Shorthand for a combination of
     * @ref Flag::IgnoreNodeSizeX and @relativeref{Flag,IgnoreNodeSizeY}. If
     * specified together with @ref Flag::PercentageNodeSize, this flag gets a
     * precedence.
     */
    IgnoreNodeSize = IgnoreNodeSizeX|IgnoreNodeSizeY,
};

/**
@debugoperatorenum{Flag}
@m_since_latest_{integration}
*/
MAGNUM_YOGAINTEGRATION_EXPORT Debug& operator<<(Debug& debug, Flag value);

/**
@brief Layout flags
@m_since_latest_{integration}

@see @ref Layouter::add(), @ref Layouter::flags()
*/
typedef Containers::EnumSet<Flag> Flags;

/**
@debugoperatorenum{Flags}
@m_since_latest_{integration}
*/
MAGNUM_YOGAINTEGRATION_EXPORT Debug& operator<<(Debug& debug, Flags value);

CORRADE_ENUMSET_OPERATORS(Flags)

/**
@brief Layout flex direction
@m_since_latest_{integration}

Corresponds to the [CSS `flex-direction` property](https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Properties/flex-direction),
see the linked documentation for detailed explanation.
@see @ref Layouter::setFlexDirection(), @ref Layouter::flexDirection()
*/
enum class FlexDirection: UnsignedInt {
    /* Don't want to bring the full Yoga headers to the public interface, but
       the implementation still expects the numeric ID match between these
       enums and Yoga's so it can just cast them without translating. */

    /**
     * Orders child nodes in a column. Matches behavior of
     * @cb{.css} flex-direction: column; @ce in CSS. This is the default in
     * Yoga.
     */
    Column = 0,

    /**
     * Orders child nodes in a column, in reverse order. Matches behavior of
     * @cb{.css} flex-direction: column-reverse; @ce in CSS.
     */
    ColumnReverse = 1,

    /**
     * Orders child nodes in a row. Matches behavior of
     * @cb{.css} flex-direction: row; @ce in CSS.
     */
    Row = 2,

    /**
     * Orders child nodes in a row, in reverse order. Matches behavior of
     * @cb{.css} flex-direction: row; @ce in CSS.
     */
    RowReverse = 3,
};

/**
@debugoperatorenum{FlexDirection}
@m_since_latest_{integration}
*/
MAGNUM_YOGAINTEGRATION_EXPORT Debug& operator<<(Debug& debug, FlexDirection value);

/**
@brief Layout node offset type
@m_since_latest_{integration}

Corresponds to the [CSS `position` property](https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Properties/position),
see the linked documentation for detailed explanation.
@see @ref Layouter::setNodeOffsetType(), @ref Layouter::nodeOffsetType()
*/
enum class NodeOffsetType: UnsignedInt {
    /* Don't want to bring the full Yoga headers to the public interface, but
       the implementation still expects the numeric ID match between these
       enums and Yoga's so it can just cast them without translating. */

    /**
     * The offset is applied as relative to the result of given node layout
     * calculation and doesn't affect layout of other nodes. Matches behavior
     * of @cb{.css} position: relative; @ce in CSS. This is the default in Yoga.
     */
    Relative = 1,

    /**
     * The node is excluded from layout calculation and the offset is applied
     * relatively to the parent node offset and size. Matches behavior of
     * @cb{.css} position: absolute; @ce in CSS. By default the offset is
     * interpreted relative to the top left corner of parent node, use
     * @ref Flag::NodeOffsetFromRight, @relativeref{Flag,NodeOffsetFromBottom}
     * or @relativeref{Flag,NodeOffsetFromRightBottom} to make it relative to
     * other edges.
     */
    Absolute = 2
};

/**
@debugoperatorenum{NodeOffsetType}
@m_since_latest_{integration}
*/
MAGNUM_YOGAINTEGRATION_EXPORT Debug& operator<<(Debug& debug, NodeOffsetType value);

/**
@brief Yoga layouter
@m_since_latest_{integration}

A @ref Ui::AbstractLayouter "layouter implementation" for the @ref Ui library
using [Yoga Layout](https://yogalayout.dev), which exposes a subset of
[CSS Flexbox](https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_flexible_box_layout).

@section YogaIntegration-Layouter-setup Setting up a layouter instance

The layouter doesn't have any shared state or configuration, so it's just about
constructing it from a fresh @ref Ui::AbstractUserInterface::createLayouter()
handle and passing it to @relativeref{Ui::AbstractUserInterface,setLayouterInstance()}.
After that, it's ready to use:

@snippet YogaIntegration.cpp Layouter-setup

@section YogaIntegration-Layouter-add Adding layouts to nodes

A layout is added to a particular node by calling @ref add(). You can save the
returned @ref Ui::LayoutHandle if you need to refer the layout later, the
layout assigned to a particular node is also accessible via
@ref Ui::AbstractUserInterface::nodeUniqueLayout() later. But the common use
case is just adding a layout and not touching it in any way afterwards:

@snippet yogaintegration.cpp Layouter-add

Yoga by default lays out the nodes in a column and in the order @ref add() is
called. The layouts inherit node sizes, if specified, so in the above snippet
the children all have a width of @cpp 150 @ce, and heights of @cpp 10 @ce,
@cpp 30 @ce and @cpp 20 @ce respectively. The `root` node has no size specified
and thus Yoga implicitly sizes it to fit all children. When visualizing node
placement, for example with @ref Ui-DebugLayer-node-highlight "Ui::DebugLayer node highlight",
the layout looks like this, with `root` being underneath the three children:

@image html yogaintegration-add.png width=240px

The @ref add() function optionally accepts a layout *before* which to insert
given child. To switch the layout direction for children of a particular node,
use @ref setFlexDirection(). The following snippet inserts a row above the
`bottom` node, with three nested nodes next to each other:

@snippet yogaintegration.cpp Layouter-add-insert-row

In the above @ref Flag::PercentageNodeSize is used, so their width is
interpreted not in UI units but as 15%, 70% and 15%, respectively. The height
is unspecified, which makes them expand to the whole height of `middle`, i.e.
@cpp 40 @ce units:

@image html yogaintegration-add-insert-row.png width=240px

When adding layouts, restrictions coming from
@ref Ui::LayouterFeature::UniqueLayouts apply, i.e. a particular node can have
only at most one layout from given layouter instance, and if adding layouts to
both a parent node and its children, the parent layout has to be added first so
it can correctly track and order its children.

@section YogaIntegration-Layouter-absolute-positioning Absolute positioning

Node offsets, if specified, by default shift the node *relatively* to its
layout position. Calling @ref setNodeOffsetType() allows you to change the
offset to *absolute*, excluding given node from layout calculations and
positioning it exactly as specified. Together with
@ref Flag::NodeOffsetFromRight, @relativeref{Flag,NodeOffsetFromBottom} or
@relativeref{Flag,NodeOffsetFromRightBottom} the offset can be then interpreted
from bottom right instead of top left. Here a `footer` is placed 15 units from
the bottom right corner of the `bottom` node:

@snippet yogaintegration.cpp Layouter-add-bottom-right

@image html yogaintegration-add-bottom-right.png width=240px

Offsets, including absolute offsets, can be interpreted as percentages as well
by enabling @ref Flag::PercentageNodeOffset. Just note that absolute
positioning is basically sidesteping most of Yoga's layouting capabilities, and
if applied extensively you might want to consider not using Yoga at all.

@section YogaIntegration-Layouter-direct Direct access to Yoga functionality

Right now, the @ref Layouter exposes only a subset of Yoga functionality. It's
however possible to call Yoga APIs directly on the Yoga node references
returned by @ref yogaNode(). Note that, however, you need to call
@ref setNeedsUpdate() afterwards to make the layouter aware of the changes and
update the layout appropriately:

@snippet YogaIntegration.cpp Layouter-direct

In case the implicit behavior of inheriting node offset and size is conflicting
with direct Yoga API use, you might want to enable @ref Flag::IgnoreNodeOffset,
@relativeref{Flag,IgnoreNodeSize} and related flags on affected nodes. With
those, the Yoga nodes are only used to query the the calculated layout results,
but they're never updated when node offsets or sizes change, leaving them
completely under your control.

@section YogaIntegration-Layouter-configuration Global Yoga configuration

All created layouts share the same Yoga configuration, which is exposed through
@ref yogaConfig(). Compared to Yoga defaults, [snapping to a pixel grid](https://www.yogalayout.dev/docs/getting-started/configuring-yoga#point-scale-factor)
is disabled to avoid stutter when animating node offsets and sizes. Similarly
as with direct Yoga node access shown above, you can access and update the
configuration yourself, but again you need to call @ref setNeedsUpdate()
afterwards to make the layouter aware of the changes. For example, the
following snippet enables pixel grid snapping back:

@snippet YogaIntegration.cpp Layouter-yogaConfig

@section YogaIntegration-Layouter-debug-integration Debug layer integration

When using @ref Ui-DebugLayer-node-inspect "DebugLayer node inspect" and
@ref Ui::DebugLayerSource::NodeLayoutDetails is enabled, passing this layouter
to @ref Ui::DebugLayer::setLayouterName(const T&, const Containers::StringView&) "Ui::DebugLayer::setLayouterName()"
will make it list basic properties of a particular layout. Because the layout
inherits node offset and size, it's useful to enable
@ref Ui::DebugLayerSource::NodeOffsetSize as well. For example:

@include yogaintegration-debuglayer.ansi
*/
class MAGNUM_YOGAINTEGRATION_EXPORT Layouter: public Ui::AbstractLayouter {
    public:
        class DebugIntegration;

        /**
         * @brief Constructor
         * @param handle    Layouter handle returned from
         *      @ref Ui::AbstractUserInterface::createLayouter()
         */
        explicit Layouter(Ui::LayouterHandle handle);

        /** @brief Copying is not allowed */
        Layouter(const Layouter&) = delete;

        /** @copydoc Ui::AbstractLayouter::AbstractLayouter(AbstractLayouter&&) */
        Layouter(Layouter&&) noexcept;

        ~Layouter();

        /** @brief Copying is not allowed */
        Layouter& operator=(const Layouter&) = delete;

        /** @brief Move assignment */
        Layouter& operator=(Layouter&&) noexcept;

        /**
         * @brief Yoga configuration used for all layout nodes
         *
         * @m_class{m-note m-warning}
         *
         * @par
         *      Note that, due to limitations of the public Yoga API, it's not
         *      possible for the layouter to detect changes made to the Yoga
         *      configuration externally. If you're modifying the returned
         *      configuration directly using Yoga APIs, you need to call
         *      @ref setNeedsUpdate() afterwards to make the layouter aware of
         *      the changes and update the layout appropriately. For example:
         * @par
         *      @snippet YogaIntegration.cpp Layouter-yogaConfig
         * @par
         *      Additionally, before Yoga 3.2.0, changes to the configuration
         *      may not properly trigger recalculation of existing layouts.
         */
        YGConfigRef yogaConfig();
        YGConfigConstRef yogaConfig() const; /**< @overload */

        /**
         * @brief Add a layout assigned to given node
         * @param node      Node to assign the layout to
         * @param before    A layout to order before or
         *      @ref Ui::LayouterHandle::Null if ordered as last
         * @param flags     Flags
         * @return New layout handle
         *
         * The @p node is expected to not have a layout assigned from this
         * layouter yet. If @p before is not null, it's expected to be valid,
         * belong to the same layouter and have the same non-null parent layout
         * as @p node.
         *
         * If @p before is @ref Ui::LayoutHandle::Null, the operation is
         * performed in an amortized @f$ \mathcal{O}(1) @f$ complexity. If it's
         * non-null, the insertion is done in a @f$ \mathcal{O}(n) @f$
         * complexity where @f$ n @f$ is sibling layout count, due to a linear
         * lookup in the layouter and insertion in the middle of an array
         * inside Yoga.
         *
         * Delegates to @ref Ui::AbstractLayouter::add(), see its documentation
         * for more information.
         * @see @ref isHandleValid(Ui::LayoutHandle) const,
         *      @ref Ui::AbstractUserInterface::nodeParent(),
         *      @ref Ui::AbstractUserInterface::nodeUniqueLayout()
         */
        Ui::LayoutHandle add(Ui::NodeHandle node, Ui::LayoutHandle before =
            #ifdef DOXYGEN_GENERATING_OUTPUT
            Ui::LayoutHandle::Null,
            #else
            Ui::LayoutHandle{}, /* To not have to include Ui/Handle.h */
            #endif
            Flags flags = {}
        );
        /** @overload */
        Ui::LayoutHandle add(Ui::NodeHandle node, Flags flags) {
            return add(node, Ui::LayoutHandle{}, flags);
        }

        /**
         * @brief Add a layout assigned to given node and before given layout assuming the layout belongs to this layouter
         *
         * Like @ref add(Ui::NodeHandle, Ui::LayoutHandle, Flags) but without
         * checking that @p before indeed belongs to this layouter. See its
         * documentation for more information.
         * @see @ref isHandleValid(Ui::LayouterDataHandle) const,
         *      @ref layoutHandleData()
         */
        Ui::LayoutHandle add(Ui::NodeHandle node, Ui::LayouterDataHandle before, Flags flags = {});

        /**
         * @brief Remove a node from this layouter
         *
         * Expects that @p handle is valid and has no child layouts from the
         * same layouter. To remove a layout that has children, either remove
         * the child layouts first or remove the whole node along with its
         * children using @ref Ui::AbstractUserInterface::removeNode().
         *
         * Delegates to @ref Ui::AbstractLayouter::remove(), see its
         * documentation for more information.
         * @see @ref isHandleValid(Ui::LayoutHandle) const
         */
        void remove(Ui::LayoutHandle handle);

        /**
         * @brief Remove a node from this layouter assuming it belongs to it
         *
         * Like @ref remove(Ui::LayoutHandle) but without checking that
         * @p handle indeed belongs to this layouter. See its documentation for
         * more information.
         * @see @ref isHandleValid(Ui::LayouterDataHandle) const,
         *      @ref layoutHandleData()
         */
        void remove(Ui::LayouterDataHandle handle);

        /**
         * @brief Yoga node associated with given layout
         *
         * Expects that @p handle is valid.
         *
         * @m_class{m-note m-warning}
         *
         * @par
         *      Note that it's not easily possible for the layouter to detect
         *      changes made to Yoga nodes externally. If you're modifying the
         *      returned node directly using Yoga APIs, you need to call
         *      @ref setNeedsUpdate() afterwards to make the layouter aware of
         *      the changes and update the layout appropriately. For example:
         * @par
         *      @snippet YogaIntegration.cpp Layouter-yogaNode
         * @par
         *      It's allowed to modify any property except for the context
         *      pointer exposed via `YGNodeGetContext()` / `YGNodeSetContext()`,
         *      which is used internally by the layouter. Similarly, the parent
         *      / child hierarchy of Yoga nodes shouldn't be touched directly,
         *      only by manipulating the UI nodes the layouts are attached to.
         *
         * @see @ref isHandleValid(Ui::LayoutHandle) const
         */
        YGNodeRef yogaNode(Ui::LayoutHandle handle);
        YGNodeConstRef yogaNode(Ui::LayoutHandle handle) const; /**< @overload */

        /**
         * @brief Yoga node associated with given layout assuming it belongs to this layouter
         *
         * Like @ref yogaNode(Ui::LayoutHandle) but without checking that
         * @p handle indeed belongs to this layouter. See its documentation for
         * more information.
         * @see @ref isHandleValid(Ui::LayouterDataHandle) const,
         *      @ref layoutHandleData()
         */
        YGNodeRef yogaNode(Ui::LayouterDataHandle handle);
        YGNodeConstRef yogaNode(Ui::LayouterDataHandle handle) const; /**< @overload */

        /**
         * @brief Layout flags
         *
         * Expects that @p handle is valid. Note that, to reduce implementation
         * complexity, the flags can be only specified in @ref add() and cannot
         * be modified afterwards.
         * @see @ref isHandleValid(Ui::LayoutHandle) const
         */
        Flags flags(Ui::LayoutHandle handle) const;

        /**
         * @brief Layout flags assuming it belongs to this layouter
         *
         * Like @ref flags(Ui::LayoutHandle) const but without checking that
         * @p handle indeed belongs to this layouter. See its documentation for
         * more information.
         * @see @ref isHandleValid(Ui::LayouterDataHandle) const,
         *      @ref layoutHandleData()
         */
        Flags flags(Ui::LayouterDataHandle handle) const;

        /**
         * @brief Layout flex direction
         *
         * Expects that @p handle is valid.
         * @see @ref isHandleValid(Ui::LayoutHandle) const
         */
        FlexDirection flexDirection(Ui::LayoutHandle handle) const;

        /**
         * @brief Layout flex direction assuming it belongs to this layouter
         *
         * Like @ref flexDirection(Ui::LayoutHandle) const but without checking
         * that @p handle indeed belongs to this layouter. See its
         * documentation for more information.
         * @see @ref isHandleValid(Ui::LayouterDataHandle) const,
         *      @ref layoutHandleData()
         */
        FlexDirection flexDirection(Ui::LayouterDataHandle handle) const;

        /**
         * @brief Set layout flex direction
         *
         * Corresponds to the [CSS `flex-direction` property](https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Properties/flex-direction),
         * see the linked documentation for detailed explanation. Expects that
         * @p handle is valid. Default is @ref FlexDirection::Column, which
         * matches behavior of @cb{.css} flex-direction: column; @ce in CSS.
         *
         * Calling this function causes @ref Ui::LayouterState::NeedsUpdate to
         * be set.
         * @see @ref isHandleValid(Ui::LayoutHandle) const
         */
        void setFlexDirection(Ui::LayoutHandle handle, FlexDirection direction);

        /**
         * @brief Set layout flex direction assuming it belongs to this layouter
         *
         * Like @ref setFlexDirection(Ui::LayoutHandle, FlexDirection) but
         * without checking that @p handle indeed belongs to this layouter. See
         * its documentation for more information.
         * @see @ref isHandleValid(Ui::LayouterDataHandle) const,
         *      @ref layoutHandleData()
         */
        void setFlexDirection(Ui::LayouterDataHandle handle, FlexDirection direction);

        /**
         * @brief Layout node offset type
         *
         * Expects that @p handle is valid.
         * @see @ref isHandleValid(Ui::LayoutHandle) const
         */
        NodeOffsetType nodeOffsetType(Ui::LayoutHandle handle) const;

        /**
         * @brief Layout node offset type assuming it belongs to this layouter
         *
         * Like @ref nodeOffsetType(Ui::LayoutHandle) const but without
         * checking that @p handle indeed belongs to this layouter. See its
         * documentation for more information.
         * @see @ref isHandleValid(Ui::LayouterDataHandle) const,
         *      @ref layoutHandleData()
         */
        NodeOffsetType nodeOffsetType(Ui::LayouterDataHandle handle) const;

        /**
         * @brief Set layout node offset type
         *
         * Corresponds to the [CSS `position` property](https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Properties/position),
         * see the linked documentation for detailed explanation, Expects that
         * @p handle is valid. Default is @ref NodeOffsetType::Relative, which
         * matches behavior of @cb{.css} position: relative; @ce in CSS.
         *
         * Calling this function causes @ref Ui::LayouterState::NeedsUpdate to
         * be set.
         * @see @ref isHandleValid(Ui::LayoutHandle) const
         */
        void setNodeOffsetType(Ui::LayoutHandle handle, NodeOffsetType type);

        /**
         * @brief Set layout node offset type assuming it belongs to this layouter
         *
         * Like @ref setNodeOffsetType(Ui::LayoutHandle, NodeOffsetType) but
         * without checking that @p handle indeed belongs to this layouter. See
         * its documentation for more information.
         * @see @ref isHandleValid(Ui::LayouterDataHandle) const,
         *      @ref layoutHandleData()
         */
        void setNodeOffsetType(Ui::LayouterDataHandle handle, NodeOffsetType type);

    private:
        MAGNUM_YOGAINTEGRATION_LOCAL Ui::LayoutHandle addInternal(Ui::NodeHandle node, Ui::LayouterDataHandle before, Flags flags);
        MAGNUM_YOGAINTEGRATION_LOCAL void setFlexDirectionInternal(UnsignedInt id, FlexDirection direction);
        MAGNUM_YOGAINTEGRATION_LOCAL void setNodeOffsetTypeInternal(UnsignedInt id, NodeOffsetType type);

        MAGNUM_YOGAINTEGRATION_LOCAL Ui::LayouterFeatures doFeatures() const override;
        MAGNUM_YOGAINTEGRATION_LOCAL void doClean(Containers::BitArrayView layoutIdsToRemove) override;
        MAGNUM_YOGAINTEGRATION_LOCAL void doUpdate(Containers::BitArrayView layoutIdsToUpdate, const Containers::StridedArrayView1D<const UnsignedInt>& topLevelLayoutIds, const Containers::StridedArrayView1D<const Ui::NodeHandle>& nodeParents, const Containers::StridedArrayView1D<const Vector2>& nodeMinSizes, const Containers::StridedArrayView1D<const Vector2>& nodeMaxSizes, const Containers::StridedArrayView1D<const Float>& nodeAspectRatios, const Containers::StridedArrayView1D<const Vector4>& nodePaddings, const Containers::StridedArrayView1D<const Vector4>& nodeMargins, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override;

        struct State;
        Containers::Pointer<State> _state;
};

/**
@brief Debug layer integration

Integrates the layouter with @ref Ui::DebugLayer. See
@ref YogaIntegration-Layouter-debug-integration "Layouter debug layer integration"
for more information and example usage.
*/
class MAGNUM_YOGAINTEGRATION_EXPORT Layouter::DebugIntegration {
    public:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Used internally by DebugLayer, no point in documenting it here */
        void print(Debug& debug, const Layouter& layouter, const Containers::StringView& layouterName, Ui::LayouterDataHandle layout);
        #endif
};

}}

#endif
