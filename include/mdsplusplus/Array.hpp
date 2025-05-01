#ifndef MDSPLUS_ARRAY_HPP
#define MDSPLUS_ARRAY_HPP

#include "Data.hpp"

#include <vector>
#include <complex>

namespace mdsplus {

///
/// Base class for all array types.
///
class Array : public Data
{
public:

    /// MDSplus Storage Class identifier.
    static constexpr Class __class = Class::A;

    Array() = default;

    /// 
    /// Creates a new Array from an underlying mdsdsc_xd_t descriptor, optionally with a Tree reference.
    /// 
    /// @param xd The mdsdsc_xd_t descriptor, passed with std::move().
    /// @param tree The Tree to be referenced when running TDI expressions on this data.
    /// 
    Array(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    /// 
    /// Creates a new Array from a initializer list of values, optionally with dimensions and a Tree reference.
    ///
    /// @param values The initializer list of values, e.g. `{ 1, 2, 3, 4}`.
    /// @param dims The dimensions describing the length of each dimension of the array, defaults to a 1D array with all of the values.
    /// @param tree The Tree to be referenced when running TDI expressions on this data.
    /// 
    template <typename CType>
    inline Array(DType dtype, std::initializer_list<CType> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        std::vector<CType> tmp(values);
        _setValues(dtype, tmp, dims);
        setTree(tree);
    }

    /// 
    /// Creates a new Array from a vector of values, optionally with dimensions and a Tree reference.
    /// 
    /// @param values The std::vector of values.
    /// @param dims The dimensions describing the length of each dimension of the array, defaults to a 1D array with all of the values.
    /// @param tree The Tree to be referenced when running TDI expressions on this data.
    /// 
    template <typename CType>
    inline Array(DType dtype, const std::vector<CType>& values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        _setValues<CType>(dtype, values, dims);
        setTree(tree);
    }

    #ifdef __cpp_lib_span

        /// 
        /// Creates a new Array from a span of values, optionally with dimensions and a Tree reference.
        /// 
        /// @param values The std::span of values.
        /// @param dims The dimensions describing the length of each dimension of the array, defaults to a 1D array with all of the values.
        /// @param tree The Tree to be referenced when running TDI expressions on this data.
        /// 
        template <typename CType>
        inline Array(DType dtype, const std::span<const CType> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
            setValues(values, dims, dtype);
            setTree(tree);
        }

    #endif

    ///
    /// Retrieve the underlying mdsdsc_a_t descriptor.
    ///
    /// @returns The mdsdsc_a_t pointer stored inside the mdsdsc_xd_t descriptor which describes the data in this array.
    ///
    [[nodiscard]]
    inline mdsdsc_a_t * getArrayDescriptor() const {
        return reinterpret_cast<mdsdsc_a_t *>(_xd.pointer);
    }

    ///
    /// Get the size of the flattened 1D array containing all the values.
    ///
    /// @returns The size of the flattened 1D array if the array is valid, or 0.
    ///
    [[nodiscard]]
    inline size_t getSize() const {
        mdsdsc_a_t * dsc = getArrayDescriptor();
        if (dsc) {
            return dsc->arsize / dsc->length;
        }
        return 0;
    }

    // STL Compatability

    inline size_t size() const {
        return getSize();
    }

    inline size_t empty() const {
        return getSize() == 0;
    }

protected:

    template <typename CType>
    inline void _setValues(DType dtype, const std::vector<CType>& values, const std::vector<uint32_t>& dims = {}) {
        if (dims.empty()) {
            _setValues(dtype, values.data(), values.size());
        }
        else {
            _setValues(dtype, values.data(), dims.data(), dims.size());
        }
    }

    #ifdef __cpp_lib_span

        template <typename CType>
        inline void _setValues(DType dtype, const std::span<const CType> values, const std::vector<uint32_t>& dims = {}) {
            if (dims.empty()) {
                _setValues(dtype, values.data(), values.size());
            }
            else {
                _setValues(dtype, values.data(), dims.data(), dims.size());
            }
        }

    #endif

    template <typename CType>
    inline void _setValues(DType dtype, const CType * values, uint32_t count) {
        _setValues(dtype, values, &count, 1);
    }

    template <typename CType>
    void _setValues(DType dtype, const CType * values, const uint32_t * dims, dimct_t dimCount)
    {
        int status;

        // TODO: Overwrite existing values if the shape/type is the same

        uint32_t count = dims[0];
        for (dimct_t i = 1; i < dimCount; ++i) {
            count *= dims[dimCount];
        }
        
        array_coeff dsc = {
            .length = sizeof(CType),
            .dtype = dtype_t(dtype),
            .class_ = CLASS_A,
            .pointer = (char *)values,
            .scale = 0,
            .digits = 0,
            .aflags = aflags_t{
                .binscale = false,
                .redim = true,
                .column = true,
                .coeff = false,
                .bounds = false,
            },
            .dimct = dimCount,
            .arsize = arsize_t(count * sizeof(CType)),
            .a0 = (char *)values,
            .m = { count, 0, 0, 0, 0, 0, 0, 0, },
        };

        if (dims && dimCount > 0) {
            dsc.aflags.coeff = true;
            for (size_t i = 0; i < dimCount; ++i) {
                dsc.m[i] = dims[i];
            }
        }

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename CType>
    inline std::vector<CType> _getValues() const {
        mdsdsc_a_t * dsc = getArrayDescriptor();
        if (dsc) {
            return std::vector<CType>((CType *)dsc->pointer, (CType *)dsc->pointer + size());
        }
        return {};
    }

    template <typename CType>
    [[nodiscard]]
    inline const CType& _getValueAt(size_t index) const {
        mdsdsc_a_t * dsc = getArrayDescriptor();
        if (dsc && index < size()) {
            return *((CType *)dsc->pointer + index);
        }

        throw TdiBadIndex();
    }

    // STL Compatability

    template <typename CType>
    inline const CType& _at(size_t index) const {
        return _getValueAt<CType>(index);
    }

    template <typename CType>
    inline CType * _begin() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        return (dsc ? (CType *)dsc->pointer : nullptr);
    }

    template <typename CType>
    inline CType * _end() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        return (dsc ? (CType *)(dsc->pointer + dsc->arsize) : nullptr);
    }

}; // class Array

#ifdef __cpp_lib_span

