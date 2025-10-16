#ifndef Magnum_YogaIntegration_Layouter_h
#define Magnum_YogaIntegration_Layouter_h
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

/** @file
 * @brief Class @ref Magnum::YogaIntegration::Layouter
 * @m_since_latest_{integration}
 */

#include <Magnum/Ui/AbstractLayouter.h>
#include <yoga/Yoga.h>

#include "Magnum/YogaIntegration/visibility.h"

namespace Magnum { namespace YogaIntegration {

/**
@brief Yoga layouter
@m_since_latest_{integration}
*/
class MAGNUM_YOGAINTEGRATION_EXPORT Layouter: public Ui::AbstractLayouter {
    public:
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

        Ui::LayoutHandle add(Ui::NodeHandle node, Ui::LayoutHandle parent =
            #ifdef DOXYGEN_GENERATING_OUTPUT
            Ui::LayoutHandle::Null
            #else
            Ui::LayoutHandle{} /* To not have to include Ui/Handle.h */
            #endif
        );

        YGNodeRef yogaNode(Ui::LayoutHandle layout) const;
        YGNodeRef yogaNode(Ui::LayouterDataHandle layout) const;

    private:
        MAGNUM_YOGAINTEGRATION_LOCAL void doUpdate(Containers::BitArrayView layoutIdsToUpdate, const Containers::StridedArrayView1D<const UnsignedInt>& topLevelLayoutIds, const Containers::StridedArrayView1D<const Ui::NodeHandle>& nodeParents, const Containers::StridedArrayView1D<Vector2>& nodeOffsets, const Containers::StridedArrayView1D<Vector2>& nodeSizes) override;

        struct State;
        Containers::Pointer<State> _state;
};

}}

#endif
