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

#include <Magnum/Math/Vector2.h>
#include <Magnum/Ui/Handle.h>
#include <Magnum/Ui/AbstractUserInterface.h>
#include <yoga/Yoga.h>

#include "Magnum/YogaIntegration/Layouter.h"

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;

/* Make sure the name doesn't conflict with any other snippets to avoid linker
   warnings, unlike with `int main()` there now has to be a declaration to
   avoid -Wmisssing-prototypes */
void mainYogaIntegration();
void mainYogaIntegration() {
{
Ui::AbstractUserInterface ui{{100, 100}};
/* [Layouter-setup] */
YogaIntegration::Layouter& layouter = ui.setLayouterInstance(
    Containers::pointer<YogaIntegration::Layouter>(ui.createLayouter()));
/* [Layouter-setup] */
static_cast<void>(layouter);
}

{
YogaIntegration::Layouter layouter{Ui::layouterHandle(0, 1)};
/* The include is already above, so doing it again here should be harmless */
/* [Layouter-direct] */
#include <yoga/Yoga.h>

DOXYGEN_ELLIPSIS()

Ui::LayoutHandle layout = layouter.add(DOXYGEN_ELLIPSIS({}));
YGNodeStyleSetFlexWrap(layouter.yogaNode(layout), YGWrapWrapReverse);
layouter.setNeedsUpdate();
/* [Layouter-direct] */
}

{
YogaIntegration::Layouter layouter{Ui::layouterHandle(0, 1)};
/* [Layouter-yogaConfig] */
YGConfigSetPointScaleFactor(layouter.yogaConfig(), 1.0f);
layouter.setNeedsUpdate();
/* [Layouter-yogaConfig] */
}

{
YogaIntegration::Layouter layouter{Ui::layouterHandle(0, 1)};
/* [Layouter-yogaNode] */
Ui::LayoutHandle layout = layouter.add(DOXYGEN_ELLIPSIS({}));
YGNodeStyleSetFlexWrap(layouter.yogaNode(layout), YGWrapWrapReverse);
layouter.setNeedsUpdate();
/* [Layouter-yogaNode] */
}
}
