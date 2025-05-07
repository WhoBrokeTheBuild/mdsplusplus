#ifndef MDSPLUS_ARRAY_HPP
#define MDSPLUS_ARRAY_HPP

#include "Data.hpp"

#include <vector>
#include <complex>

#if __has_include(<span>)
    #include <span>
#endif

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
    /// Retrieve the underlying mdsdsc_a_t descriptor.
    ///
    /// @returns The mdsdsc_a_t pointer stored inside the mdsdsc_xd_t descriptor which describes the data in this array.
    ///
    [[nodiscard]]
    inline mdsdsc_a_t * getArrayDescriptor() const {
        return reinterpret_cast<mdsdsc_a_t *>(_xd.pointer);
    }
    
    [[nodiscard]]
    std::vector<uint32_t> getDimensions() const;

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

    inline bool empty() const {
        return (getSize() == 0);
    }

protected:

    /// 
    /// Creates a new Array from a initializer list of values, optionally with dimensions and a Tree reference.
    ///
    /// @param values The initializer list of values, e.g. `{ 1, 2, 3, 4}`.
    /// @param dims The dimensions describing the length of each dimension of the array, defaults to a 1D array with all of the values.
    /// 
    template <typename CType>
    inline Array(DType dtype, std::initializer_list<CType> values, const std::vector<uint32_t>& dims = {}) {
        std::vector<CType> tmp(values);
        _setValues(dtype, tmp, dims);
    }

    #ifdef __cpp_lib_span

        template <typename CType>
        inline Array(DType dtype, std::span<const CType> values, const std::vector<uint32_t>& dims = {}) {
            _setValues<CType>(dtype, values, dims);
        }

        template <typename CType>
        inline Array(DType dtype, const std::vector<CType>& values, const std::vector<uint32_t>& dims = {}) {
            _setValues<CType>(dtype, std::span(values), dims);
        }

        template <typename CType>
        inline void _setValues(DType dtype, std::span<const CType> values, const std::vector<uint32_t>& dims = {}) {
            if (dims.empty()) {
                _setValues(dtype, values.data(), values.size());
            }
            else {
                _setValues(dtype, values.data(), dims.data(), dims.size());
            }
        }

        template <typename CType>
        std::span<CType> _getSpan() const;
    
    #else

        template <typename CType>
        inline Array(DType dtype, const std::vector<CType>& values, const std::vector<uint32_t>& dims = {}) {
            _setValues<CType>(dtype, values, dims);
        }

        template <typename CType>
        inline void _setValues(DType dtype, const std::vector<CType>& values, const std::vector<uint32_t>& dims = {}) {
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
    void _setValues(DType dtype, const CType * values, const uint32_t * dims, dimct_t dimCount);

    template <typename CType>
    std::vector<CType> _getVector() const;

    template <typename CType>
    const CType& _getValueAt(size_t index) const;

    // STL Compatability

    template <typename CType>
    inline const CType& _at(size_t index) const {
        return _getValueAt<CType>(index);
    }

    template <typename CType>
    CType * _begin() const;

    template <typename CType>
    CType * _end() const;

}; // class Array

#ifdef __cpp_lib_span

    #define _MDSPLUS_ARRAY_BOOTSTRAP_STD(ArrayType)                \
                                                                   \
        inline ArrayType(                                          \
            std::span<const __ctype> values,                       \
            const std::vector<uint32_t>& dims = {}                 \
        )                                                          \
            : Array(__dtype, values, dims)                         \
        { }                                                        \
                                                                   \
        inline void setValues(                                     \
            std::span<const __ctype> values,                       \
            const std::vector<uint32_t>& dims = {}                 \
        ) {                                                        \
            _setValues<__ctype>(__dtype, values, dims);            \
        }                                                          \
                                                                   \
        inline ArrayType(                                          \
            const std::vector<__ctype> &values,                    \
            const std::vector<uint32_t> &dims = {}                 \
        )                                                          \
            : Array(__dtype, std::span(values), dims)              \
        { }                                                        \
                                                                   \
        inline void setValues(                                     \
            const std::vector<__ctype> &values,                    \
            const std::vector<uint32_t> &dims = {}                 \
        ) {                                                        \
            _setValues<__ctype>(__dtype, std::span(values), dims); \
        }                                                          \
                                                                   \
        inline std::span<__ctype> getSpan() const {                \
            return _getSpan<__ctype>();                            \
        }