    #define MDSPLUS_ARRAY_BOOTSTRAP_SPAN(ArrayType) \
        inline Int8Array(                           \
            const std::span<const __ctype> values,  \
            const std::vector<uint32_t>& dims = {}, \
            Tree * tree = nullptr                   \
        )                                           \
            : Array(__dtype, values, dims, tree)    \
        { }                                         \
                                                    \
        inline void setValues(                      \
            const std::span<const __ctype> values,  \
            const std::vector<uint32_t>& dims = {}  \
        ) {                                         \
            _setValues(__dtype, values, dims);      \
        }

#else

    #define MDSPLUS_ARRAY_BOOTSTRAP_SPAN(ArrayType)

#endif // __cpp_lib_span

#define MDSPLUS_ARRAY_BOOTSTRAP(ArrayType, CType, DataType)                    \
                                                                               \
    typedef CType __ctype;                                                     \
    static constexpr DType __dtype = DataType;                                 \
                                                                               \
    ArrayType() = default;                                                     \
                                                                               \
    ArrayType(ArrayType &&) = default;                                         \
    ArrayType &operator=(ArrayType &&) = default;                              \
                                                                               \
    inline ArrayType(mdsdsc_xd_t &&xd, Tree *tree = nullptr)                   \
        : Array(std::move(xd), tree)                                           \
    { }                                                                        \
                                                                               \
    inline ArrayType(                                                          \
        std::initializer_list<__ctype> values,                                 \
        const std::vector<uint32_t> &dims = {},                                \
        Tree *tree = nullptr                                                   \
    )                                                                          \
        : Array(__dtype, values, dims, tree)                                   \
    { }                                                                        \
                                                                               \
    inline ArrayType(                                                          \
        const std::vector<__ctype> &values,                                    \
        const std::vector<uint32_t> &dims = {},                                \
        Tree *tree = nullptr                                                   \
    )                                                                          \
        : Array(__dtype, values, dims, tree)                                   \
    { }                                                                        \
                                                                               \
    inline std::string_view getClassName() const override {                    \
        return #ArrayType;                                                     \
    }                                                                          \
                                                                               \
    [[nodiscard]]                                                              \
    inline ArrayType clone() const {                                           \
        return _clone<ArrayType>();                                            \
    }                                                                          \
                                                                               \
    inline void setValues(                                                     \
        const std::vector<__ctype> &values,                                    \
        const std::vector<uint32_t> &dims = {}                                 \
    ) {                                                                        \
        _setValues(__dtype, values, dims);                                     \
    }                                                                          \
                                                                               \
    inline void setValues(const __ctype *values, uint32_t count) {             \
        _setValues(__dtype, values, count);                                    \
    }                                                                          \
                                                                               \
    void setValues(                                                            \
        const __ctype *values, const uint32_t *dims,                           \
        dimct_t dimCount                                                       \
    ) {                                                                        \
        _setValues(__dtype, values, dims, dimCount);                           \
    }                                                                          \
                                                                               \
    [[nodiscard]]                                                              \
    inline std::vector<__ctype> getValues() const {                            \
        return _getValues<__ctype>();                                          \
    }                                                                          \
                                                                               \
    [[nodiscard]]                                                              \
    inline const __ctype &getValueAt(size_t index) const {                     \
        return _getValueAt<__ctype>(index);                                    \
    }                                                                          \
                                                                               \
    [[nodiscard]]                                                              \
    inline const __ctype &operator[](size_t index) const {                     \
        return _getValueAt<__ctype>(index);                                    \
    }                                                                          \
                                                                               \
    [[nodiscard]]                                                              \
    inline const __ctype& at(size_t index) const {                             \
        return _at<__ctype>(index);                                            \
    }                                                                          \
                                                                               \
    [[nodiscard]]                                                              \
    inline __ctype * begin() const {                                           \
        return _begin<__ctype>();                                              \
    }                                                                          \
                                                                               \
    [[nodiscard]]                                                              \
    inline __ctype * end() const {                                             \
        return _end<__ctype>();                                                \
    }                                                                          \
                                                                               \
    MDSPLUS_ARRAY_BOOTSTRAP_SPAN(ArrayType)

class Int8Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(Int8Array, int8_t, DType::B)

