#ifndef MDSPLUS_SCALAR_HPP
#define MDSPLUS_SCALAR_HPP

#include "Data.hpp"
#include "DataView.hpp"

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
    Scalar(DType dtype, CType value) {
        _setValue<CType>(dtype, value);
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
        return {};
    }

};

#define _MDSPLUS_SCALAR_BOOTSTRAP(ScalarType, CType, DataType)  \
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
    inline ScalarType(__ctype value)                            \
        : Scalar(__dtype, value)                                \
    { }                                                         \
                                                                \
    inline const char * getClassName() const override {         \
        return #ScalarType;                                     \
    }                                                           \
                                                                \
    [[nodiscard]]                                               \
    inline ScalarType clone() const {                           \
        return _clone<ScalarType>();                            \
    }                                                           \
                                                                \
    [[nodiscard]]                                               \
    inline __ctype getValue() const {                           \
        return _getValue<__ctype>();                            \
    }                                                           \
                                                                \
    inline void setValue(__ctype value) {                       \
        _setValue<__ctype>(__dtype, value);                     \
    }

#define _MDSPLUS_SCALAR_CUSTOMIZATION(ScalarType)                    \
                                                                     \
    template <>                                                      \
    struct is_valid_ctype<ScalarType::__ctype> : std::true_type { }; \
                                                                     \
    template <>                                                      \
    inline ScalarType Data::releaseAndConvert() {                    \
        return _convertToScalar<ScalarType>();                       \
    }                                                                \
                                                                     \
    template <>                                                      \
    inline ScalarType::__ctype Data::getData() const {               \
        return getData<ScalarType>().getValue();                     \
    }                                                                \
                                                                     \
    template <>                                                      \
    inline Data Data::FromScalar(ScalarType::__ctype value) {        \
        return ScalarType(value);                                    \
    }

class Int8 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Int8, int8_t, DType::B)

}; // class Int8

_MDSPLUS_SCALAR_CUSTOMIZATION(Int8)

class UInt8 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(UInt8, uint8_t, DType::BU)

}; // class UInt8

_MDSPLUS_SCALAR_CUSTOMIZATION(UInt8)

class Int16 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Int16, int16_t, DType::W)

}; // class Int16

_MDSPLUS_SCALAR_CUSTOMIZATION(Int16)

class UInt16 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(UInt16, uint16_t, DType::WU)

}; // class UInt16

_MDSPLUS_SCALAR_CUSTOMIZATION(UInt16)

class Int32 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Int32, int32_t, DType::L)

}; // class Int32

_MDSPLUS_SCALAR_CUSTOMIZATION(Int32)

class UInt32 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(UInt32, uint32_t, DType::LU)

}; // class UInt32

_MDSPLUS_SCALAR_CUSTOMIZATION(UInt32)

class Int64 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Int64, int64_t, DType::Q)

}; // class Int32

_MDSPLUS_SCALAR_CUSTOMIZATION(Int64)

class UInt64 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(UInt64, uint64_t, DType::QU)

}; // class UInt64

_MDSPLUS_SCALAR_CUSTOMIZATION(UInt64)

// TODO: Int128 / UInt128 ?

class Float32 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Float32, float, DType::FS)

}; // class Float32

_MDSPLUS_SCALAR_CUSTOMIZATION(Float32)

class Float64 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Float64, double, DType::FT)

}; // class Float64

_MDSPLUS_SCALAR_CUSTOMIZATION(Float64)

class Complex32 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Complex32, std::complex<float>, DType::FSC)

}; // class Complex32

_MDSPLUS_SCALAR_CUSTOMIZATION(Complex32)

class Complex64 : public Scalar
{
public:

    _MDSPLUS_SCALAR_BOOTSTRAP(Complex64, std::complex<double>, DType::FTC)

}; // class Complex64

_MDSPLUS_SCALAR_CUSTOMIZATION(Complex64)

} // namespace mdsplus

namespace std {

#define _MDSPLUS_SPECIALIZE_STD_HASH(ScalarType)                      \
    template <> struct hash<ScalarType>                               \
    {                                                                 \
        size_t operator()(const ScalarType& data) const {             \
            return std::hash<ScalarType::__ctype>()(data.getValue()); \
        }                                                             \
    };

_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Int8)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Int16)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Int32)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Int64)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::UInt8)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::UInt16)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::UInt32)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::UInt64)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Float32)
_MDSPLUS_SPECIALIZE_STD_HASH(mdsplus::Float64)

} // namespace std

#endif // MDSPLUS_SCALAR_HPP