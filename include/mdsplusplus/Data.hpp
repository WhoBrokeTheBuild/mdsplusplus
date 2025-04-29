#ifndef MDSPLUS_DATA_HPP
#define MDSPLUS_DATA_HPP

#include "MDSplusException.hpp"

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#ifdef __cpp_lib_span
    #include <span>
#endif

extern "C" {

    // Needed for mdsdsc*_t, <dtypedef.h>, <classdef.h>
    #include <mdsdescrip.h>

    // Needed for MdsFree1Dx
    #include <mdsshr.h>

    // Contains invalid definition for TdiConvert()
    // #include <tdishr.h>

    int _TdiIntrinsic(void **ctx, opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);
    int TdiIntrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);

    int TdiConvert(mdsdsc_a_t * dsc, mdsdsc_a_t * convert);

    int TdiCall(dtype_t rtype, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);

} // extern "C"

namespace mdsplus {

enum class Class : uint8_t
{
    Missing = CLASS_MISSING,
    S = CLASS_S,
    D = CLASS_D,
    A = CLASS_A,
    XD = CLASS_XD,
    XS = CLASS_XS,
    R = CLASS_R,
    CA = CLASS_CA,
    APD = CLASS_APD,

}; // enum class Class

std::string to_string(const Class& class_);

inline constexpr bool operator==(const Class lhs, const class_t rhs) {
    return lhs == static_cast<Class>(rhs);
}

enum class DType : uint8_t
{
    Missing = DTYPE_MISSING,

    BU = DTYPE_BU,
    WU = DTYPE_WU,
    LU = DTYPE_LU,
    QU = DTYPE_QU,
    B = DTYPE_B,
    W = DTYPE_W,
    L = DTYPE_L,
    Q = DTYPE_Q,
    F = DTYPE_F,
    D = DTYPE_D,
    FC = DTYPE_FC,
    DC = DTYPE_DC,
    T = DTYPE_T,
    DSC = DTYPE_DSC,
    OU = DTYPE_OU,
    O = DTYPE_O,
    G = DTYPE_G,
    H = DTYPE_H,
    GC = DTYPE_GC,
    HC = DTYPE_HC,
    FS = DTYPE_FS,
    FT = DTYPE_FT,
    FSC = DTYPE_FSC,
    FTC = DTYPE_FTC,
    C = DTYPE_C,

    Float = DTYPE_FLOAT,
    Double = DTYPE_DOUBLE,

    Pointer = DTYPE_POINTER,
    Ident = DTYPE_IDENT,
    NID = DTYPE_NID,
    Path = DTYPE_PATH,
    Param = DTYPE_PARAM,
    Signal = DTYPE_SIGNAL,
    Dimension = DTYPE_DIMENSION,
    Window = DTYPE_WINDOW,
    Slope = DTYPE_SLOPE,
    Function = DTYPE_FUNCTION,
    Conglom = DTYPE_CONGLOM,
    Range = DTYPE_RANGE,
    Action = DTYPE_ACTION,
    Dispatch = DTYPE_DISPATCH,
    Program = DTYPE_PROGRAM,
    Routine = DTYPE_ROUTINE,
    Procedure = DTYPE_PROCEDURE,
    Method = DTYPE_METHOD,
    Dependency = DTYPE_DEPENDENCY,
    Condition = DTYPE_CONDITION,
    Event = DTYPE_EVENT,
    WithUnits = DTYPE_WITH_UNITS,
    Call = DTYPE_CALL,
    WithError = DTYPE_WITH_ERROR,
    List = DTYPE_LIST,
    Tuple = DTYPE_TUPLE,
    Dictionary = DTYPE_DICTIONARY,
    Opaque = DTYPE_OPAQUE,

}; // enum class DType

std::string to_string(const DType& dtype);

inline constexpr bool operator==(const DType lhs, const dtype_t rhs) {
    return lhs == static_cast<DType>(rhs);
}

class Tree;

class Data
{
public:

    typedef std::nullptr_t ctype;
    static constexpr Class class_ = Class::Missing;
    static constexpr DType dtype = DType::Missing;

    template <typename T = Data, typename ...Args>
    static T Compile(std::string_view expression, Args... args);

    template <typename T = Data, typename ...Args>
    static T Execute(std::string_view expression, Args... args);