#else

    #define _MDSPLUS_ARRAY_BOOTSTRAP_STD(ArrayType) \
                                                    \
        inline ArrayType(                           \
            const std::vector<__ctype> &values,     \
            const std::vector<uint32_t> &dims = {}  \
        )                                           \
            : Array(__dtype, values, dims)          \
        { }                                         \
                                                    \
        inline void setValues(                      \
            const std::vector<__ctype> &values,     \
            const std::vector<uint32_t> &dims = {}  \
        ) {                                         \
            _setValues(__dtype, values, dims);      \
        }

#endif // __cpp_lib_span

#define _MDSPLUS_ARRAY_BOOTSTRAP(ArrayType, CType, DataType)                       \
                                                                                   \
    typedef CType __ctype;                                                         \
    static constexpr DType __dtype = DataType;                                     \
                                                                                   \
    ArrayType() = default;                                                         \
                                                                                   \
    ArrayType(ArrayType &&) = default;                                             \
    ArrayType &operator=(ArrayType &&) = default;                                  \
                                                                                   \
    inline ArrayType(mdsdsc_xd_t &&xd, Tree *tree = nullptr)                       \
        : Array(std::move(xd), tree)                                               \
    { }                                                                            \
                                                                                   \
    inline ArrayType(                                                              \
        std::initializer_list<__ctype> values,                                     \
        const std::vector<uint32_t> &dims = {}                                     \
    )                                                                              \
        : Array(__dtype, std::vector<__ctype>(values.begin(), values.end()), dims) \
    { }                                                                            \
                                                                                   \
    inline const char * getClassName() const override {                            \
        return #ArrayType;                                                         \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline ArrayType clone() const {                                               \
        return _clone<ArrayType>();                                                \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline std::vector<__ctype> getVector() const {                                \
        return _getVector<__ctype>();                                              \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline std::vector<__ctype> getValues() const {                                \
        return getVector();                                                        \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline const __ctype &getValueAt(size_t index) const {                         \
        return _getValueAt<__ctype>(index);                                        \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline const __ctype &operator[](size_t index) const {                         \
        return _getValueAt<__ctype>(index);                                        \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline const __ctype& at(size_t index) const {                                 \
        return _at<__ctype>(index);                                                \
    }                                                                              \
                                                                                   \
    inline void setValues(const __ctype *values, uint32_t count) {                 \
        _setValues(__dtype, values, count);                                        \
    }                                                                              \
                                                                                   \
    void setValues(                                                                \
        const __ctype *values,                                                     \
        const uint32_t *dims,                                                      \
        dimct_t dimCount                                                           \
    ) {                                                                            \
        _setValues(__dtype, values, dims, dimCount);                               \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline __ctype * begin() const {                                               \
        return _begin<__ctype>();                                                  \
    }                                                                              \
                                                                                   \
    [[nodiscard]]                                                                  \
    inline __ctype * end() const {                                                 \
        return _end<__ctype>();                                                    \
    }                                                                              \
                                                                                   \
    _MDSPLUS_ARRAY_BOOTSTRAP_STD(ArrayType)

class Int8Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Int8Array, int8_t, DType::B)

    ///
    /// Deserialize a serialized Int8Array/UInt8Array into an MDSplus Data object, optionally converting it to a specified type.
    ///
    /// @returns The MDSplus Data object of the specified type.
    /// @throws TdiInvalidDataType if the object cannot be converted to the specified type.
    ///
    template <typename ResultType = Data>
    [[nodiscard]]
    ResultType deserialize();

}; // class Int8Array

class UInt8Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(UInt8Array, uint8_t, DType::BU)

    ///
    /// Deserialize a serialized Int8Array/UInt8Array into an MDSplus Data object, optionally converting it to a specified type.
    ///
    /// @returns The MDSplus Data object of the specified type.
    /// @throws TdiInvalidDataType if the object cannot be converted to the specified type.
    ///
    template <typename ResultType = Data>
    [[nodiscard]]
    ResultType deserialize();

}; // class Int8Array

class Int16Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Int16Array, int16_t, DType::W)

}; // class Int16Array

class UInt16Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(UInt16Array, uint16_t, DType::WU)

}; // class UInt16Array

