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

///
/// MDSplus Storage Class (class_t)
///
/// Used to indicate how the data is stored in a descriptor.
///
enum class Class : uint8_t
{
    Missing = CLASS_MISSING,        ///< Unknown

    S = CLASS_S,                    ///< %Static
    D = CLASS_D,                    ///< Dynamic (%String)
    A = CLASS_A,                    ///< %Array
    XD = CLASS_XD,                  ///< Extended Dynamic
    XS = CLASS_XS,                  ///< Extended Static
    R = CLASS_R,                    ///< %Record / Compound
    CA = CLASS_CA,                  ///< Compressed %Array
    APD = CLASS_APD,                ///< %Array of Pointers to Descriptors

}; // enum class Class

std::string to_string(const Class& class_);

inline constexpr bool operator==(const Class lhs, const class_t rhs) {
    return lhs == static_cast<Class>(rhs);
}

///
/// MDSplus %Data Type (dtype_t)
///
/// Used to indicate the type of the data stored in a descriptor.
///
enum class DType : uint8_t
{
    Missing = DTYPE_MISSING,        ///< Unknown

    BU = DTYPE_BU,                  ///< Byte Unsigned (uint8_t)
    WU = DTYPE_WU,                  ///< Word Unsigned (uint16_t)
    LU = DTYPE_LU,                  ///< Long Unsigned (uint32_t)
    QU = DTYPE_QU,                  ///< Quadword Unsigned (uint64_t)
    B = DTYPE_B,                    ///< Byte (int8_t)
    W = DTYPE_W,                    ///< Word (int16_t)
    L = DTYPE_L,                    ///< Long (int32_t)
    Q = DTYPE_Q,                    ///< Quadword (int64_t)
    F = DTYPE_F,                    ///< VMS Float
    D = DTYPE_D,                    ///< VMS Double
    FC = DTYPE_FC,                  ///< VMS Complex Float
    DC = DTYPE_DC,                  ///< VMS Complex Double
    T = DTYPE_T,                    ///< Text / String (char *)
    DSC = DTYPE_DSC,                ///< Descriptor (mdsdsc_t *)
    OU = DTYPE_OU,                  ///< Octaword Unsigned (uint128_t)
    O = DTYPE_O,                    ///< Octaword (int128_t)
    G = DTYPE_G,                    ///< VMS 64-bit Floating Point
    H = DTYPE_H,                    ///< VMS 128-bit Floating Point
    GC = DTYPE_GC,                  ///< VMS 64-bit Complex Floating Point
    HC = DTYPE_HC,                  ///< VMS 128-bit Complex Floating Point
    FS = DTYPE_FS,                  ///< IEEE-754 Float
    FT = DTYPE_FT,                  ///< IEEE-754 Double
    FSC = DTYPE_FSC,                ///< IEEE-754 Complex Float
    FTC = DTYPE_FTC,                ///< IEEE-754 Complex Double
    C = DTYPE_C,                    ///<

    Float = DTYPE_FLOAT,            ///< Alias for Native Float
    Double = DTYPE_DOUBLE,          ///< Alias for Native Double

    Pointer = DTYPE_POINTER,        ///<
    Ident = DTYPE_IDENT,            ///< TDI Identifier
    NID = DTYPE_NID,                ///< TreeNode ID (NID)
    Path = DTYPE_PATH,              ///< TreeNode %Path
    Param = DTYPE_PARAM,            ///< %Param(Value, Help, Validation)
    Signal = DTYPE_SIGNAL,          ///< %Signal(Value, Raw, [Dimensions]...)
    Dimension = DTYPE_DIMENSION,    ///< %Dimension(Window, Axis)
    Window = DTYPE_WINDOW,          ///< %Window(StartIndex, EndIndex, ValueAtIndex0)
    Slope = DTYPE_SLOPE,            ///< Deprecated
    Function = DTYPE_FUNCTION,      ///< %Function(opcode_t, Args...)
    Conglom = DTYPE_CONGLOM,        ///< %Conglom(Image, Model, Name, [Qualifiers])
    Range = DTYPE_RANGE,            ///< %Range(Begin, Ending, Delta)
    Action = DTYPE_ACTION,          ///< %Action(Dispatch, Task, [ErrorLogs])
    Dispatch = DTYPE_DISPATCH,      ///< %Dispatch(treesched_t, Ident, Phase, When, [Completion])
    Program = DTYPE_PROGRAM,        ///< Deprecated
    Routine = DTYPE_ROUTINE,        ///< %Routine(Image, Routine, Timeout, NumArgs)
    Procedure = DTYPE_PROCEDURE,    ///< Deprecated
    Method = DTYPE_METHOD,          ///< %Method(Method, Object, Timeout NumArgs)
    Dependency = DTYPE_DEPENDENCY,  ///< Deprecated
    Condition = DTYPE_CONDITION,    ///< Deprecated
    Event = DTYPE_EVENT,            ///<
    WithUnits = DTYPE_WITH_UNITS,   ///< %WithUnits(Value, Units)
    Call = DTYPE_CALL,              ///< %Call(dtype *, NumArgs, Image, Routine)
    WithError = DTYPE_WITH_ERROR,   ///< %WithError(Value, Error)
    List = DTYPE_LIST,              ///< %List([Values]...)
    Tuple = DTYPE_TUPLE,            ///< %Tuple([Values]...)
    Dictionary = DTYPE_DICTIONARY,  ///< %Dictionary([Key, Value,]...)
    Opaque = DTYPE_OPAQUE,          ///< %Opaque(%Data, OpaqueType)

}; // enum class DType