    template <typename T>
    static Data FromScalar(T value);

#ifdef __cpp_lib_span
    template <typename T>
    static Data FromArray(const std::span<T> values, const std::vector<uint32_t>& dims = {});
#endif

    template <typename T>
    static Data FromArray(const std::vector<T>& values, const std::vector<uint32_t>& dims = {});

    template <typename T = Data, typename ...Args>
    static inline T FromExpression(std::string_view expression, Args... args) {
        return Execute<T>(expression, args...);
    }

    inline Data() = default;

    inline Data(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : _xd(xd)
        , _tree(tree)
    {
        xd = MDSDSC_XD_INITIALIZER;
    }

    inline virtual ~Data() {
        MdsFree1Dx(&_xd, nullptr);
    }

    // Disallow copy and assign
    Data(const Data&) = delete;
    Data& operator=(Data&) = delete;

    // Enable move operators
    inline Data(Data&& other) {
        _xd = other._xd;
        other._xd = MDSDSC_XD_INITIALIZER;
        _tree = other._tree;
        other._tree = nullptr;
    }

    Data& operator=(Data&& other) {
        _xd = other._xd;
        other._xd = MDSDSC_XD_INITIALIZER;
        _tree = other._tree;
        other._tree = nullptr;
        return *this;
    }

    template <typename T, std::enable_if_t<std::is_base_of<Data, T>::value, bool> = true>
    inline bool operator==(const T& other) const {
        int status;

        // TODO: Should this always be the case?
        if (getTree() != other.getTree()) {
            return false;
        }

        bool equal = false;
        mdsdsc_s_t out = {
            .length = 1,
            .dtype = DTYPE_BU,
            .class_ = CLASS_S,
            .pointer = (char *)&equal
        };

        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_EQ, 2, args, (mdsdsc_xd_t *)&out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return equal;
    }

    template <typename T, std::enable_if_t<std::is_base_of<Data, T>::value, bool> = true>
    inline bool operator!=(const T& other) const {
        return !(*this == other);
    }

    inline void setTree(Tree * tree) {
        _tree = tree;
    }

    inline Tree * getTree() const {
        return _tree;
    }

    inline bool hasTree() const {
        return (_tree != nullptr);
    }

    [[nodiscard]]
    inline mdsdsc_xd_t release() {
        mdsdsc_xd_t tmp = std::move(_xd);
        _xd = MDSDSC_XD_INITIALIZER;
        return tmp;
    }

    [[nodiscard]]
    inline Data clone() const {
        return _clone<Data>();
    }

    template <typename T>
    [[nodiscard]]
    inline T convert() const {
        return clone().releaseAndConvert<T>();
    }

    template <typename T>
    [[nodiscard]]
    T releaseAndConvert();

    [[nodiscard]]
    inline mdsdsc_t * getDescriptor() const {
        return _xd.pointer;
    }

    [[nodiscard]]
    inline length_t getLength() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? dsc->length : 0);
    }

    [[nodiscard]]
    inline Class getClass() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? Class(dsc->class_) : Class::Missing);
    }

    [[nodiscard]]
    inline DType getDType() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? DType(dsc->dtype) : DType::Missing);
    }

    [[nodiscard]]
    inline void * getPointer() const {
        mdsdsc_t * dsc = getDescriptor();
        return (dsc ? dsc->pointer : nullptr);
    }

    template <typename T = Data>
    inline T getData() const {
        return clone().convert<T>();
    }

    template <typename T = Data>
    T getUnits() const;

    template <typename T = Data, typename U = Data>
    std::tuple<T, U> getDataWithUnits() const;

    template <typename T = Data>
    T getError() const;

    template <typename T = Data>
    T getHelp() const;

    template <typename T = Data>
    T getValidation() const;

    std::string decompile() const;

    // UInt8Array serialize() const;

protected:

    mdsdsc_xd_t _xd = MDSDSC_XD_INITIALIZER;

    Tree * _tree = nullptr;

    int _intrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t * out) const;

    template <typename T>
    inline T _clone() const {
        int status;

        mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
        status = MdsCopyDxXd(getDescriptor(), &xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return T(std::move(xd), getTree());
    }

}; // class Data

static const Data EmptyData;

std::string to_string(const Data * data);

