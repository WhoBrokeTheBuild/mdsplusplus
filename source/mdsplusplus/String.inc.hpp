#ifndef MDSPLUS_STRING_INC_HPP
#define MDSPLUS_STRING_INC_HPP

#include "String.hpp"

namespace mdsplus {

inline void String::setValue(const char * value, length_t length)
{
    mdsdsc_s_t dsc = {
        length,
        DTYPE_T,
        CLASS_S,
        (char *)value
    };

    int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

inline std::vector<std::string> StringArray::getValues() const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc) {
        std::vector<std::string> values;
        for (size_t i = 0; i < size(); ++i) {
            values.emplace_back(dsc->pointer + (i * dsc->length), dsc->length);
        }
        return values;
    }
    return {};
}

inline std::string StringArray::getValueAt(size_t index) const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc && index < size()) {
        return std::string(dsc->pointer + (index * dsc->length), dsc->length);
    }

    throw TdiBadIndex();
}

inline size_t StringArray::getSize() const
{
    mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    if (dsc) {
        return dsc->arsize / dsc->length;
    }
    return 0;
}

inline void StringArray::setValues(
    const std::vector<std::string>& values,
    const std::vector<uint32_t>& dims /*= {}*/
) {
    length_t maxSize = 0;
    for (const auto& value : values) {
        if (value.size() > maxSize) {
            maxSize = value.size();
        }
    }

    std::string buffer;
    for (const auto& value : values) {
        buffer += value;
        for (length_t i = value.size(); i < maxSize; ++i) {
            buffer.push_back(' ');
        }
    }

    if (dims.empty()) {
        _setValues(buffer, maxSize, { length_t(values.size()) });
    }
    else {
        _setValues(buffer, maxSize, dims);
    }
}

#ifdef __cpp_lib_string_view

    inline void StringArray::setValues(
        const std::vector<std::string_view>& values,
        const std::vector<uint32_t>& dims /*= {}*/
    ) {
        length_t maxSize = 0;
        for (const auto& value : values) {
            if (value.size() > maxSize) {
                maxSize = value.size();
            }
        }

        std::string buffer;
        for (const auto& value : values) {
            buffer += value;
            for (length_t i = value.size(); i < maxSize; ++i) {
                buffer.push_back(' ');
            }
        }

        if (dims.empty()) {
            _setValues(buffer, maxSize, { length_t(values.size()) });
        }
        else {
            _setValues(buffer, maxSize, dims);
        }
    }

#endif // __cpp_lib_string_view

inline void StringArray::_setValues(
    const std::string& buffer,
    length_t length,
    const std::vector<uint32_t>& dims
) {
    array_coeff dsc = {
        .length = length,
        .dtype = DTYPE_T,
        .class_ = CLASS_A,
        .pointer = const_cast<char *>(buffer.data()),
        .scale = 0,
        .digits = 0,
        .aflags = aflags_t{
            .binscale = false,
            .redim = true,
            .column = true,
            .coeff = true,
            .bounds = false,
        },
        .dimct = dimct_t(dims.size()),
        .arsize = arsize_t(buffer.size()),
        .a0 = const_cast<char *>(buffer.data()),
        .m = { 0 },
    };

    for (size_t i = 0; i < dims.size(); ++i) {
        dsc.m[i] = dims[i];
    }

    int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

} // namespace mdsplus

#endif // MDSPLUS_STRING_INC_HPP