class Int32Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Int32Array, int32_t, DType::L)

}; // class Int32Array

class UInt32Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(UInt32Array, uint32_t, DType::LU)

}; // class UInt32Array

class Int64Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Int64Array, int64_t, DType::Q)

}; // class Int64Array

class UInt64Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(UInt64Array, uint64_t, DType::QU)

}; // class UInt64Array

class Float32Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Float32Array, float, DType::FS)

}; // class Float32Array

class Float64Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Float64Array, double, DType::FT)

}; // class Float64Array

class Complex32Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Complex32Array, std::complex<float>, DType::FSC)

}; // class Complex32Array

class Complex64Array : public Array
{
public:

    _MDSPLUS_ARRAY_BOOTSTRAP(Complex64Array, std::complex<double>, DType::FTC)

}; // class Complex64Array

#ifdef __cpp_lib_span

    #define MDSPLUS_ARRAY_CUSTOMIZATION_SPAN(ArrayType)    \
        template <>                                        \
        inline Data Data::FromArray(                       \
            std::span<const ArrayType::__ctype> values,    \
            const std::vector<uint32_t>& dims /*= {}*/     \
        ) {                                                \
            return ArrayType(values, dims);                \
        }                                                  \
                                                           \
        template <>                                        \
        inline Data Data::FromArray(                       \
            const std::vector<ArrayType::__ctype>& values, \
            const std::vector<uint32_t>& dims /*= {}*/     \
        ) {                                                \
            return ArrayType(std::span(values), dims);     \
        }

#else

    #define MDSPLUS_ARRAY_CUSTOMIZATION_SPAN(ArrayType)    \
                                                           \
        template <>                                        \
        inline Data Data::FromArray(                       \
            const std::vector<ArrayType::__ctype>& values, \
            const std::vector<uint32_t>& dims /*= {}*/     \
        ) {                                                \
            return ArrayType(values, dims);                \
        }

#endif

#define MDSPLUS_ARRAY_CUSTOMIZATION(ArrayType)                     \
                                                                   \
    template <>                                                    \
    inline ArrayType Data::releaseAndConvert() {                   \
        return _convertToArray<ArrayType>();                       \
    }                                                              \
                                                                   \
    template <>                                                    \
    inline std::vector<ArrayType::__ctype> Data::getData() const { \
        return getData<ArrayType>().getValues();                   \
    }                                                              \
                                                                   \
    MDSPLUS_ARRAY_CUSTOMIZATION_SPAN(ArrayType)

MDSPLUS_ARRAY_CUSTOMIZATION(Int8Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Int16Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Int32Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Int64Array)
MDSPLUS_ARRAY_CUSTOMIZATION(UInt8Array)
MDSPLUS_ARRAY_CUSTOMIZATION(UInt16Array)
MDSPLUS_ARRAY_CUSTOMIZATION(UInt32Array)
MDSPLUS_ARRAY_CUSTOMIZATION(UInt64Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Float32Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Float64Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Complex32Array)
MDSPLUS_ARRAY_CUSTOMIZATION(Complex64Array)

} // namespace mdsplus

#endif // MDSPLUS_ARRAY_HPP