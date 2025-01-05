#ifndef Magnum_EigenIntegration_DynamicMatrixIntegration_h
#define Magnum_EigenIntegration_DynamicMatrixIntegration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Janos <janos.meny@googlemail.com>

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
@brief Conversion of Eigen dynamically sized matrix types
@m_since_latest_{integration}

In addition to converting betweem fixed-size types provided by
@ref Magnum/EigenIntegration/Integration.h, which always involves a copy, it's
possible to make @relativeref{Corrade,Containers::StridedArrayView}
instances point to Eigen types and, conversely, an @m_class{m-doc-external} [Eigen::Map](https://eigen.tuxfamily.org/dox/classEigen_1_1Map.html)
from any @relativeref{Corrade,Containers::StridedArrayView1D} or
@relativeref{Corrade::Containers,StridedArrayView2D} using
@ref EigenIntegration::arrayCast():

@snippet EigenIntegration.cpp DynamicMatrixIntegration

@see @ref types-thirdparty-integration
*/

#include <Corrade/Containers/StridedArrayView.h>
#include <Magnum/Magnum.h>

#include <Eigen/Core>

namespace Magnum { namespace EigenIntegration {

/**
@brief Convert a @relativeref{Corrade,Containers::StridedArrayView2D} to Eigen's dynamic matrix type
@m_since_latest
*/
template<class T> inline Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Unaligned, Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>> arrayCast(const Containers::StridedArrayView2D<T>& from) {
    const Containers::StridedDimensions<2, std::size_t> size = from.size();
    const Containers::StridedDimensions<2, std::ptrdiff_t> stride = from.stride();
    return Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Unaligned, Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>>(
        reinterpret_cast<T*>(from.data()), size[0], size[1],
        /* This is strange, but correct -- while the size is (rows, cols), the
           stride is (inner, outer) where inner is pointer increment between
           two consecutive entries within a given column of a column-major
           matrix (so our column stride) and outer is pointer increment between two consecutive columns of a column-major matrix (so our row
           stride): https://eigen.tuxfamily.org/dox/classEigen_1_1Stride.html

           Alternatively we could define the Matrix as RowMajor and then those
           two would be swapped, but that would make the output type even
           longer (right now it's usually "just" something like

            Eigen::Map<Eigen::MatrixXf, Eigen::Unaligned, Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>>

           or

            Eigen::Map<Eigen::MatrixXd, Eigen::Unaligned, Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>>

           in user code, which isn't *that* bad compared to a fully expanded
           Eigen::Matrix type. According to https://eigen.tuxfamily.org/dox/group__TutorialMapClass.html#TutorialMapTypes,
           (see the StrideType example snippet), these two approaches are
           equivalent. */
        {Eigen::Index(stride[1]/sizeof(T)),
         Eigen::Index(stride[0]/sizeof(T))});
}

/**
@brief Convert a @relativeref{Corrade,Containers::StridedArrayView1D} to Eigen's dynamic vector type
@m_since_latest

Since for a one-dimensional @relativeref{Corrade,Containers::StridedArrayView}
there is no column or row version, we always return an Eigen column vector.
*/
template<class T> inline Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Unaligned, Eigen::InnerStride<>> arrayCast(const Containers::StridedArrayView1D<T>& from) {
    return Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Unaligned, Eigen::InnerStride<>>(reinterpret_cast<T*>(from.data()), from.size(), Eigen::InnerStride<>(from.stride()/sizeof(T)));
}

/**
@brief Convert an Eigen expression to @relativeref{Corrade,Containers::StridedArrayView1D}
@m_since_latest

If it is known at compile time that the Eigen expression has either one column
or one row, then this function maps the Eigen expression to a
@relativeref{Corrade,Containers::StridedArrayView1D}. Otherwise the overload
below is picked, returning a @relativeref{Corrade,Containers::StridedArrayView2D}.
*/
template<class Derived> inline typename std::enable_if<
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Eigen::internal::traits<Derived>::ColsAtCompileTime == 1 || Eigen::internal::traits<Derived>::RowsAtCompileTime == 1
    #else
    ...
    #endif
, Containers::StridedArrayView1D<typename Derived::Scalar>>::type arrayCast(const Eigen::DenseCoeffsBase<Derived, Eigen::DirectWriteAccessors>& from) {
    return {
        /* We assume that the memory the Eigen expression is referencing is in
           bounds, so the view size passed is ~std::size_t{} */
        {const_cast<typename Derived::Scalar*>(&from(0,0)), ~std::size_t{}},
        std::size_t(from.size()),
        std::ptrdiff_t(from.innerStride()*sizeof(typename Derived::Scalar))
    };
}

/**
@brief Convert an Eigen expression to @relativeref{Corrade,Containers::StridedArrayView2D}
@m_since_latest

Takes care of any Eigen expression that was not handled by the one-dimensional
overload above.
*/
template<class Derived> inline typename std::enable_if<
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Eigen::internal::traits<Derived>::ColsAtCompileTime == Eigen::Dynamic && Eigen::internal::traits<Derived>::RowsAtCompileTime == Eigen::Dynamic
    #else
    ...
    #endif
, Containers::StridedArrayView2D<typename Derived::Scalar>>::type arrayCast(const Eigen::DenseCoeffsBase<Derived, Eigen::DirectWriteAccessors>& from) {
    return {
        /* We assume that the memory the Eigen expression is referencing is in
           bounds, so the view size passed is ~std::size_t{} */
        {const_cast<typename Derived::Scalar*>(&from(0,0)), ~std::size_t{}}, {std::size_t(from.rows()),
         std::size_t(from.cols())},
        {std::ptrdiff_t(from.rowStride()*sizeof(typename Derived::Scalar)),
         std::ptrdiff_t(from.colStride()*sizeof(typename Derived::Scalar))}
    };
}

/**
@brief Convert an Eigen reverse expression to @relativeref{Corrade,Containers::StridedArrayView1D}
@m_since_latest

If it is known at compile time that the Eigen reverse expression has either one
column or one row, then this function maps the Eigen reverse expression to a
@relativeref{Corrade,Containers::StridedArrayView1D}. Otherwise the overload
below is picked, returning a @relativeref{Corrade,Containers::StridedArrayView2D}.
*/
template<class Derived, int Direction> inline typename std::enable_if<
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Eigen::internal::traits<Derived>::ColsAtCompileTime == 1 || Eigen::internal::traits<Derived>::RowsAtCompileTime == 1
    #else
    ...
    #endif
, Containers::StridedArrayView1D<typename Derived::Scalar>>::type arrayCast(const Eigen::Reverse<Derived, Direction>& from) {
    constexpr bool isColVector =
        Eigen::internal::traits<Derived>::ColsAtCompileTime == 1;
    constexpr bool isRowVector =
        Eigen::internal::traits<Derived>::RowsAtCompileTime == 1;
    constexpr Int reverse = (Direction == Eigen::BothDirections ||
        (Direction == Eigen::Vertical && isRowVector) ||
        (Direction == Eigen::Horizontal && isColVector)) ? -1 : 1;

    const auto& nested = from.nestedExpression();
    return {
        /* We assume that the memory the Eigen expression is referencing is in
           bounds, so the view size passed is ~std::size_t{} */
        {const_cast<typename Derived::Scalar*>(&from(0,0)), ~std::size_t{}},
        std::size_t(nested.size()),
        std::ptrdiff_t(nested.innerStride()*reverse*sizeof(typename Derived::Scalar))
    };
}

/**
@brief Convert an Eigen reverse expression to @relativeref{Corrade,Containers::StridedArrayView2D}
@m_since_latest

Takes care of any Eigen expression that was not handled by the one-dimensional
overload above.
*/
template<class Derived, int Direction> inline typename std::enable_if<
    #ifndef DOXYGEN_GENERATING_OUTPUT
    Eigen::internal::traits<Derived>::ColsAtCompileTime == Eigen::Dynamic && Eigen::internal::traits<Derived>::RowsAtCompileTime == Eigen::Dynamic
    #else
    ...
    #endif
, Containers::StridedArrayView2D<typename Derived::Scalar>>::type arrayCast(const Eigen::Reverse<Derived, Direction>& from) {
    constexpr Int reverseCol = (Direction == Eigen::Vertical ||
                                Direction == Eigen::BothDirections) ? -1 : 1;
    constexpr Int reverseRow = (Direction == Eigen::Horizontal ||
                                Direction == Eigen::BothDirections) ? -1 : 1;

    const auto& nested = from.nestedExpression();
    return {
        /* We assume that the memory the Eigen expression is referencing is in
           bounds, so the view size passed is ~std::size_t{} */
        {const_cast<typename Derived::Scalar*>(&from(0, 0)), ~std::size_t{}},
        {std::size_t(nested.rows()), std::size_t(nested.cols())},
        {std::ptrdiff_t(sizeof(typename Derived::Scalar))*nested.rowStride()*reverseCol,
         std::ptrdiff_t(sizeof(typename Derived::Scalar))*nested.colStride()*reverseRow}
    };
}

}

}

#endif