inline std::string to_string(const Data& data) {
    return to_string(&data);
}

template <>
inline Data Data::FromScalar(Data value) {
    return value;
}

template <>
inline Data Data::FromScalar(std::nullptr_t) {
    return Data();
}

class String : public Data
{
public:

    typedef char * ctype;
    static constexpr Class class_ = Class::S;
    static constexpr DType dtype = DType::T;

    String() = default;

    String(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    inline String(std::string_view value, Tree * tree = nullptr) {
        setValue(value);
        setTree(tree);
    }

    String(String&&) = default;
    String& operator=(String&&) = default;

    [[nodiscard]]
    inline String clone() const {
        return _clone<String>();
    }

    void setValue(std::string_view value);

    std::string getValue() const {
        mdsdsc_t * dsc = getDescriptor();
        return std::string(dsc->pointer, dsc->length);
    }

}; // class String

template <>
inline Data Data::FromScalar(const char * value) {
    return String(value);
}

template <>
inline Data Data::FromScalar(std::string value) {
    return String(value);
}

#ifdef __cpp_lib_string_view
template <>
inline Data Data::FromScalar(std::string_view value) {
    return String(value);
}
#endif

template <>
inline std::string Data::getData() const {
    return getData().releaseAndConvert<String>().getValue();
}

// TODO: Rename
static inline constexpr mdsdsc_t StringViewToDescriptor(std::string_view str) {
    return {
        .length = length_t(str.length()),
        .dtype = DTYPE_T,
        .class_ = CLASS_S,
        .pointer = const_cast<char *>(str.data()),
    };
}

class StringArray : public Data
{
public:

    static constexpr Class class_ = Class::A;
    static constexpr DType dtype = DType::T;

    StringArray() = default;

    inline StringArray(mdsdsc_xd_t && xd, Tree * tree = nullptr)
        : Data(std::move(xd), tree)
    { }

    inline StringArray(const std::vector<std::string>& values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        setValues(values, dims);
        setTree(tree);
    }

#ifdef __cpp_lib_span
    inline StringArray(std::span<std::string> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        setValues(values, dims);
        setTree(tree);
    }
#endif

    inline StringArray(std::initializer_list<std::string> values, const std::vector<uint32_t>& dims = {}, Tree * tree = nullptr) {
        std::vector<std::string> tmp(values);
        setValues(tmp, dims);
        setTree(tree);
    }

    StringArray(StringArray&&) = default;
    StringArray& operator=(StringArray&&) = default;

    [[nodiscard]]
    inline StringArray clone() const {
        return _clone<StringArray>();
    }

    void setValues(const std::vector<std::string>& values, const std::vector<uint32_t>& dims = {});

#ifdef __cpp_lib_span
    void setValues(std::span<std::string> values, const std::vector<uint32_t>& dims = {}) {
        setValues(values, dims);
    }
#endif

    [[nodiscard]]
    inline std::vector<std::string> getValues() const {
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

    [[nodiscard]]
    inline std::string getValueAt(size_t index) const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
        if (dsc && index < size()) {
            return std::string(dsc->pointer + (index * dsc->length), dsc->length);
        }

        throw TdiBadIndex();
    }

