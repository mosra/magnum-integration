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

#include <Corrade/Containers/Function.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Magnum/Image.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderer.h>
#ifdef MAGNUM_TARGET_EGL
#include <Magnum/Platform/WindowlessEglApplication.h>
#elif defined(CORRADE_TARGET_APPLE)
#include <Magnum/Platform/WindowlessCglApplication.h>
#elif defined(CORRADE_TARGET_UNIX)
#include <Magnum/Platform/WindowlessGlxApplication.h>
#elif defined(CORRADE_TARGET_WINDOWS)
#include <Magnum/Platform/WindowlessWglApplication.h>
#else
#error no windowless application available on this platform
#endif
#include <Magnum/Trade/AbstractImageConverter.h>
#include <Magnum/Ui/DebugLayerGL.h>
#include <Magnum/Ui/Handle.h>
#include <Magnum/Ui/RendererGL.h>
#include <Magnum/Ui/UserInterfaceGL.h>

#include "Magnum/YogaIntegration/Layouter.h"

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;
using namespace Math::Literals;

namespace {

struct UiDebugLayer: Platform::WindowlessApplication {
    explicit UiDebugLayer(const Arguments& arguments): Platform::WindowlessApplication{arguments} {}

    int exec() override;
};

constexpr Vector2i ImageSize{480, 330};

/** @todo ffs, duplicated with extras, make a batch utility in Magnum */
Image2D unpremultiply(Image2D image) {
    for(Containers::StridedArrayView1D<Color4ub> row: image.pixels<Color4ub>())
        for(Color4ub& pixel: row)
            pixel = pixel.unpremultiplied();

    return image;
}

int UiDebugLayer::exec() {
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::One, GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    PluginManager::Manager<Trade::AbstractImageConverter> converterManager;
    Containers::Pointer<Trade::AbstractImageConverter> converter = converterManager.loadAndInstantiate("AnyImageConverter");
    if(!converter)
        return 1;

    Ui::UserInterfaceGL ui{NoCreate};
    /* Using a compositing framebuffer because it's easier than setting up a
       custom framebuffer here */
    ui
        /** @todo uh, can't setting renderer flags be doable in some more
            intuitive way? such as flags on the style? */
        .setRendererInstance(Containers::pointer<Ui::RendererGL>(Ui::RendererGL::Flag::CompositingFramebuffer))
        /* The actual framebuffer size is 3x the UI size */
        .setSize({160.0f, 110.0f}, Vector2{ImageSize}, ImageSize);

    /* Inflate the node capacity so we have a consistent colormap for the node
       highlight in all cases below. Just allocate and then remove all nodes,
       assuming their handles are trivial like this. */
    /** @todo clean up once some reserveNodeCapacity() is a thing? */
    constexpr UnsignedInt nodeCapacity = 9;
    for(UnsignedInt i = 0; i != nodeCapacity; ++i)
        ui.createNode({}, {});
    for(UnsignedInt i = 0; i != nodeCapacity; ++i)
        ui.removeNode(Ui::nodeHandle(i, 1));
    CORRADE_INTERNAL_ASSERT(ui.nodeUsedCount() == 0);

    Ui::DebugLayer& debugLayer = ui.setLayerInstance(Containers::pointer<Ui::DebugLayerGL>(ui.createLayer(), Ui::DebugLayerSource::Nodes|Ui::DebugLayerSource::Layouters, Ui::DebugLayerFlags{}));
    debugLayer.setNodeHighlightColorMap(DebugTools::ColorMap::turbo(), 0.5f);

    YogaIntegration::Layouter& layouter = ui.setLayouterInstance(Containers::pointer<YogaIntegration::Layouter>(ui.createLayouter()));

/* [Layouter-add] */
Ui::NodeHandle root = ui.createNode({}, {});
Ui::NodeHandle header = ui.createNode(root, {}, {150, 10});
Ui::NodeHandle top = ui.createNode(root, {}, {150, 30});
Ui::NodeHandle bottom = ui.createNode(root, {}, {150, 20});

layouter.add(root);
layouter.add(header);
layouter.add(top);
Ui::LayoutHandle bottomLayout = layouter.add(bottom);
/* [Layouter-add] */

    /* Move the layout to the center so it looks nice in the image */
    /** @todo clean up once it automatically centers in the UI */
    ui.setNodeOffset(root, {5, 5});

    /* Make DebugLayer aware of all nodes and highlight nodes that have a
       layout assigned */
    ui.update();
    debugLayer.highlightNodes(layouter, [](const YogaIntegration::Layouter&, Ui::LayouterDataHandle) {
        return true;
    });

    ui.renderer().compositingFramebuffer().clearColor(0, 0x00000000_rgbaf);
    ui.draw();
    converter->convertToFile(unpremultiply(ui.renderer().compositingFramebuffer().read(Range2Di::fromSize({0, 115}, {480, 215}), {PixelFormat::RGBA8Unorm})), "yogaintegration-add.png");

/* [Layouter-add-insert-row] */
Ui::NodeHandle middle = ui.createNode(root, {}, {0, 40});
Ui::NodeHandle left = ui.createNode(middle, {}, {15, 0});
Ui::NodeHandle center = ui.createNode(middle, {}, {70, 0});
Ui::NodeHandle right = ui.createNode(middle, {}, {15, 0});

/* Insert the middle node before the bottom one */
Ui::LayoutHandle middleLayout = layouter.add(middle, bottomLayout);
layouter.setFlexDirection(middleLayout, YogaIntegration::FlexDirection::Row);
layouter.add(left, YogaIntegration::Flag::PercentageNodeSize);
layouter.add(center, YogaIntegration::Flag::PercentageNodeSize);
layouter.add(right, YogaIntegration::Flag::PercentageNodeSize);
/* [Layouter-add-insert-row] */

    /* Again nake DebugLayer aware of the new nodes and highlight */
    ui.update();
    debugLayer.highlightNodes(layouter, [](const YogaIntegration::Layouter&, Ui::LayouterDataHandle) {
        return true;
    });

    ui.renderer().compositingFramebuffer().clearColor(0, 0x00000000_rgbaf);
    ui.draw();
    converter->convertToFile(unpremultiply(ui.renderer().compositingFramebuffer().read(Range2Di::fromSize({0, 0}, {480, 330}), {PixelFormat::RGBA8Unorm})), "yogaintegration-add-insert-row.png");

/* [Layouter-add-bottom-right] */
Ui::NodeHandle footer = ui.createNode(bottom, {5, 5}, {60, 10});
Ui::LayoutHandle footerLayout = layouter.add(footer,
    YogaIntegration::Flag::NodeOffsetFromRightBottom);
layouter.setNodeOffsetType(footerLayout, YogaIntegration::NodeOffsetType::Absolute);
/* [Layouter-add-bottom-right] */

    /* Again nake DebugLayer aware of the new nodes and highlight */
    ui.update();
    debugLayer.highlightNodes(layouter, [](const YogaIntegration::Layouter&, Ui::LayouterDataHandle) {
        return true;
    });

    ui.renderer().compositingFramebuffer().clearColor(0, 0x00000000_rgbaf);
    ui.draw();
    converter->convertToFile(unpremultiply(ui.renderer().compositingFramebuffer().read(Range2Di::fromSize({0, 0}, {480, 330}), {PixelFormat::RGBA8Unorm})), "yogaintegration-add-bottom-right.png");

    CORRADE_INTERNAL_ASSERT(ui.nodeCapacity() == nodeCapacity && ui.nodeUsedCount() == nodeCapacity);

    return 0;
}

}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(UiDebugLayer)
