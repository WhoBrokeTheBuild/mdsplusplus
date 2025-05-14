#ifndef MDSPLUS_DATA_VIEW_HPP
#define MDSPLUS_DATA_VIEW_HPP

#include "Data.hpp"
#include "TreeNode.hpp"

#include <type_traits>
#include <typeindex>
#include <complex>
#include <vector>

extern "C" {

    // Needed for mdsdsc*_t, <dtypedef.h>, <classdef.h>
    #include <mdsdescrip.h>

} // extern "C"

namespace mdsplus {

template <typename T>
struct is_std_vector : std::false_type { };

template <typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type { };

template <typename T>
struct is_valid_ctype : std::false_type { };

// TODO: Rename? DataWrapper DataView
class DataView
{
public:

    DataView() = delete;

    DataView(const DataView&) = default;
    DataView& operator=(const DataView&) = default;

    inline mdsdsc_t * getDescriptor() const {
        if (_dsc.dtype == DTYPE_MISSING && _dsc.class_ == CLASS_MISSING) {
            return nullptr;
        }

        return (mdsdsc_t *)&_dsc;
    }

    inline Tree * getTree() const {
        return _tree;
    }

    inline DataView(std::nullptr_t)
        : _dsc(mdsdsc_a_t{
            .length = 0,
            .dtype = DTYPE_MISSING,
            .class_ = CLASS_MISSING,
            .pointer = nullptr,
        })
    { }

    // If the argument is already a MDSplus Data type, we only want to store a reference
    // to it, not take ownership, so we store a CLASS_S descriptor instead of a CLASS_XD.
    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline DataView(const DataType &value)
        : _dsc(mdsdsc_a_t{
            .length = 0,
            .dtype = DTYPE_DSC,
            .class_ = CLASS_S,
            .pointer = const_cast<char *>(reinterpret_cast<const char *>(value.getDescriptor())),
        })
        , _tree(value.getTree())
    { }

    template <typename CType,
        typename std::enable_if<is_valid_ctype<CType>::value, bool>::type = true>
    inline DataView(const std::vector<CType> &values)
        : _dsc(mdsdsc_a_t{
            .length = sizeof(CType),
            .dtype = _getDTypeForCType<CType>(),
            .class_ = CLASS_S,
            .pointer = const_cast<char *>(reinterpret_cast<const char *>(values.data())),
            .scale = 0,
            .digits = 0,
            .aflags = aflags_t{
                .binscale = false,
                .redim = true,
                .column = true,
                .coeff = false,
                .bounds = false,
            },
            .dimct = 0,
            .arsize = arsize_t(values.size() * sizeof(CType)),
        })
    { }

    #ifdef __cpp_lib_span

        template <typename CType,
            typename std::enable_if<is_valid_ctype<CType>::value, bool>::type = true>
        inline DataView(std::span<const CType> values)
            : _dsc(mdsdsc_a_t{
                .length = sizeof(CType),
                .dtype = _getDTypeForCType<CType>(),
                .class_ = CLASS_S,
                .pointer = const_cast<char *>(reinterpret_cast<const char *>(values.data())),
                .scale = 0,
                .digits = 0,
                .aflags = aflags_t{
                    .binscale = false,
                    .redim = true,
                    .column = true,
                    .coeff = false,
                    .bounds = false,
                },
                .dimct = 0,
                .arsize = arsize_t(values.size() * sizeof(CType)),
            })
        { }
        
    #endif

    template <typename CType,
        typename std::enable_if<is_valid_ctype<CType>::value, bool>::type = true>
    inline DataView(const CType &value)
        : _dsc(mdsdsc_a_t{
            .length = sizeof(CType),
            .dtype = _getDTypeForCType<CType>(),
            .class_ = CLASS_S,
            .pointer = const_cast<char *>(reinterpret_cast<const char *>(&value)),
        })
    { }

    // TODO: Improve?
    #ifdef __cpp_lib_string_view

        inline DataView(std::string_view value)
            : _dsc(mdsdsc_a_t{
                .length = length_t(value.size()),
                .dtype = DTYPE_T,
                .class_ = CLASS_S,
                .pointer = const_cast<char *>(value.data()),
            })
        { }

    #else

        inline DataView(const char * value)
            : _dsc(mdsdsc_a_t{
                .length = length_t(strlen(value)),
                .dtype = DTYPE_T,
                .class_ = CLASS_S,
                .pointer = const_cast<char *>(value),
            })
        { }

        inline DataView(const std::string& value)
            : _dsc(mdsdsc_a_t{
                .length = length_t(value.size()),
                .dtype = DTYPE_T,
                .class_ = CLASS_S,
                .pointer = const_cast<char *>(value.data()),
            })
        { }

    #endif // __cpp_lib_string_view

    inline DataView(const TreeNode& node)
        : _dsc(mdsdsc_a_t{
            .length = sizeof(node._nid),
            .dtype = DTYPE_NID,
            .class_ = CLASS_S,
            .pointer = const_cast<char *>(reinterpret_cast<const char *>(&node._nid)),
        })
        , _tree(node._tree)
    { }

private:

    mdsdsc_a_t _dsc = MDSDSC_XD_INITIALIZER;

    Tree * _tree = nullptr;

    template <typename CType>
    inline dtype_t _getDTypeForCType()
    {
        std::type_index id = typeid(CType);
        if (id == typeid(int8_t)) {
            return DTYPE_B;
        }
        else if (id == typeid(int16_t)) {
            return DTYPE_W;
        }
        else if (id == typeid(int32_t)) {
            return DTYPE_L;
        }
        else if (id == typeid(int64_t)) {
            return DTYPE_Q;
        }
        else if (id == typeid(uint8_t)) {
            return DTYPE_BU;
        }
        else if (id == typeid(uint16_t)) {
            return DTYPE_WU;
        }
        else if (id == typeid(uint32_t)) {
            return DTYPE_LU;
        }
        else if (id == typeid(uint64_t)) {
            return DTYPE_QU;
        }
        else if (id == typeid(float)) {
            return DTYPE_FS;
        }
        else if (id == typeid(double)) {
            return DTYPE_FT;
        }
        else if (id == typeid(std::complex<float>)) {
            return DTYPE_FSC;
        }
        else if (id == typeid(std::complex<double>)) {
            return DTYPE_FTC;
        }

        return DTYPE_MISSING;
    };

};

} // namespace mdsplus

#endif // MDSPLUS_DATA_VIEW_HPP