    [[nodiscard]]
    inline size_t getSize() const {
        mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
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
    inline std::string at(size_t index) const {
        return getValueAt(index);
    }

    // TODO: StringArrayIterator

    // [[nodiscard]]
    // inline ctype * begin() const {
    //     mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    //     return (dsc ? (ctype *)dsc->pointer : nullptr);
    // }

    // [[nodiscard]]
    // inline ctype * end() const {
    //     mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    //     return (dsc ? (ctype *)(dsc->pointer + dsc->arsize) : nullptr);
    // }

}; // class StringArray

template <typename T>
T Data::releaseAndConvert()
{
    int status;

    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if constexpr (std::is_same_v<Data, T>) {
        return T(std::move(xd), getTree());
    }
    else if constexpr (std::is_base_of_v<Data, T>) {

        if (Class(dsc->class_) == T::class_ && DType(dsc->dtype) == T::dtype) {
            return T(std::move(xd), getTree());
        }

        // TODO: Check edge cases for when we need to call DATA()
        if (Class(dsc->class_) == Class::R) {
            mdsdsc_xd_t dataXD = MDSDSC_XD_INITIALIZER;
            status = _intrinsic(OPC_DATA, 1, &dsc, &dataXD);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            MdsFree1Dx(&xd, nullptr);

            xd = dataXD;
            dsc = xd.pointer;

            if (Class(dsc->class_) == T::class_ && DType(dsc->dtype) == T::dtype) {
                return T(std::move(xd), getTree());
            }
        }

        // TODO: Is there a better way to convert to a string?
        if constexpr (std::is_same_v<String, T>) {
            mdsdsc_xd_t decoXD = MDSDSC_XD_INITIALIZER;
            status = _intrinsic(OPC_DECOMPILE, 1, &dsc, &decoXD);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            MdsFree1Dx(&xd, nullptr);
            return String(std::move(decoXD), getTree());
        }
        else if constexpr (T::class_ == Class::S) {
            typename T::ctype value = {};

            mdsdsc_t convert = {
                .length = sizeof(typename T::ctype),
                .dtype = dtype_t(T::dtype),
                .class_ = CLASS_S,
                .pointer = (char *)&value,
            };
    
            status = TdiConvert((mdsdsc_a_t *)dsc, (mdsdsc_a_t *)&convert);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }
    
            MdsFree1Dx(&xd, nullptr);
            return T(value, getTree());
        }
        else if constexpr (T::class_ == Class::A && T::dtype != DType::T) {
            mdsdsc_a_t arrayOfOne = {
                .length = sizeof(typename T::ctype),
                .dtype = dtype_t(T::dtype),
                .class_ = CLASS_A,
                .pointer = dsc->pointer,
                .scale = 0,
                .digits = 0,
                .aflags = {
                    .binscale = false,
                    .redim = true,
                    .column = true,
                    .coeff = false,
                    .bounds = false,
                },
                .dimct = 1,
                .arsize = sizeof(typename T::ctype),
            };

            array_coeff * dscArray;
            if (dsc->class_ == CLASS_A) {
                dscArray = (array_coeff *)dsc;
            }
            else {
                dscArray = (array_coeff *)&arrayOfOne;
            }

            size_t size = (dscArray->arsize / dscArray->length);
            std::vector<typename T::ctype> values(size);
            array_coeff convert = {
                .length = sizeof(typename T::ctype),
                .dtype = dtype_t(T::dtype),
                .class_ = CLASS_A,
                .pointer = (char *)values.data(),
                .scale = dscArray->scale,
                .digits = dscArray->digits,
                .aflags = dscArray->aflags,
                .dimct = dscArray->dimct,
                .arsize = arsize_t(size * sizeof(typename T::ctype)),
                .a0 = (char *)values.data(),
                .m = { 0 },
            };

            std::vector<uint32_t> dims(dscArray->dimct);
        
            if (dscArray->aflags.coeff) {
                for (dimct_t i = 0; i < dscArray->dimct; ++i) {
                    convert.m[i] = dscArray->m[i];
                    dims[i] = dscArray->m[i];
                }
            }
        
            status = TdiConvert((mdsdsc_a_t *)dscArray, (mdsdsc_a_t *)&convert);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }
    
            MdsFree1Dx(&xd, nullptr);
            return T(values, dims, getTree());
        }
    }

    MdsFree1Dx(&xd, nullptr);
    throw TdiInvalidDataType();
}

template <typename T /*= Data*/>
inline T Data::getUnits() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    status = _intrinsic(OPC_UNITS_OF, 1, &dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<T>();
}

template <typename T /*= Data*/, typename U /*= Data*/>
std::tuple<T, U> Data::getDataWithUnits() const
{
    // We could use OPC_DATA_WITH_UNITS to upcast it to a WithUnits() type,
    // but returning a tuple is a cleaner API
    return { getData<T>(), getUnits<U>() };
}

template <typename T /*= Data*/>
inline T Data::getError() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    status = _intrinsic(OPC_ERROR_OF, 1, &dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<T>();
}

template <typename T /*= Data*/>
inline T Data::getHelp() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    status = _intrinsic(OPC_HELP_OF, 1, &dsc, &out);
    if (status == TdiINVDTYDSC) {
        // Unlike UNITS_OF and ERROR_OF, this throws an error when there is no value
        return T();
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<T>();
}