std::string to_string(const DType& dtype);

inline constexpr bool operator==(const DType lhs, const dtype_t rhs) {
    return lhs == static_cast<DType>(rhs);
}

class Tree;

///
/// MDSplus Data base class
///
class Data
{
public:

    typedef std::nullptr_t __ctype;
    static constexpr Class __class = Class::Missing;
    static constexpr DType __dtype = DType::Missing;

    template <typename ResultType = Data, typename ...ArgTypes>
    static ResultType Compile(std::string_view expression, ArgTypes... args);

    template <typename ResultType = Data, typename ...ArgTypes>
    static ResultType Execute(std::string_view expression, ArgTypes... args);

    template <typename ValueType>
    static Data FromScalar(ValueType value) = delete;

#ifdef __cpp_lib_span
    template <typename ValueType>
    static Data FromArray(const std::span<ValueType> values, const std::vector<uint32_t>& dims = {});
#endif

    template <typename ValueType>
    static Data FromArray(const std::vector<ValueType>& values, const std::vector<uint32_t>& dims = {});

    template <typename ResultType = Data, typename ...ArgTypes>
    static inline ResultType FromExpression(std::string_view expression, ArgTypes... args) {
        return Execute<ResultType>(expression, args...);
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

    virtual std::string_view getClassName() const {
        return "Data";
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

    template <typename ResultType>
    [[nodiscard]]
    inline ResultType convert() const {
        return clone().releaseAndConvert<ResultType>();
    }

    template <typename ResultType>
    [[nodiscard]]
    ResultType releaseAndConvert();

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

    template <typename DataType = Data>
    inline DataType getData() const {
        return clone().convert<DataType>();
    }

    template <typename UnitsType = Data>
    UnitsType getUnits() const;

    template <typename DataType = Data, typename UnitsType = Data>
    std::tuple<DataType, UnitsType> getDataWithUnits() const;

    template <typename ErrorType = Data>
    ErrorType getError() const;

    template <typename HelpType = Data>
    HelpType getHelp() const;

    template <typename ValidationType = Data>
    ValidationType getValidation() const;

    std::string decompile() const;

    auto serialize() const;

    template <typename ScalarType>
    inline auto operator+(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this + Data::FromScalar(value);
    }

    template <typename ScalarType>
    inline auto operator-(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this - Data::FromScalar(value);
    }

    template <typename ScalarType>
    inline auto operator*(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this * Data::FromScalar(value);
    }

    template <typename ScalarType>
    inline auto operator/(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this / Data::FromScalar(value);
    }

    template <typename ScalarType>
    inline auto operator%(const ScalarType& value) -> decltype(Data::FromScalar(value)) {
        return *this % Data::FromScalar(value);
    }

    template <typename ArrayType>
    inline auto operator+(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this + Data::FromArray(values);
    }

    template <typename ArrayType>
    inline auto operator-(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this - Data::FromArray(values);
    }

    template <typename ArrayType>
    inline auto operator*(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this * Data::FromArray(values);
    }

    template <typename ArrayType>
    inline auto operator/(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this / Data::FromArray(values);
    }

    template <typename ArrayType>
    inline auto operator%(const ArrayType& values) -> decltype(Data::FromArray(values)) {
        return *this % Data::FromArray(values);
    }

    template <typename ResultType, typename ScalarType>
    inline auto add(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this + Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ScalarType>
    inline auto subtract(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this - Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ScalarType>
    inline auto multiply(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this * Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ScalarType>
    inline auto divide(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this / Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ScalarType>
    inline auto modulo(const ScalarType& value) -> decltype(Data::FromScalar(value), ResultType()) {
        return (*this % Data::FromScalar(value)).template releaseAndConvert<ResultType>();
    }

    template <typename DataType,
        std::enable_if_t<std::is_base_of<Data, DataType>::value, bool> = true>
    inline Data operator+(const DataType& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_ADD, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }
    
    template <typename DataType,
        std::enable_if_t<std::is_base_of<Data, DataType>::value, bool> = true>
    inline Data operator-(const DataType& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_SUBTRACT, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        std::enable_if_t<std::is_base_of<Data, DataType>::value, bool> = true>
    inline Data operator/(const DataType& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_DIVIDE, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        std::enable_if_t<std::is_base_of<Data, DataType>::value, bool> = true>
    inline Data operator*(const DataType& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_MULTIPLY, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        std::enable_if_t<std::is_base_of<Data, DataType>::value, bool> = true>
    inline Data operator%(const DataType& other) {
        int status;

        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        status = _intrinsic(OPC_MOD, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        std::enable_if_t<std::is_base_of<Data, DataType>::value, bool> = true>
    inline bool operator==(const DataType& other) const {
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

    template <typename DataType,
        std::enable_if_t<std::is_base_of<Data, DataType>::value, bool> = true>
    inline bool operator!=(const DataType& other) const {
        return !(*this == other);
    }

protected:

    mdsdsc_xd_t _xd = MDSDSC_XD_INITIALIZER;

    Tree * _tree = nullptr;

    int _intrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t * out) const;

    template <typename ResultType>
    inline ResultType _clone() const {
        int status;

        mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
        status = MdsCopyDxXd(getDescriptor(), &xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return ResultType(std::move(xd), getTree());
    }

}; // class Data

static const Data EmptyData;

std::string to_string(const Data * data);

inline std::string to_string(const Data& data) {
    return to_string(&data);
}

template <>
inline Data Data::FromScalar(std::nullptr_t) {
    return Data();
}

class String : public Data
{
public:

    static constexpr Class __class = Class::S;
    static constexpr DType __dtype = DType::T;

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

    static constexpr Class __class = Class::A;
    static constexpr DType __dtype = DType::T;

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
    // inline __ctype * begin() const {
    //     mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    //     return (dsc ? (__ctype *)dsc->pointer : nullptr);
    // }

    // [[nodiscard]]
    // inline __ctype * end() const {
    //     mdsdsc_a_t * dsc = (mdsdsc_a_t *)getDescriptor();
    //     return (dsc ? (__ctype *)(dsc->pointer + dsc->arsize) : nullptr);
    // }

}; // class StringArray

template <typename ResultType>
ResultType Data::releaseAndConvert()
{
    int status;

    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if constexpr (std::is_same_v<Data, ResultType>) {
        return ResultType(std::move(xd), getTree());
    }
    else if constexpr (std::is_base_of_v<Data, ResultType>) {

        if (Class(dsc->class_) == ResultType::__class && DType(dsc->dtype) == ResultType::__dtype) {
            return ResultType(std::move(xd), getTree());
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

            if (Class(dsc->class_) == ResultType::__class && DType(dsc->dtype) == ResultType::__dtype) {
                return ResultType(std::move(xd), getTree());
            }
        }

        // TODO: Is there a better way to convert to a string?
        if constexpr (std::is_same_v<String, ResultType>) {
            mdsdsc_xd_t decoXD = MDSDSC_XD_INITIALIZER;
            status = _intrinsic(OPC_DECOMPILE, 1, &dsc, &decoXD);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            MdsFree1Dx(&xd, nullptr);
            return String(std::move(decoXD), getTree());
        }
        else if constexpr (ResultType::__class == Class::S) {
            typename ResultType::__ctype value = {};

            mdsdsc_t convert = {
                .length = sizeof(typename ResultType::__ctype),
                .dtype = dtype_t(ResultType::__dtype),
                .class_ = CLASS_S,
                .pointer = (char *)&value,
            };
    
            status = TdiConvert((mdsdsc_a_t *)dsc, (mdsdsc_a_t *)&convert);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }
    
            MdsFree1Dx(&xd, nullptr);
            return ResultType(value, getTree());
        }
        else if constexpr (ResultType::__class == Class::A && ResultType::__dtype != DType::T) {
            mdsdsc_a_t arrayOfOne = {
                .length = dsc->length,
                .dtype = dsc->dtype,
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
                .dimct = 0,
                .arsize = dsc->length,
            };

            array_coeff * dscArray;
            if (dsc->class_ == CLASS_A) {
                dscArray = (array_coeff *)dsc;
            }
            else {
                dscArray = (array_coeff *)&arrayOfOne;
            }

            size_t size = (dscArray->arsize / dscArray->length);
            std::vector<typename ResultType::__ctype> values(size);
            array_coeff convert = {
                .length = sizeof(typename ResultType::__ctype),
                .dtype = dtype_t(ResultType::__dtype),
                .class_ = CLASS_A,
                .pointer = (char *)values.data(),
                .scale = dscArray->scale,
                .digits = dscArray->digits,
                .aflags = dscArray->aflags,
                .dimct = dscArray->dimct,
                .arsize = arsize_t(size * sizeof(typename ResultType::__ctype)),
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
            else {
                // TODO: Improve
                dims.clear();
            }
        
            status = TdiConvert((mdsdsc_a_t *)dscArray, (mdsdsc_a_t *)&convert);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }
    
            MdsFree1Dx(&xd, nullptr);
            return ResultType(values, dims, getTree());
        }
    }

    MdsFree1Dx(&xd, nullptr);
    throw TdiInvalidDataType();
}

template <typename UnitsType /*= Data*/>
inline UnitsType Data::getUnits() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    status = _intrinsic(OPC_UNITS_OF, 1, &dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<UnitsType>();
}

template <typename DataType /*= Data*/, typename UnitsType /*= Data*/>
std::tuple<DataType, UnitsType> Data::getDataWithUnits() const
{
    // We could use OPC_DATA_WITH_UNITS to upcast it to a WithUnits() type,
    // but returning a tuple is a cleaner API
    return { getData<DataType>(), getUnits<UnitsType>() };
}

template <typename ErrorType /*= Data*/>
inline ErrorType Data::getError() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    status = _intrinsic(OPC_ERROR_OF, 1, &dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ErrorType>();
}

template <typename HelpType /*= Data*/>
inline HelpType Data::getHelp() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    status = _intrinsic(OPC_HELP_OF, 1, &dsc, &out);
    if (status == TdiINVDTYDSC) {
        // Unlike UNITS_OF and ERROR_OF, this throws an error when there is no value
        return HelpType();
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<HelpType>();
}

template <typename ValidationType /*= Data*/>
inline ValidationType Data::getValidation() const
{
    int status;

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    status = _intrinsic(OPC_ERROR_OF, 1, &dsc, &out);
    if (status == TdiINVDTYDSC) {
        // Unlike UNITS_OF and ERROR_OF, this throws an error when there is no value
        return ValidationType();
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ValidationType>();
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
    template <typename DataType,
        std::enable_if_t<std::is_base_of_v<Data, DataType>, bool> = true>
    inline Argument(const DataType &value)
        : _dsc({
            .length = 0,
            .dtype = DTYPE_DSC,
            .class_ = CLASS_S,
            .pointer = value.getDescriptor(),
            .l_length = 0,
        })
    { }

    #ifdef __cpp_lib_span

        template <typename ValueType>
        inline Argument(const std::span<ValueType> value)
            : _dsc(Data::FromArray(value).release())
        { }
        
    #endif

    template <typename ValueType>
    inline Argument(const std::vector<ValueType> &value)
        : _dsc(Data::FromArray(value).release())
    { }

    template <typename ValueType, std::enable_if_t<std::is_integral_v<ValueType>, bool> = true>
    inline Argument(const ValueType &value)
        : _dsc(Data::FromScalar(value).release())
    { }

    template <typename ValueType, std::enable_if_t<std::is_floating_point_v<ValueType>, bool> = true>
    inline Argument(const ValueType &value)
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

template <typename ResultType /*= Data*/, typename ...Args>
ResultType Data::Compile(std::string_view expression, Args... args)
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

    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

template <typename ResultType /*= Data*/, typename ...Args>
ResultType Data::Execute(std::string_view expression, Args... args)
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

    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

} // namespace mdsplus

#endif // MDSPLUS_DATA_HPP