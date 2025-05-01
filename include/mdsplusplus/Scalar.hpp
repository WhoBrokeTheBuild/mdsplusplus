#ifndef MDSPLUS_SCALAR_HPP
#define MDSPLUS_SCALAR_HPP

#include "Data.hpp"

#include <complex>

namespace mdsplus {

class Scalar : public Data
{
public:
    
    static constexpr Class __class = Class::S;

    Scalar() = default;

    Scalar(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

protected:

    template <typename CType>
    Scalar(DType dtype, CType value, Tree * tree = nullptr)
    {
        _setValue<CType>(dtype, value);
        setTree(tree);
    }

    template <typename CType>
    inline void _setValue(DType dtype, CType value)
    {
        int status;

        mdsdsc_s_t dsc = {
            .length = sizeof(CType),
            .dtype = dtype_t(dtype),
            .class_ = class_t(__class),
            .pointer = (char *)&value,
        };

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename CType>
    inline CType _getValue() const {
        mdsdsc_t * dsc = getDescriptor();
        if (dsc && dsc->pointer) {
            return *(CType *)dsc->pointer;
        }

        // TODO: throw something?
        return CType{};
    }

};

#define MDSPLUS_SCALAR_BOOTSTRAP(ScalarType, CType, DataType)   \
                                                                \
    typedef CType __ctype;                                      \
    static constexpr DType __dtype = DataType;                  \
                                                                \
    ScalarType() = default;                                     \
                                                                \
    ScalarType(ScalarType&&) = default;                         \
    ScalarType& operator=(ScalarType&&) = default;              \
                                                                \
    inline ScalarType(mdsdsc_xd_t && xd, Tree * tree = nullptr) \
        : Scalar(std::move(xd), tree)                           \
    { }                                                         \
                                                                \
    /** Construct a Scalar from a value **/                     \
    inline ScalarType(__ctype value, Tree * tree = nullptr)     \
        : Scalar(__dtype, value, tree)                          \
    { }                                                         \
                                                                \
    inline std::string_view getClassName() const override {     \
        return #ScalarType;                                     \
    }                                                           \
                                                                \
    [[nodiscard]]                                               \
    inline ScalarType clone() const {                           \
        return _clone<ScalarType>();                            \
    }                                                           \
                                                                \
    inline void setValue(__ctype value) {                       \
        _setValue<__ctype>(__dtype, value);                     \
    }                                                           \
                                                                \
    [[nodiscard]]                                               \
    inline __ctype getValue() const {                           \
        return _getValue<__ctype>();                            \
    }

class Int8 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(Int8, int8_t, DType::B)

}; // class Int8

class UInt8 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(UInt8, uint8_t, DType::BU)

}; // class UInt8

class Int16 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(Int16, int16_t, DType::W)

}; // class Int16

class UInt16 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(UInt16, uint16_t, DType::WU)

}; // class UInt16

class Int32 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(Int32, int32_t, DType::L)

}; // class Int32

class UInt32 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(UInt32, uint32_t, DType::LU)

}; // class UInt32

class Int64 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(Int64, int64_t, DType::Q)

}; // class Int32

class UInt64 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(UInt64, uint64_t, DType::QU)

}; // class UInt64

// TODO: Int128 / UInt128 ?

class Float32 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(Float32, float, DType::FS)

}; // class Float32

class Float64 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(Float64, double, DType::FT)

}; // class Float64

class Complex32 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(Complex32, std::complex<float>, DType::FSC)

}; // class Complex32

class Complex64 : public Scalar
{
public:

    MDSPLUS_SCALAR_BOOTSTRAP(Complex64, std::complex<double>, DType::FTC)

}; // class Complex64

template <>
inline Int8::__ctype Data::getData() const {
    return getData<Int8>().getValue();
}

template <>
inline UInt8::__ctype Data::getData() const {
    return getData<UInt8>().getValue();
}

template <>
inline Int16::__ctype Data::getData() const {
    return getData<Int16>().getValue();
}

template <>
inline UInt16::__ctype Data::getData() const {
    return getData<UInt16>().getValue();
}

template <>
inline Int32::__ctype Data::getData() const {
    return getData<Int32>().getValue();
}

template <>
inline UInt32::__ctype Data::getData() const {
    return getData<UInt32>().getValue();
}

template <>
inline Int64::__ctype Data::getData() const {
    return getData<Int64>().getValue();
}

template <>
inline UInt64::__ctype Data::getData() const {
    return getData<UInt64>().getValue();
}

template <>
inline Float32::__ctype Data::getData() const {
    return getData<Float32>().getValue();
}

template <>
inline Float64::__ctype Data::getData() const {
    return getData<Float64>().getValue();
}

template <>
inline Complex32::__ctype Data::getData() const {
    return getData<Complex32>().getValue();
}

template <>
inline Complex64::__ctype Data::getData() const {
    return getData<Complex32>().getValue();
}

template <>
inline Data Data::FromScalar(Int8::__ctype value) {
    return Int8(value);
}

template <>
inline Data Data::FromScalar(UInt8::__ctype value) {
    return UInt8(value);
}

template <>
inline Data Data::FromScalar(Int16::__ctype value) {
    return Int16(value);
}

template <>
inline Data Data::FromScalar(UInt16::__ctype value) {
    return UInt16(value);
}

template <>
inline Data Data::FromScalar(Int32::__ctype value) {
    return Int32(value);
}

template <>
inline Data Data::FromScalar(UInt32::__ctype value) {
    return UInt32(value);
}

template <>
inline Data Data::FromScalar(Int64::__ctype value) {
    return Int64(value);
}

template <>
inline Data Data::FromScalar(UInt64::__ctype value) {
    return UInt64(value);
}

template <>
inline Data Data::FromScalar(Float32::__ctype value) {
    return Float32(value);
}

template <>
inline Data Data::FromScalar(Float64::__ctype value) {
    return Float64(value);
}

template <>
inline Data Data::FromScalar(Complex32::__ctype value) {
    return Complex32(value);
}

template <>
inline Data Data::FromScalar(Complex64::__ctype value) {
    return Complex64(value);
}

} // namespace mdsplus

#endif // MDSPLUS_SCALAR_HPP