template <typename T /*= Data*/>
inline T Data::getValidation() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    status = _intrinsic(OPC_ERROR_OF, 1, &dsc, &out);
    if (status == TdiINVDTYDSC) {
        // Unlike UNITS_OF and ERROR_OF, this throws an error when there is no value
        return T();
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<T>();
}

template <typename T>
struct is_std_vector : std::false_type { };

template <typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type { };

template <typename T>
inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

// T = int, std::string, Float32Array, std::vector<float>

// template <typename T>
// mdsdsc_xd_t _getArgument(const T &value);

class Argument
{
public:

    Argument() = delete;

    Argument(const Argument&) = default;
    Argument& operator=(const Argument&) = default;

    inline Argument(std::nullptr_t)
        : _dsc({
            .length = 0,
            .dtype = DTYPE_MISSING,
            .class_ = CLASS_MISSING,
            .pointer = nullptr,
            .l_length = 0,
        })
    { }

    // If the argument is already a MDSplus Data type, we only want to store a reference
    // to it, not take ownership, so we store a CLASS_S descriptor instead of a CLASS_XD.
    template <typename T, std::enable_if_t<std::is_base_of_v<Data, T>, bool> = true>
    inline Argument(const T &value)
        : _dsc({
            .length = 0,
            .dtype = DTYPE_DSC,
            .class_ = CLASS_S,
            .pointer = value.getDescriptor(),
            .l_length = 0,
        })
    { }

#ifdef __cpp_lib_span
    template <typename T>
    inline Argument(const std::span<T> value)
        : _dsc(Data::FromArray(value).release())
    { }
#endif

    template <typename T>
    inline Argument(const std::vector<T> &value)
        : _dsc(Data::FromArray(value).release())
    { }

    template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
    inline Argument(const T &value)
        : _dsc(Data::FromScalar(value).release())
    { }

    template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
    inline Argument(const T &value)
        : _dsc(Data::FromScalar(value).release())
    { }

    inline Argument(const std::string &value)
        : _dsc(Data::FromScalar(value).release())
    { }

    // template <typename T,
    //     std::enable_if_t<std::is_same_v<std::string_view, T>, bool> = true>
    // inline Argument(const T &value) {
    //     dsc = Data::FromScalar(value).release();
    // }

    inline mdsdsc_t * getDescriptor() const {
        return _dsc.pointer;
    }

    inline void free() {
        if (_dsc.class_ == CLASS_XD) {
            MdsFree1Dx(&_dsc, nullptr);
        }
    }

// private:

    mdsdsc_xd_t _dsc = MDSDSC_XD_INITIALIZER;
};


// template <typename T>
// inline mdsdsc_xd_t _getArgument(const T& value)
// {
//     return Argument(value)._dsc;
// }

// // TODO: Rename
// inline void _freeArgument(mdsdsc_xd_t dsc) {
//     if (dsc.class_ == CLASS_XD) {
//         MdsFree1Dx(&dsc, nullptr);
//     }
// }

// TODO: WTF?
#define MdsEND_ARG ((void *)(intptr_t)1)
#define MDS_END_ARG , MdsEND_ARG

template <typename T /*= Data*/, typename ...Args>
T Data::Compile(std::string_view expression, Args... args)
{
    int status;
    mdsdsc_t dscExp = StringViewToDescriptor(expression);

    std::vector<Argument> argList = { Argument(args)... };
    std::vector<mdsdsc_t *> dscList = { &dscExp };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    status = TdiIntrinsic(OPC_COMPILE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    for (auto& arg : argList) {
        arg.free();
    }

    return Data(std::move(out)).releaseAndConvert<T>();
}

template <typename T /*= Data*/, typename ...Args>
T Data::Execute(std::string_view expression, Args... args)
{
    int status;
    mdsdsc_t dscExp = StringViewToDescriptor(expression);

    std::vector<Argument> argList = { Argument(args)... };
    std::vector<mdsdsc_t *> dscList = { &dscExp };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    status = TdiIntrinsic(OPC_EXECUTE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    for (auto& arg : argList) {
        arg.free();
    }

    return Data(std::move(out)).releaseAndConvert<T>();
}

} // namespace mdsplus

#endif // MDSPLUS_DATA_HPP