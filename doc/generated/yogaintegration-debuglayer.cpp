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

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Path.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Ui/AbstractUserInterface.h>
#include <Magnum/Ui/DebugLayer.h>

#include "Magnum/YogaIntegration/Layouter.h"

using namespace Magnum;

int main() {
    Ui::AbstractUserInterface ui{{100, 100}};

    Ui::DebugLayer& debugLayer = ui.setLayerInstance(Containers::pointer<Ui::DebugLayer>(ui.createLayer(), Ui::DebugLayerSource::NodeLayoutDetails|Ui::DebugLayerSource::NodeOffsetSize, Ui::DebugLayerFlag::NodeInspect|Ui::DebugLayerFlag::ColorAlways));

    /* Adding some extra layouters to have the listed handle non-trivial */
    ui.createLayouter();
    ui.removeLayouter(ui.createLayouter());
    YogaIntegration::Layouter& layouter = ui.setLayouterInstance(Containers::pointer<YogaIntegration::Layouter>(ui.createLayouter()));
    debugLayer.setLayouterName(layouter, "Yoga");

    /* Adding some extra nodes and layouts to have the listed handles
       non-trivial */
    ui.createNode({}, {});
    ui.createNode({}, {});
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.add(ui.createNode({}, {}));
    layouter.remove(layouter.add(ui.createNode({}, {})));
    layouter.remove(layouter.add(ui.createNode({}, {})));
    Ui::NodeHandle root = ui.createNode({}, {});
    ui.removeNode(ui.createNode({}, {}));
    Ui::NodeHandle node = ui.createNode(root, {15.0f, 5.0f}, {75.0f, 0.0f});
    Ui::LayoutHandle layout = layouter.add(node, YogaIntegration::Flag::NodeOffsetFromRight|YogaIntegration::Flag::PercentageNodeSizeX);
    layouter.setFlexDirection(layout, YogaIntegration::FlexDirection::Row);
    layouter.setNodeOffsetType(layout, YogaIntegration::NodeOffsetType::Absolute);

    ui.update();

    Containers::String out;
    {
        Debug redirectOutput{&out};
        CORRADE_INTERNAL_ASSERT(debugLayer.inspectNode(node));
    }
    Debug{} << out;
    Utility::Path::write("yogaintegration-debuglayer.ansi", out);
}
