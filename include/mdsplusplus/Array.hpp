#ifndef MDSPLUS_ARRAY_HPP
#define MDSPLUS_ARRAY_HPP

#include "Data.hpp"
#include "Scalar.hpp"

#include <vector>
#include <complex>

namespace mdsplus {

template <typename CT, typename ST, DType DT>
class Array : public Data
{
public:

    typedef CT ctype;
    static constexpr Class class_ = Class::A;
    static constexpr DType dtype = DT;

    Array() = default;

    inline Array(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        if (dsc && dsc->length == 0) {
            dsc->length = sizeof(ctype);
        }
    }

    inline Array(std::initializer_list<ctype> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        std::vector<ctype> tmp(values);
        setValues(tmp, dims);
        setTree(tree);
    }

    inline Array(const std::vector<ctype>& values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        setValues(values, dims);
        setTree(tree);
    }

#ifdef __cpp_lib_span
    inline Array(const std::span<const ctype> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        setValues(values, dims);
        setTree(tree);
    }
#endif

    Array(Array&&) = default;
    Array& operator=(Array&&) = default;

    [[nodiscard]]
    inline mdsdsc_a_t * getArrayDescriptor() const {
        return reinterpret_cast<mdsdsc_a_t *>(_xd.pointer);
    }

    [[nodiscard]]
    inline const ctype& operator[](size_t index) const {
        return getValueAt(index);
    }

    [[nodiscard]]
    inline Array clone() const {
        return _clone<Array>();
    }

    inline void setValues(const std::vector<ctype>& values, const std::vector<uint32_t>& dims = {}) {
        if (dims.empty()) {
            setValues(values.data(), values.size());
        }
        else {
            setValues(values.data(), dims.data(), dims.size());
        }
    }

#ifdef __cpp_lib_span
    inline void setValues(const std::span<const ctype> values, const std::vector<uint32_t>& dims = {}) {
        if (dims.empty()) {
            setValues(values.data(), values.size());
        }
        else {
            setValues(values.data(), dims.data(), dims.size());
        }
    }
#endif

    inline void setValues(const ctype * values, uint32_t count) {
        setValues(values, &count, 1);
    }

    void setValues(const ctype * values, const uint32_t * dims, dimct_t dimCount)
    {
        int status;

        uint32_t count = dims[0];
        for (dimct_t i = 1; i < dimCount; ++i) {
            count *= dims[dimCount];
        }
        
        array_coeff dsc = {
            .length = sizeof(ctype),
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
            .arsize = arsize_t(count * sizeof(ctype)),
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

    [[nodiscard]]
    inline std::vector<ctype> getValues() const {
        mdsdsc_a_t * dsc = getArrayDescriptor();
        if (dsc) {
            return std::vector<ctype>((ctype *)dsc->pointer, (ctype *)dsc->pointer + size());
        }
        return {};
    }

    [[nodiscard]]
    inline const ctype& getValueAt(size_t index) const {
        mdsdsc_a_t * dsc = getArrayDescriptor();
        if (dsc && index < size()) {
            return *((ctype *)dsc->pointer + index);
        }

        throw TdiBadIndex();
    }

    [[nodiscard]]
    inline size_t getSize() const {
        mdsdsc_a_t * dsc = getArrayDescriptor();
        if (dsc) {
            return dsc->arsize / dsc->length;
        }
        return 0;
    }

    // STL Compatability

    [[nodiscard]]
    inline size_t size() const {
        return getSize();
    }

    [[nodiscard]]
    inline size_t empty() const {
        return getSize() == 0;
    }

    [[nodiscard]]
    inline const ctype& at(size_t index) const {
        return getValueAt(index);
    }

    [[nodiscard]]
    inline ctype * begin() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        return (dsc ? (ctype *)dsc->pointer : nullptr);
    }

    [[nodiscard]]
    inline ctype * end() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        return (dsc ? (ctype *)(dsc->pointer + dsc->arsize) : nullptr);
    }

    template <typename T>
    typename std::enable_if_t<(sizeof(CT) == 1), T> deserialize()
    {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        status = MdsSerializeDscIn(getDescriptor()->pointer, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out)).releaseAndConvert<T>();
    }

}; // class Array<>

typedef Array<int8_t, Int8, DType::B> Int8Array;
typedef Array<uint8_t, UInt8, DType::BU> UInt8Array;
typedef Array<int16_t, Int16, DType::W> Int16Array;
typedef Array<uint16_t, UInt16, DType::WU> UInt16Array;
typedef Array<int32_t, Int32, DType::L> Int32Array;
typedef Array<uint32_t, UInt32, DType::LU> UInt32Array;
typedef Array<int64_t, Int64, DType::Q> Int64Array;
typedef Array<uint64_t, UInt64, DType::QU> UInt64Array;
typedef Array<float, Float32, DType::Float> Float32Array;
typedef Array<double, Float64, DType::Double> Float64Array;
typedef Array<std::complex<float>, Complex32, DType::Float> Complex32Array;
typedef Array<std::complex<double>, Complex64, DType::Double> Complex64Array;

template <>
inline Data Data::FromArray(const std::vector<Int8Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int8Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<UInt8Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt8Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Int16Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int16Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<UInt16Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt16Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Int32Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int32Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<UInt32Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt32Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Int64Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int64Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<UInt64Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt64Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Float32Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Float32Array(values, dims);
}

template <>
inline Data Data::FromArray(const std::vector<Float64Array::ctype>& values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Float64Array(values, dims);
}

#ifdef __cpp_lib_span

template <>
inline Data Data::FromArray(std::span<Int8Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int8Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<UInt8Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt8Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Int16Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int16Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<UInt16Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt16Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Int32Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int32Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<UInt32Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt32Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Int64Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Int64Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<UInt64Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return UInt64Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Float32Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Float32Array(values, dims);
}

template <>
inline Data Data::FromArray(std::span<Float64Array::ctype> values, const std::vector<uint32_t>& dims /*= {}*/) {
    return Float64Array(values, dims);
}

#endif // __cpp_lib_span

template <>
inline std::vector<Int8Array::ctype> Data::getData() const {
    return getData<Int8Array>().getValues();
}

template <>
inline std::vector<UInt8Array::ctype> Data::getData() const {
    return getData<UInt8Array>().getValues();
}

template <>
inline std::vector<Int16Array::ctype> Data::getData() const {
    return getData<Int16Array>().getValues();
}

template <>
inline std::vector<UInt16Array::ctype> Data::getData() const {
    return getData<UInt16Array>().getValues();
}

template <>
inline std::vector<Int32Array::ctype> Data::getData() const {
    return getData<Int32Array>().getValues();
}

template <>
inline std::vector<UInt32Array::ctype> Data::getData() const {
    return getData<UInt32Array>().getValues();
}

template <>
inline std::vector<Int64Array::ctype> Data::getData() const {
    return getData<Int64Array>().getValues();
}

template <>
inline std::vector<UInt64Array::ctype> Data::getData() const {
    return getData<UInt64Array>().getValues();
}

template <>
inline std::vector<Float32Array::ctype> Data::getData() const {
    return getData<Float32Array>().getValues();
}

template <>
inline std::vector<Float64Array::ctype> Data::getData() const {
    return getData<Float64Array>().getValues();
}

} // namespace mdsplus

#endif // MDSPLUS_ARRAY_HPP