    ///
    /// Deserialize a serialized Int8Array/UInt8Array into an MDSplus Data object, optionally converting it to a specified type.
    ///
    /// @returns The MDSplus Data object of the specified type.
    /// @throws TdiInvalidDataType if the object cannot be converted to the specified type.
    ///
    template <typename ResultType = Data>
    ResultType deserialize()
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        int status = MdsSerializeDscIn(getDescriptor()->pointer, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out)).releaseAndConvert<ResultType>();
    }

}; // class Int8Array

inline auto Data::serialize() const
{
    int status;

    mdsdsc_t * dsc = getDescriptor();
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    status = MdsSerializeDscOut(dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
    
    return Int8Array(std::move(out), getTree());
}

class UInt8Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(UInt8Array, uint8_t, DType::BU)

    ///
    /// Deserialize a serialized Int8Array/UInt8Array into an MDSplus Data object, optionally converting it to a specified type.
    ///
    /// @returns The MDSplus Data object of the specified type.
    /// @throws TdiInvalidDataType if the object cannot be converted to the specified type.
    ///
    template <typename ResultType = Data>
    ResultType deserialize()
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        int status = MdsSerializeDscIn(getDescriptor()->pointer, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out)).releaseAndConvert<ResultType>();
    }

}; // class Int8Array

class Int16Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(Int16Array, int16_t, DType::W)

}; // class Int16Array

