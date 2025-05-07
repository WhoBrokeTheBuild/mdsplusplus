#ifndef MDSPLUS_DATA_HPP
#define MDSPLUS_DATA_HPP

#include "Exceptions.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#if __has_include(<span>)
    #include <span>
#endif

extern "C" {

    /// Needed for mdsdsc*_t, <dtypedef.h>, <classdef.h>
    #include <mdsdescrip.h>

    /// Needed for MdsFree1Dx
    #include <mdsshr.h>

    /// Contains invalid definition for TdiConvert()
    /// #include <tdishr.h>
    int TdiConvert(mdsdsc_a_t * dsc, mdsdsc_a_t * convert);
    int TdiCall(dtype_t rtype, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);
    int _TdiIntrinsic(void **ctx, opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);
    int TdiIntrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t *out_ptr);

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
    static ResultType Compile(const std::string& expression, const ArgTypes& ...args);

    template <typename ResultType = Data, typename ...ArgTypes>
    static ResultType Execute(const std::string& expression, const ArgTypes& ...args);

    template <typename ValueType>
    static Data FromScalar(ValueType value) = delete;

    #ifdef __cpp_lib_span

        template <typename ValueType>
        static Data FromArray(
            std::span<const ValueType> values,
            const std::vector<uint32_t>& dims = {}
        );

    #endif

    template <typename ValueType>
    static Data FromArray(
        const std::vector<ValueType>& values,
        const std::vector<uint32_t>& dims = {}
    );

    template <typename ResultType = Data, typename ...ArgTypes>
    static inline ResultType FromExpression(const std::string& expression, ArgTypes... args) {
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

    virtual const char * getClassName() const {
        return "Data";
    }

    inline void setTree(Tree * tree) {
        _tree = tree;
    }

    [[nodiscard]]
    inline Tree * getTree() const {
        return _tree;
    }

    inline bool hasTree() const {
        return (_tree != nullptr);
    }

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

    std::string decompile() const;

    // TODO: Return Int8Array?
    Data serialize() const;

    template <typename DataType = Data>
    inline DataType getData() const {
        return convert<DataType>();
    }

    template <typename UnitsType = Data>
    UnitsType getUnits() const;

    // TODO: Default UnitsType to String
    template <typename DataType = Data, typename UnitsType = Data>
    std::tuple<DataType, UnitsType> getDataWithUnits() const;

    template <typename ErrorType = Data>
    ErrorType getError() const;

    template <typename HelpType = Data>
    HelpType getHelp() const;

    template <typename ValidationType = Data>
    ValidationType getValidation() const;

    // TODO: Explain decltype() conditionally enabling functions

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

    template <typename ResultType, typename ArrayType>
    inline auto add(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this + Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ArrayType>
    inline auto subtract(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this - Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ArrayType>
    inline auto multiply(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this * Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ArrayType>
    inline auto divide(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this / Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename ResultType, typename ArrayType>
    inline auto modulo(const ArrayType& value) -> decltype(Data::FromArray(value), ResultType()) {
        return (*this % Data::FromArray(value)).template releaseAndConvert<ResultType>();
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator+(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_ADD, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }
    
    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator-(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_SUBTRACT, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator/(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_DIVIDE, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator*(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_MULTIPLY, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline Data operator%(const DataType& other)
    {
        mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
        mdsdsc_t * args[] = { getDescriptor(), other.getDescriptor() };
        int status = _intrinsic(OPC_MOD, 2, args, &out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return Data(std::move(out), getTree());
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline bool operator==(const DataType& other) const
    {
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
        int status = _intrinsic(OPC_EQ, 2, args, (mdsdsc_xd_t *)&out);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return equal;
    }

    template <typename DataType,
        typename std::enable_if<std::is_base_of<Data, DataType>::value, bool>::type = true>
    inline bool operator!=(const DataType& other) const {
        return !(*this == other);
    }

    template <typename DataType>
    inline bool equals(const DataType& other) const {
        return (*this == other);
    }

    template <typename DataType>
    inline bool notEquals(const DataType& other) const {
        return (*this != other);
    }

protected:

    mdsdsc_xd_t _xd = MDSDSC_XD_INITIALIZER;

    Tree * _tree = nullptr;

    int _intrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t * out) const;

    template <typename ResultType>
    inline ResultType _clone() const {
        mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
        int status = MdsCopyDxXd(getDescriptor(), &xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        return ResultType(std::move(xd), getTree());
    }

    template <typename ResultType>
    ResultType _convertToScalar();

    template <typename ResultType>
    ResultType _convertToArray();

}; // class Data

static const Data EmptyData;

std::string to_string(const Data * data);

inline std::string to_string(const Data& data) {
    return to_string(&data);
}

template <>
inline Data Data::releaseAndConvert() {
    return Data(release(), getTree());
}

template <>
inline Data Data::FromScalar(std::nullptr_t) {
    return Data();
}

} // namespace mdsplus

// namespace std {

// template <> struct hash<mdsplus::Data>
// {
//     size_t operator()(const mdsplus::Data& data) const {
//         // TODO: Improve?
//         return (ptrdiff_t)data.getDescriptor();
//     }
// };

// // TODO: Define all types?

// } // namespace std

#endif // MDSPLUS_DATA_HPP