#ifndef MDSPLUS_APD_HPP
#define MDSPLUS_APD_HPP

#include <map>
#include <unordered_map>

#include "Array.hpp"
#include "DataView.hpp"

namespace mdsplus {


class APD : public Array
{
public:

    static constexpr Class __class = Class::APD;

    APD() = default;

    inline APD(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Array(std::move(dsc), tree)
    { }

    APD(APD&&) = default;
    APD& operator=(APD&&) = default;

    template <typename ...ArgTypes>
    inline APD(DType dtype, const ArgTypes& ...args) {
        _append(dtype, args...);
    }

protected:

    inline mdsdsc_t ** _getDescriptorArray() const {
        return reinterpret_cast<mdsdsc_t **>(getPointer());
    }

    template <typename ...ArgTypes>
    inline void _append(DType dtype, const ArgTypes& ...args)
    {
        std::vector<DataView> argList = { DataView(args)... };

        std::vector<mdsdsc_t *> dscList;
        for (size_t i = 0; i < argList.size(); ++i) {
            dscList.push_back(argList[i].getDescriptor());
        }

        _append(dtype, dscList);
    }

    inline void _append(DType dtype, std::vector<mdsdsc_t *> values)
    {
        std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        dscList.reserve(dscList.size() + values.size());
        dscList.insert(dscList.end(), values.begin(), values.end());
        
        DESCRIPTOR_APD(dsc, dtype_t(dtype), dscList.data(), 0);

        // TODO: Replace the 0 above with dscList.size() once a cast to arsize_t is added
        dsc.arsize = arsize_t(dscList.size() * sizeof(mdsdsc_t *));

        mdsdsc_xd_t tmpXD = std::move(_xd);

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        MdsFree1Dx(&tmpXD, nullptr);
    }

}; // class APD

class List : public APD
{
public:

    static constexpr DType __dtype = DType::List;

    inline List(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : APD(std::move(dsc), tree)
    { }

    template <typename ...ArgTypes>
    inline List(const ArgTypes& ...args)
        : APD(__dtype, args...)
    { }

    template <typename DataType>
    inline List(const std::vector<DataType>& values)
    {
        std::vector<mdsdsc_t *> dscList;
        for (size_t i = 0; i < values.size(); ++i) {
            dscList.push_back(values[i].getDescriptor());
        }

        _append(__dtype, dscList);
    }

    inline const char * getClassName() const override {
        return "List";
    }

    template <typename ...ArgTypes>
    inline void append(ArgTypes ...args) {
        _append(__dtype, args...);
    }

    inline std::vector<Data> getValues() const
    {
        int status;

        #ifdef __cpp_lib_span
            std::span<mdsdsc_t *> dscList = _getSpan<mdsdsc_t *>();
        #else
            std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        #endif

        std::vector<Data> values;
        for (mdsdsc_t * dsc : dscList) {
            mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
            status = MdsCopyDxXd(dsc, &out);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            values.emplace_back(Data(std::move(out), getTree()));
        }

        return std::move(values);
    }

    // STL Compatibility

    struct Iterator
    {
    public:

        Iterator(size_t index, const List * list)
            : _index(index)
            , _list(list)
        { }

        inline Data operator*() const
        {
            mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
            mdsdsc_a_t * dsc = _list->getArrayDescriptor();

            mdsdsc_t * tmp = _list->_getDescriptorArray()[_index];
            int status = MdsCopyDxXd(tmp, &out);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            return Data(std::move(out), _list->getTree());
        }

        inline mdsdsc_t * operator->() const {
            return _list->_getDescriptorArray()[_index];
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
            return (a._index == b._index && a._list == b._list);
        };

        inline friend bool operator!=(const Iterator& a, const Iterator& b) {
            return (a._index != b._index || a._list != b._list);
        };

    private:

        size_t _index;

        const List * _list;

    };

    inline Iterator begin() const {
        return Iterator(0, this);
    }

    inline Iterator end() const {
        return Iterator(getSize(), this);
    }

}; // class List

class Dictionary : public APD
{
public:

    static constexpr DType __dtype = DType::Dictionary;