class UInt16Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(UInt16Array, uint16_t, DType::WU)

}; // class UInt16Array

class Int32Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(Int32Array, int32_t, DType::L)

}; // class Int32Array

class UInt32Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(UInt32Array, uint32_t, DType::LU)

}; // class UInt32Array

class Int64Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(Int64Array, int64_t, DType::Q)

}; // class Int64Array

class UInt64Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(UInt64Array, uint64_t, DType::QU)

}; // class UInt64Array

class Float32Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(Float32Array, float, DType::FS)

}; // class Float32Array

class Float64Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(Float64Array, double, DType::FT)

}; // class Float64Array

class Complex32Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(Complex32Array, std::complex<float>, DType::FSC)

}; // class Complex32Array

class Complex64Array : public Array
{
public:

    MDSPLUS_ARRAY_BOOTSTRAP(Complex64Array, std::complex<double>, DType::FTC)

}; // class Complex64Array

template <>
inline std::vector<Int8Array::__ctype> Data::getData() const {
    return getData<Int8Array>().getValues();
}

template <>
inline std::vector<UInt8Array::__ctype> Data::getData() const {
    return getData<UInt8Array>().getValues();
}

template <>
inline std::vector<Int16Array::__ctype> Data::getData() const {
    return getData<Int16Array>().getValues();
}

template <>
inline std::vector<UInt16Array::__ctype> Data::getData() const {
    return getData<UInt16Array>().getValues();
}

template <>
inline std::vector<Int32Array::__ctype> Data::getData() const {
    return getData<Int32Array>().getValues();
}

template <>
inline std::vector<UInt32Array::__ctype> Data::getData() const {
    return getData<UInt32Array>().getValues();
}

template <>
inline std::vector<Int64Array::__ctype> Data::getData() const {
    return getData<Int64Array>().getValues();
}

template <>
inline std::vector<UInt64Array::__ctype> Data::getData() const {
    return getData<UInt64Array>().getValues();
}

template <>
inline std::vector<Float32Array::__ctype> Data::getData() const {
    return getData<Float32Array>().getValues();
}

template <>
inline std::vector<Float64Array::__ctype> Data::getData() const {
    return getData<Float64Array>().getValues();
}

template <>
inline std::vector<Complex32Array::__ctype> Data::getData() const {
    return getData<Complex32Array>().getValues();
}

template <>
inline std::vector<Complex64Array::__ctype> Data::getData() const {
    return getData<Complex64Array>().getValues();
}

template <>
inline Data Data::FromArray(
    const std::vector<Int8Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return Int8Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<UInt8Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return UInt8Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<Int16Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return Int16Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<UInt16Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return UInt16Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<Int32Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return Int32Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<UInt32Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return UInt32Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<Int64Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return Int64Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<UInt64Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return UInt64Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<Float32Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return Float32Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<Float64Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return Float64Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<Complex32Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return Complex32Array(values, dims);
}

template <>
inline Data Data::FromArray(
    const std::vector<Complex64Array::__ctype>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    return Complex64Array(values, dims);
}

#ifdef __cpp_lib_span

    template <>
    inline Data Data::FromArray(
        std::span<Int8Array::__ctype> values,
        const std::vector<uint32_t>& dims /*= {}*/
    ) {
        return Int8Array(values, dims);
    }

    template <>
    inline Data Data::FromArray(
        std::span<UInt8Array::__ctype> values,
        const std::vector<uint32_t>& dims /*= {}*/
    ) {
        return UInt8Array(values, dims);
    }

    template <>
    inline Data Data::FromArray(
        std::span<Int16Array::__ctype> values,
        const std::vector<uint32_t>& dims /*= {}*/
    ) {
        return Int16Array(values, dims);
    }

    template <>
    inline Data Data::FromArray(
        std::span<UInt16Array::__ctype> values,
        const std::vector<uint32_t>& dims /*= {}*/
    ) {
        return UInt16Array(values, dims);
    }

#endif // __cpp_lib_span

} // namespace mdsplus

#endif // MDSPLUS_ARRAY_HPP