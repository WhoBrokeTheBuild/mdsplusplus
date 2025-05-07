#ifndef MDSPLUS_STRING_HPP
#define MDSPLUS_STRING_HPP

#include "Data.hpp"

#if __has_include(<string_view>)
    #include <string_view>
#endif

namespace mdsplus {

class String : public Data
{
public:

    static constexpr Class __class = Class::S;
    static constexpr DType __dtype = DType::T;

    String() = default;

    String(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    #ifdef __cpp_lib_string_view

        inline String(std::string_view value) {
            setValue(value);
        }

    #else

        inline String(const std::string& value) {
            setValue(value);
        }

    #endif // __cpp_lib_string_view

    inline String(const char *value, length_t length) {
        setValue(value, length);
    }

    String(String&&) = default;
    String& operator=(String&&) = default;

    [[nodiscard]]
    inline String clone() const {
        return _clone<String>();
    }
    
    inline std::string getValue() const {
        return getString();
    }

    inline std::string getString() const {
        mdsdsc_t * dsc = getDescriptor();
        return std::string(dsc->pointer, dsc->length);
    }

    #ifdef __cpp_lib_string_view

        // Warning: this will not be null-terminated
        inline std::string_view getStringView() const {
            mdsdsc_t * dsc = getDescriptor();
            return std::string_view(dsc->pointer, dsc->length);
        }

    #endif // __cpp_lib_string_view

    #ifdef __cpp_lib_string_view

        void setValue(std::string_view value) {
            setValue(value.data(), value.size());
        }

    #else

        void setValue(const std::string& value) {
            setValue(value.data(), value.size());
        }

    #endif // __cpp_lib_string_view

    void setValue(const char * value, length_t length);

}; // class String

// TODO: Decide if to_string(String) should be different
inline std::string to_string(const String * data) {
    return data->getString();
}

inline std::string to_string(const String& data) {
    return data.getString();
}

template <>
inline String Data::releaseAndConvert()
{
    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if (dsc->class_ == CLASS_S && dsc->dtype == DTYPE_T) {
        return String(std::move(xd), getTree());
    }

    // TODO: Call DATA()?

    mdsdsc_xd_t xdDeco = MDSDSC_XD_INITIALIZER;
    int status = _intrinsic(OPC_DECOMPILE, 1, &dsc, &xdDeco);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    MdsFree1Dx(&xd, nullptr);
    return String(std::move(xdDeco), getTree());
}

template <>
inline std::string Data::getData() const {
    return getData<String>().getString();
}

#ifdef __cpp_lib_string_view

    template <>
    inline Data Data::FromScalar(std::string_view value) {
        return String(value);
    }

#else

    template <>
    inline Data Data::FromScalar(const char * value) {
        return String(value);
    }

    template <>
    inline Data Data::FromScalar(const std::string& value) {
        return String(value);
    }

#endif // __cpp_lib_string_view

class StringArray : public Data
{
public:

    static constexpr Class __class = Class::A;
    static constexpr DType __dtype = DType::T;

    StringArray() = default;

    inline StringArray(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    #ifdef __cpp_lib_span

        inline StringArray(std::span<const std::string> values, const std::vector<uint32_t>& dims = {}) {
            setValues(values, dims);
        }
    
    #else

        inline StringArray(const std::vector<std::string>& values, const std::vector<uint32_t>& dims = {}) {
            setValues(values, dims);
            setTree(tree);
        }

    #endif // __cpp_lib_span

    inline StringArray(std::initializer_list<std::string> values, const std::vector<uint32_t>& dims = {}) {
        std::vector<std::string> tmp(values);
        setValues(tmp, dims);
    }

    #ifdef __cpp_lib_string_view

        inline StringArray(std::initializer_list<std::string_view> values, const std::vector<uint32_t>& dims = {}) {
            std::vector<std::string_view> tmp(values);
            setValues(tmp, dims);
        }

    #endif // __cpp_lib_string_view

    StringArray(StringArray&&) = default;
    StringArray& operator=(StringArray&&) = default;

    [[nodiscard]]
    inline mdsdsc_a_t * getArrayDescriptor() const {
        return reinterpret_cast<mdsdsc_a_t *>(_xd.pointer);
    }

    [[nodiscard]]
    inline StringArray clone() const {
        return _clone<StringArray>();
    }

    [[nodiscard]]
    std::vector<std::string> getValues() const;

    [[nodiscard]]
    std::string getValueAt(size_t index) const;

    [[nodiscard]]
    size_t getSize() const;

    void setValues(
        const std::vector<std::string>& values,
        const std::vector<uint32_t>& dims = {}
    );

    #ifdef __cpp_lib_string_view

        void setValues(
            const std::vector<std::string_view>& values,
            const std::vector<uint32_t>& dims = {}
        );

    #endif // __cpp_lib_string_view

#ifdef __cpp_lib_span

        inline void setValues(
            std::span<const std::string> values,
            const std::vector<uint32_t>& dims = {}
        ) {
            setValues(values, dims);
        }

    #endif

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
    inline std::string at(size_t index) const {
        return getValueAt(index);
    }

    struct Iterator
    {
    public:

        Iterator(size_t index, const StringArray * array)
            : _index(index)
            , _array(array)
        { }

        inline std::string operator*() const {
            return _array->getValueAt(_index);
        }

        inline Iterator& operator++() {
            _index++;
            return *this;
        }
    
        inline Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
    
        inline friend bool operator==(const Iterator& a, const Iterator& b) {
            return (a._index == b._index && a._array == b._array);
        };

        inline friend bool operator!=(const Iterator& a, const Iterator& b) {
            return (a._index != b._index || a._array != b._array);
        };

    private:

        size_t _index;

        const StringArray * _array;

    };

    inline Iterator begin() const {
        return Iterator(0, this);
    }

    inline Iterator end() const {
        return Iterator(getSize(), this);
    }

protected:

    void _setValues(
        const std::string& buffer,
        length_t length,
        const std::vector<uint32_t>& dims
    );

}; // class StringArray

template <>
inline StringArray Data::releaseAndConvert()
{
    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if (dsc->class_ == CLASS_A && dsc->dtype == DTYPE_T) {
        return StringArray(std::move(xd), getTree());
    }
    else if (dsc->class_ == CLASS_S && dsc->dtype == DTYPE_T) {
        #ifdef __cpp_lib_string_view
            return StringArray({ std::string_view(dsc->pointer, dsc->length) });
        #else
            return StringArray({ std::string(dsc->pointer, dsc->length) });
        #endif
    }

    MdsFree1Dx(&xd, nullptr);
    throw TdiInvalidDataType();
}

} // namespace mdsplus

namespace std {

template <> struct hash<mdsplus::String>
{
    size_t operator()(const mdsplus::String& data) const
    {
        #ifdef __cpp_lib_string_view
            return std::hash<std::string_view>()(data.getStringView());
        #else
            return std::hash<std::string>()(data.getString());
        #endif
    }
};

} // namespace std

#endif // MDSPLUS_STRING_HPP