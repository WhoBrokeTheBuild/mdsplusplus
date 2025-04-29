#ifndef MDSPLUS_SCALAR_HPP
#define MDSPLUS_SCALAR_HPP

#include "Data.hpp"

#include <complex>

namespace mdsplus {

class Scalar : public Data
{
public:

    static constexpr Class class_ = Class::S;

    Scalar() = default;

    Scalar(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    template <typename T>
    inline Data operator+(const T& value) {
        return *this + Data::FromScalar(value);
    }

    template <typename T>
    inline Data operator-(const T& value) {
        return *this + Data::FromScalar(value);
    }

    template <typename T>
    inline Data operator/(const T& value) {
        return *this + Data::FromScalar(value);
    }

    template <typename T>
    inline Data operator*(const T& value) {
        return *this + Data::FromScalar(value);
    }

    template <typename T>
    inline Data operator%(const T& value) {
        return *this + Data::FromScalar(value);
    }

    inline Data operator+(const Data& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_ADD, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }
    
    inline Data operator-(const Data& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_SUBTRACT, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    inline Data operator/(const Data& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_DIVIDE, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    inline Data operator*(const Data& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_MULTIPLY, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    inline Data operator%(const Data& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_MOD, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

};

template <typename CT, DType DT>
class ScalarType : public Scalar
{
public:

    typedef CT ctype;
    static constexpr DType dtype = DT;

    ScalarType() = default;

    ScalarType(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Scalar(std::move(xd), tree)
    { }

    ScalarType(ctype value, Tree * tree = nullptr) {
        setValue(value);
        setTree(tree);
    }

    ScalarType(ScalarType&&) = default;
    ScalarType& operator=(ScalarType&&) = default;

    [[nodiscard]]
    inline ScalarType clone() const {
        return _clone<ScalarType>();
    }

    // TODO: Math?

    inline void setValue(ctype value)
    {
        int status;

        mdsdsc_s_t dsc = {
            .length = sizeof(ctype),
            .dtype = dtype_t(dtype),
            .class_ = class_t(class_),
            .pointer = (char *)&value,
        };

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    [[nodiscard]]
    inline ctype getValue() const {
        mdsdsc_t * dsc = getDescriptor();
        if (dsc && dsc->pointer) {
            return *(ctype *)dsc->pointer;
        }

        // TODO: throw something?
        return ctype{};
    }

}; // class Scalar<>

typedef ScalarType<int8_t, DType::B> Int8;
typedef ScalarType<uint8_t, DType::BU> UInt8;
typedef ScalarType<int16_t, DType::W> Int16;
typedef ScalarType<uint16_t, DType::WU> UInt16;
typedef ScalarType<int32_t, DType::L> Int32;
typedef ScalarType<uint32_t, DType::LU> UInt32;
typedef ScalarType<int64_t, DType::Q> Int64;
typedef ScalarType<uint64_t, DType::QU> UInt64;
typedef ScalarType<float, DType::FS> Float32;
typedef ScalarType<double, DType::FT> Float64;
typedef ScalarType<std::complex<float>, DType::FSC> Complex32;
typedef ScalarType<std::complex<double>, DType::FTC> Complex64;

template <>
inline Data Data::FromScalar(Int8::ctype value) {
    return Int8(value);
}

template <>
inline Data Data::FromScalar(UInt8::ctype value) {
    return UInt8(value);
}

template <>
inline Data Data::FromScalar(Int16::ctype value) {
    return Int16(value);
}

template <>
inline Data Data::FromScalar(UInt16::ctype value) {
    return UInt16(value);
}

template <>
inline Data Data::FromScalar(Int32::ctype value) {
    return Int32(value);
}

template <>
inline Data Data::FromScalar(UInt32::ctype value) {
    return UInt32(value);
}

template <>
inline Data Data::FromScalar(Int64::ctype value) {
    return Int64(value);
}

template <>
inline Data Data::FromScalar(UInt64::ctype value) {
    return UInt64(value);
}

template <>
inline Data Data::FromScalar(Float32::ctype value) {
    return Float32(value);
}

template <>
inline Data Data::FromScalar(Float64::ctype value) {
    return Float64(value);
}

template <>
inline Int8::ctype Data::getData() const {
    return getData<Int8>().getValue();
}

template <>
inline UInt8::ctype Data::getData() const {
    return getData<UInt8>().getValue();
}

template <>
inline Int16::ctype Data::getData() const {
    return getData<Int16>().getValue();
}

template <>
inline UInt16::ctype Data::getData() const {
    return getData<UInt16>().getValue();
}

template <>
inline Int32::ctype Data::getData() const {
    return getData<Int32>().getValue();
}

template <>
inline UInt32::ctype Data::getData() const {
    return getData<UInt32>().getValue();
}

template <>
inline Int64::ctype Data::getData() const {
    return getData<Int64>().getValue();
}

template <>
inline UInt64::ctype Data::getData() const {
    return getData<UInt64>().getValue();
}

template <>
inline Float32::ctype Data::getData() const {
    return getData<Float32>().getValue();
}

template <>
inline Float64::ctype Data::getData() const {
    return getData<Float64>().getValue();
}

} // namespace mdsplus

#endif // MDSPLUS_SCALAR_HPP