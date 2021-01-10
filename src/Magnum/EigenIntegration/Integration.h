#ifndef Magnum_EigenIntegration_Integration_h
#define Magnum_EigenIntegration_Integration_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>
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
@brief Conversion of Eigen array and matrix types
@m_since_{integration,2019,10}

@section EigenIntegration-math Math type conversion

Provides conversion for the following fixed-size types. See
@ref Magnum/EigenIntegration/GeometryIntegration.h for conversion of special
geometry types and @ref EigenIntegration-stridedarrayview for conversion of
dynamically sized types.

| Magnum vector type                             | Equivalent Eigen type    |
| ---------------------------------------------- | ------------------------ |
| @ref Magnum::Math::BoolVector "Math::BoolVector<size>" | @m_class{m-doc-external} [Eigen::Array<bool, size, 1>](https://eigen.tuxfamily.org/dox/classEigen_1_1Array.html) |
| @ref Magnum::Math::Vector "Math::Vector<size, T>" and derived classes | @m_class{m-doc-external} [Eigen::Array<T, size, 1>](https://eigen.tuxfamily.org/dox/classEigen_1_1Array.html) |
| @ref Magnum::Math::Vector "Math::Vector<size, T>" and derived classes | @m_class{m-doc-external} [Eigen::Matrix<T, size, 1>](https://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html) |

| Magnum matrix type                             | Equivalent Eigen type    |
| ---------------------------------------------- | ------------------------ |
| @ref Magnum::Math::RectangularMatrix "Math::RectangularMatrix<cols, rows, T>" and derived classes | @m_class{m-flat} [Eigen::Array<T, rows, cols>](https://eigen.tuxfamily.org/dox/classEigen_1_1Array.html) |
| @ref Magnum::Math::RectangularMatrix "Math::RectangularMatrix<cols, rows, T>" and derived classes | @m_class{m-flat} [Eigen::Matrix<T, rows, cols>](https://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html) |

@m_class{m-block m-warning}

@par Conversion to Eigen types
    While creating Magnum types from Eigen types is done via the usual explicit
    conversion, unlike with other libraries such as @ref Magnum::GlmIntegration "GLM"
    the other direction has to be done a different way. Due to both
    @m_class{m-doc-external} [Eigen::Array](https://eigen.tuxfamily.org/dox/classEigen_1_1Array.html)
    and @m_class{m-doc-external} [Eigen::Matrix](https://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html)
    having an [implicit all-catching constructor](https://github.com/eigenteam/eigen-git-mirror/blob/28728b910ed1e280aad4a4c9c46ef4ae2dddccc7/Eigen/src/Core/Array.h#L165-L172),
    implementing such a conversion on Magnum side is not possible. To work
    around that, there's a special
    @ref Magnum::EigenIntegration::cast() "EigenIntegration::cast()" function
    that does the job instead. See the snippet below for an usage example.
@par
    This restriction does not apply to the geometry types such as
    @m_class{m-doc-external} [Eigen::Quaternion](https://eigen.tuxfamily.org/dox/classEigen_1_1Quaternion.html)
    or @m_class{m-doc-external} [Eigen::Transform](https://eigen.tuxfamily.org/dox/classEigen_1_1Transform.html).

@cpp Eigen::Array @ce and @cpp Eigen::Matrix @ce classes provide a
@ref std::iostream @cpp operator<< @ce overload, which means they're directly
usable with @ref Corrade::Utility::Debug if you include
@ref Corrade/Utility/DebugStl.h as well. Example usage:

@snippet EigenIntegration.cpp Integration

@section EigenIntegration-stridedarrayview Multidimensional strided array view conversion

In addition to converting betweem fixed-size types, which always involves a
copy, it's possible to make @ref Corrade::Containers::StridedArrayView "Containers::StridedArrayView"
instances point to Eigen types and, conversely, an @m_class{m-doc-external} [Eigen::Map](https://eigen.tuxfamily.org/dox/classEigen_1_1Map.html)
from any @ref Containers::StridedArrayView1D or @ref Containers::StridedArrayView2D "StridedArrayView2D" using
@ref EigenIntegration::arrayCast():

@snippet EigenIntegration.cpp Integration-arrayCast

@see @ref types-thirdparty-integration
*/

/* I would usually frown upon unconditionally including this relatively huge
   header into a widely-used utility header, but it's *Eigen* we're dealing
   with -- no amount of Magnum's code can compete with Eigen's sky-high compile
   times. */
#include <Corrade/Containers/StridedArrayView.h>

#include <Magnum/Magnum.h>
#include <Magnum/Math/RectangularMatrix.h>
#include <Eigen/Core>

namespace Magnum {

namespace Math { namespace Implementation {

/* This is extremely awful in order to support T, Eigen::Ref<const T> and
   Eigen::Ref<T> for all types, further made worse by workarounds for MSVC and
   GCC. Sorry for your bleeding eyes. Take a shower after. */

template<std::size_t size> struct BoolVectorConverter<size, Eigen::Ref<const Eigen::Array<bool, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    /* Otherwise neither MSVC 2015, 2017 nor 2019 is able to match the signature */
    , 0, int(size), 1
    #endif
>>> {
    static BoolVector<size> from(const Eigen::Ref<const Eigen::Array<bool, size, 1>>& other) {
        BoolVector<size> out; /* NoInit prints a warning on GCC 9, don't */
        for(std::size_t i = 0; i != size; ++i)
            out.set(i, other(i, 0));
        return out;
    }
};
template<std::size_t size> struct BoolVectorConverter<size, Eigen::Ref<Eigen::Array<bool, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    /* Otherwise neither MSVC 2015, 2017 nor 2019 is able to match the signature */
    , 0, int(size), 1
    #endif
>>>: BoolVectorConverter<size, Eigen::Ref<const Eigen::Array<bool, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    /* Otherwise neither MSVC 2015, 2017 nor 2019 is able to match the signature */
    , 0, int(size), 1
    #endif
>>> {};
template<std::size_t size> struct BoolVectorConverter<size, Eigen::Array<bool, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    /* Otherwise neither MSVC 2015, 2017 nor 2019 is able to match the signature */
    , 0, int(size), 1
    #endif
>> {
    static BoolVector<size> from(const Eigen::Array<bool, size, 1>& other) {
        #if defined(__GNUC__) && !defined(__clang__)
        /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
           here. We don't care one bit, so silence that. Clang doesn't have
           this warning enabled in UseCorrade.cmake. */
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
        #endif
        return BoolVectorConverter<size, Eigen::Ref<const Eigen::Array<bool, int(size), 1>>>::from(other);
        #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic pop
        #endif
    }

    static Eigen::Array<bool, size, 1> to(const BoolVector<size>& other) {
        /** @todo is there a NoInit tag or something? */
        Eigen::Array<bool, size, 1> out;
        for(std::size_t i = 0; i != size; ++i)
            out(i, 0) = other[i];
        return out;
    }
};

template<std::size_t size, class T> struct VectorConverter<size, T, Eigen::Ref<const Eigen::Array<T, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(size), 1 /* See above */
    #endif
>>> {
    static Vector<size, T> from(const Eigen::Ref<const Eigen::Array<T, size, 1>>& other) {
        Vector<size, T> out{Magnum::NoInit};
        for(std::size_t i = 0; i != size; ++i)
            out[i] = other(i, 0);
        return out;
    }
};
template<std::size_t size, class T> struct VectorConverter<size, T, Eigen::Ref<Eigen::Array<T, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(size), 1 /* See above */
    #endif
>>>: VectorConverter<size, T, Eigen::Ref<const Eigen::Array<T, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(size), 1 /* See above */
    #endif
>>> {};
template<std::size_t size, class T> struct VectorConverter<size, T, Eigen::Array<T, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(size), 1 /* See above */
    #endif
>> {
    static Vector<size, T> from(const Eigen::Array<T, size, 1>& other) {
        #if defined(__GNUC__) && !defined(__clang__)
        /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
           here. We don't care one bit, so silence that. Clang doesn't have
           this warning enabled in UseCorrade.cmake. */
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
        #endif
        return VectorConverter<size, T, Eigen::Ref<const Eigen::Array<T, int(size), 1>>>::from(other);
        #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic pop
        #endif
    }

    static Eigen::Array<T, size, 1> to(const Vector<size, T>& other) {
        /** @todo is there a NoInit tag or something? */
        Eigen::Array<T, size, 1> out;
        for(std::size_t i = 0; i != size; ++i)
            out(i, 0) = other[i];
        return out;
    }
};

template<std::size_t size, class T> struct VectorConverter<size, T, Eigen::Ref<const Eigen::Matrix<T, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(size), 1 /* See above */
    #endif
>>> {
    static Vector<size, T> from(const Eigen::Ref<const Eigen::Matrix<T, size, 1>>& other) {
        Vector<size, T> out{Magnum::NoInit};
        for(std::size_t i = 0; i != size; ++i)
            out[i] = other(i, 0);
        return out;
    }
};
template<std::size_t size, class T> struct VectorConverter<size, T, Eigen::Ref<Eigen::Matrix<T, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(size), 1 /* See above */
    #endif
>>>: VectorConverter<size, T, Eigen::Ref<const Eigen::Matrix<T, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(size), 1 /* See above */
    #endif
>>> {};
template<std::size_t size, class T> struct VectorConverter<size, T, Eigen::Matrix<T, int(size), 1
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(size), 1 /* See above */
    #endif
>> {
    static Vector<size, T> from(const Eigen::Matrix<T, size, 1>& other) {
        #if defined(__GNUC__) && !defined(__clang__)
        /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
           here. We don't care one bit, so silence that. Clang doesn't have
           this warning enabled in UseCorrade.cmake. */
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
        #endif
        return VectorConverter<size, T, Eigen::Ref<const Eigen::Matrix<T, int(size), 1>>>::from(other);
        #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic pop
        #endif
    }

    static Eigen::Matrix<T, size, 1> to(const Vector<size, T>& other) {
        /** @todo is there a NoInit tag or something? */
        Eigen::Matrix<T, size, 1> out;
        for(std::size_t i = 0; i != size; ++i)
            out(i, 0) = other[i];
        return out;
    }
};

template<std::size_t cols, std::size_t rows, class T> struct RectangularMatrixConverter<cols, rows, T, Eigen::Ref<const Eigen::Array<T, int(rows), int(cols)
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(rows), int(cols) /* See above */
    #endif
>>> {
    static RectangularMatrix<cols, rows, T> from(const Eigen::Ref<const Eigen::Array<T, rows, cols>>& other) {
        RectangularMatrix<cols, rows, T> out{Magnum::NoInit};
        for(std::size_t col = 0; col != cols; ++col)
            for(std::size_t row = 0; row != rows; ++row)
                out[col][row] = other(row, col);
        return out;
    }
};
template<std::size_t cols, std::size_t rows, class T> struct RectangularMatrixConverter<cols, rows, T, Eigen::Ref<Eigen::Array<T, int(rows), int(cols)
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(rows), int(cols) /* See above */
    #endif
>>>: RectangularMatrixConverter<cols, rows, T, Eigen::Ref<const Eigen::Array<T, int(rows), int(cols)
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(rows), int(cols) /* See above */
    #endif
>>> {};
template<std::size_t cols, std::size_t rows, class T> struct RectangularMatrixConverter<cols, rows, T, Eigen::Array<T, int(rows), int(cols)
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(rows), int(cols) /* See above */
    #endif
>> {
    static RectangularMatrix<cols, rows, T> from(const Eigen::Array<T, rows, cols>& other) {
        #if defined(__GNUC__) && !defined(__clang__)
        /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
           here. We don't care one bit, so silence that. Clang doesn't have
           this warning enabled in UseCorrade.cmake. */
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
        #endif
        return RectangularMatrixConverter<cols, rows, T, Eigen::Ref<const Eigen::Array<T, int(rows), int(cols)>>>::from(other);
        #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic pop
        #endif
    }

    static Eigen::Array<T, rows, cols> to(const RectangularMatrix<cols, rows, T>& other) {
        /** @todo is there a NoInit tag or something? */
        Eigen::Array<T, rows, cols> out;
        for(std::size_t col = 0; col != cols; ++col)
            for(std::size_t row = 0; row != rows; ++row)
                out(row, col) = other[col][row];
        return out;
    }
};

template<std::size_t cols, std::size_t rows, class T> struct RectangularMatrixConverter<cols, rows, T, Eigen::Ref<const Eigen::Matrix<T, int(rows), int(cols)
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(rows), int(cols) /* See above */
    #endif
>>> {
    static RectangularMatrix<cols, rows, T> from(const Eigen::Ref<const Eigen::Matrix<T, rows, cols>>& other) {
        RectangularMatrix<cols, rows, T> out{Magnum::NoInit};
        for(std::size_t col = 0; col != cols; ++col)
            for(std::size_t row = 0; row != rows; ++row)
                out[col][row] = other(row, col);
        return out;
    }
};
template<std::size_t cols, std::size_t rows, class T> struct RectangularMatrixConverter<cols, rows, T, Eigen::Ref<Eigen::Matrix<T, int(rows), int(cols)
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(rows), int(cols) /* See above */
    #endif
>>>: RectangularMatrixConverter<cols, rows, T, Eigen::Ref<const Eigen::Matrix<T, int(rows), int(cols)
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(rows), int(cols) /* See above */
    #endif
>>> {};
template<std::size_t cols, std::size_t rows, class T> struct RectangularMatrixConverter<cols, rows, T, Eigen::Matrix<T, int(rows), int(cols)
    #ifdef CORRADE_MSVC2019_COMPATIBILITY
    , 0, int(rows), int(cols) /* See above */
    #endif
>> {
    static RectangularMatrix<cols, rows, T> from(const Eigen::Matrix<T, rows, cols>& other) {
        #if defined(__GNUC__) && !defined(__clang__)
        /* There's T* = 0 in Eigen::Ref constructor but GCC insists on warning
           here. We don't care one bit, so silence that. Clang doesn't have
           this warning enabled in UseCorrade.cmake. */
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
        #endif
        return RectangularMatrixConverter<cols, rows, T, Eigen::Ref<const Eigen::Matrix<T, int(rows), int(cols)>>>::from(other);
        #if defined(__GNUC__) && !defined(__clang__)
        #pragma GCC diagnostic pop
        #endif
    }

    static Eigen::Matrix<T, rows, cols> to(const RectangularMatrix<cols, rows, T>& other) {
        /** @todo is there a NoInit tag or something? */
        Eigen::Matrix<T, rows, cols> out;
        for(std::size_t col = 0; col != cols; ++col)
            for(std::size_t row = 0; row != rows; ++row)
                out(row, col) = other[col][row];
        return out;
    }
};

}}

namespace EigenIntegration {

/**
@brief Convert a Magnum type to Eigen type
@m_since_{integration,2019,10}

Due to the design of @m_class{m-doc-external} [Eigen::Array](https://eigen.tuxfamily.org/dox/classEigen_1_1Array.html)
and @m_class{m-doc-external} [Eigen::Matrix](https://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html)
classes, it's not possible to use the usual explicit conversion approach. See
@ref Magnum/EigenIntegration/Integration.h for more information.
*/
template<class To, std::size_t cols, std::size_t rows, class T> inline To cast(const Math::RectangularMatrix<cols, rows, T>& from) {
    return Math::Implementation::RectangularMatrixConverter<cols, rows, T, To>::to(from);
}

/**
@overload
@m_since_{integration,2019,10}
*/
template<class To, std::size_t size> inline To cast(const Math::BoolVector<size>& from) {
    return Math::Implementation::BoolVectorConverter<size, To>::to(from);
}

/**
@overload
@m_since_{integration,2019,10}
*/
template<class To, std::size_t size, class T> inline To cast(const Math::Vector<size, T>& from) {
    return Math::Implementation::VectorConverter<size, T, To>::to(from);
}

/**
@brief Convert a @ref Containers::StridedArrayView2D to Eigen's dynamic matrix type
@m_since_latest

@see @ref EigenIntegration-stridedarrayview
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
@brief Convert a @ref Containers::StridedArrayView1D to Eigen's dynamic vector type
@m_since_latest

Since for a one-dimensional @ref Corrade::Containers::StridedArrayView "Containers::StridedArrayView"
there is no column or row version, we always return an Eigen column vector.
@see @ref EigenIntegration-stridedarrayview
*/
template<class T> inline Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Unaligned, Eigen::InnerStride<>> arrayCast(const Containers::StridedArrayView1D<T>& from) {
    return Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>, Eigen::Unaligned, Eigen::InnerStride<>>(reinterpret_cast<T*>(from.data()), from.size(), Eigen::InnerStride<>(from.stride()/sizeof(T)));
}

/**
@brief Convert an Eigen expression to @ref Containers::StridedArrayView1D
@m_since_latest

If it is known at compile time that the Eigen expression has either one column
or one row, then this function maps the Eigen expression to a
@ref Containers::StridedArrayView1D. Otherwise the overload below is picked,
returning a @ref Containers::StridedArrayView2D.
@see @ref EigenIntegration-stridedarrayview
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
@brief Convert an Eigen expression to @ref Containers::StridedArrayView2D
@m_since_latest

Takes care of any Eigen expression that was not handled by the one-dimensional
overload above.
@see @ref EigenIntegration-stridedarrayview
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
@brief Convert an Eigen reverse expression to @ref Containers::StridedArrayView1D
@m_since_latest

If it is known at compile time that the Eigen reverse expression has either one
column or one row, then this function maps the Eigen reverse expression to a
@ref Containers::StridedArrayView1D. Otherwise the overload below is picked,
returning a @ref Containers::StridedArrayView2D.
@see @ref EigenIntegration-stridedarrayview
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
@brief Convert an Eigen reverse expression to @ref Containers::StridedArrayView2D
@m_since_latest

Takes care of any Eigen expression that was not handled by the one-dimensional
overload above.
@see @ref EigenIntegration-stridedarrayview
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
