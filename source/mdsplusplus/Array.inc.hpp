#ifndef MDSPLUS_ARRAY_INC_HPP
#define MDSPLUS_ARRAY_INC_HPP

#include "Array.hpp"

namespace mdsplus {

inline std::vector<uint32_t> Array::getDimensions() const
{
    mdsdsc_a_t * dsc = getArrayDescriptor();
    if (dsc) {
        if (dsc->aflags.coeff) {
            array_coeff * dscCoeff = reinterpret_cast<array_coeff *>(dsc);
            return std::vector<uint32_t>(
                (uint32_t *)dscCoeff->m,
                (uint32_t *)(dscCoeff->m + dscCoeff->dimct)
            );
        }
        else {
            return { (dsc->arsize / dsc->length) };
        }
    }

    return {};
}

template <typename CType>
inline void Array::_setValues(DType dtype, const CType * values, const uint32_t * dims, dimct_t dimCount)
{
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

    int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

#ifdef __cpp_lib_span

    template <typename CType>
    inline std::span<CType> Array::_getSpan() const {
        mdsdsc_a_t * dsc = getArrayDescriptor();
        if (dsc) {
            return std::span<CType>(
                reinterpret_cast<CType *>(dsc->pointer),
                reinterpret_cast<CType *>(dsc->pointer + dsc->arsize)
            );
        }
        return {};
    }

#endif // __cpp_lib_span

template <typename CType>
inline std::vector<CType> Array::_getVector() const {
    mdsdsc_a_t * dsc = getArrayDescriptor();
    if (dsc) {
        return std::vector<CType>(
            reinterpret_cast<CType *>(dsc->pointer),
            reinterpret_cast<CType *>(dsc->pointer + dsc->arsize)
        );
    }
    return {};
}

template <typename CType>
inline const CType& Array::_getValueAt(size_t index) const {
    mdsdsc_a_t * dsc = getArrayDescriptor();
    if (dsc && index < size()) {
        return *((CType *)dsc->pointer + index);
    }

    throw TdiBadIndex();
}

template <typename CType>
inline CType * Array::_begin() const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc) {
        return reinterpret_cast<CType *>(dsc->pointer);
    }
    
    return nullptr;
}

template <typename CType>
inline CType * Array::_end() const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc) {
        return reinterpret_cast<CType *>(dsc->pointer + dsc->arsize);
    }
    
    return nullptr;
}

template <typename ResultType /*= Data*/>
inline ResultType Int8Array::deserialize()
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = MdsSerializeDscIn(getDescriptor()->pointer, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

template <typename ResultType /*= Data*/>
inline ResultType UInt8Array::deserialize()
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = MdsSerializeDscIn(getDescriptor()->pointer, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

} // namespace mdsplus

#endif // MDSPLUS_ARRAY_INC_HPP