    inline Dictionary(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : APD(std::move(dsc), tree)
    { }

    template <typename KeyType, typename ValueType,
        typename std::enable_if<is_valid_ctype<KeyType>::value, bool>::type = true,
        typename std::enable_if<is_valid_ctype<ValueType>::value, bool>::type = true>
    inline Dictionary(const std::map<KeyType, ValueType>& map)
    {
        std::vector<Data> values;
        for (const auto& it : map) {
            values.emplace_back(Data::FromScalar(it->first));
            values.emplace_back(Data::FromScalar(it->second));
        }
    }

    inline Dictionary(const std::vector<Data>& keys, const std::vector<Data>& values)
    {
        assert(keys.size() == values.size());

        std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        for (size_t i = 0; i < keys.size(); ++i) {
            dscList.push_back(keys[i].getDescriptor());
            dscList.push_back(values[i].getDescriptor());
        }
        
        DESCRIPTOR_APD(dsc, dtype_t(__dtype), dscList.data(), 0);

        // TODO: Replace the 0 above with dscList.size() once a cast to arsize_t is added
        dsc.arsize = arsize_t(dscList.size() * sizeof(mdsdsc_t *));

        int status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
    }

    template <typename ...ArgTypes,
        typename std::enable_if<(sizeof...(ArgTypes) % 2) == 0, bool>::type = true>
    inline Dictionary(const ArgTypes& ...args)
        : APD(__dtype, args...)
    { }

    inline const char * getClassName() const override {
        return "Dictionary";
    }

    template <typename KeyType, typename ValueType>
    void append(KeyType key, ValueType value) {
        _append(__dtype, key, value);
    }

    inline std::tuple<std::vector<Data>, std::vector<Data>> getKeysAndValues() const {
        int status;

        #ifdef __cpp_lib_span
            std::span<mdsdsc_t *> dscList = _getSpan<mdsdsc_t *>();
        #else
            std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        #endif

        assert((dscList.size() % 2) == 0);

        std::vector<Data> keys;
        std::vector<Data> values;
        for (size_t i = 0; i < dscList.size(); i += 2) {
            mdsdsc_xd_t outKey = MDSDSC_XD_INITIALIZER;
            mdsdsc_xd_t outValue = MDSDSC_XD_INITIALIZER;

            status = MdsCopyDxXd(dscList[i], &outKey);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            status = MdsCopyDxXd(dscList[i + 1], &outValue);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }
            
            // keys.emplace(std::move(outKey), getTree());
            // values.emplace(std::move(outValue), getTree());
        }


        return { std::move(keys), std::move(values) };
    }

    template <typename KeyType = Data, typename ValueType = Data,
        typename std::enable_if<std::is_base_of<Data, KeyType>::value, bool>::type = true,
        typename std::enable_if<std::is_base_of<Data, ValueType>::value, bool>::type = true>
    inline std::unordered_map<KeyType, ValueType> getUnorderedMap() const {
        int status;

        #ifdef __cpp_lib_span
            std::span<mdsdsc_t *> dscList = _getSpan<mdsdsc_t *>();
        #else
            std::vector<mdsdsc_t *> dscList = _getVector<mdsdsc_t *>();
        #endif
        
        assert((dscList.size() % 2) == 0);

        std::unordered_map<KeyType, ValueType> values;
        for (size_t i = 0; i < dscList.size(); i += 2) {
            mdsdsc_xd_t outKey = MDSDSC_XD_INITIALIZER;
            mdsdsc_xd_t outValue = MDSDSC_XD_INITIALIZER;

            status = MdsCopyDxXd(dscList[i], &outKey);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            status = MdsCopyDxXd(dscList[i + 1], &outValue);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }
            
            values.emplace(
                Data(std::move(outKey), getTree()).releaseAndConvert<KeyType>(),
                Data(std::move(outValue), getTree()).releaseAndConvert<ValueType>()
            );
        }

        return std::move(values);
    }

}; // class Dictionary

// TODO: Tuple

} // namespace mdsplus

#endif // MDSPLUS_APD_HPP