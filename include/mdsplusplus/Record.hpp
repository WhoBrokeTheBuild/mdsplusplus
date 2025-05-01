#ifndef MDSPLUS_RECORD_HPP
#define MDSPLUS_RECORD_HPP

#include "Data.hpp"

namespace mdsplus {

class Record : public Data
{
public:

    static constexpr Class __class = Class::R;

    Record() = default;

    inline Record(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Data(std::move(dsc), tree)
    { }

    Record(Record&&) = default;
    Record& operator=(Record&&) = default;

    virtual ~Record() = default;

    [[nodiscard]]
    inline mdsdsc_r_t * getRecordDescriptor() const {
        return reinterpret_cast<mdsdsc_r_t *>(_xd.pointer);
    }

    inline size_t getNumDescriptors() const {
        mdsdsc_r_t * dsc = getRecordDescriptor();
        return (dsc ? dsc->ndesc : 0);
    }

    inline bool hasDescriptorAt(size_t index) const {
        mdsdsc_r_t * dsc = getRecordDescriptor();
        return (dsc && index < dsc->ndesc && dsc->dscptrs[index] != nullptr);
    }

    template <typename ResultType = Data>
    inline ResultType getDescriptorAt(size_t index) const {
        mdsdsc_r_t * dsc = getRecordDescriptor();

        if (dsc && index < dsc->ndesc) {
            mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
            int status = MdsCopyDxXd(dsc->dscptrs[index], &xd);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            mdsdsc_t * newDsc = xd.pointer;
            if (Class(newDsc->class_) == ResultType::__class && DType(newDsc->dtype) == ResultType::__dtype) {
                return ResultType(std::move(xd), getTree());
            }

            return Data(std::move(xd), getTree()).releaseAndConvert<ResultType>();
        }

        return ResultType();
    }

}; // class Record<>

#define MDSPLUS_RECORD_BOOTSTRAP(RecordType, DataType)           \
                                                                 \
    static constexpr DType __dtype = DataType;                   \
                                                                 \
    RecordType() = default;                                      \
                                                                 \
    inline RecordType(mdsdsc_xd_t && dsc, Tree * tree = nullptr) \
        : Record(std::move(dsc), tree)                           \
    { }                                                          \
                                                                 \
    RecordType(RecordType &&) = default;                         \
    RecordType &operator=(RecordType &&) = default;              \
                                                                 \
    inline std::string_view getClassName() const override {      \
        return #RecordType;                                      \
    }                                                            \
                                                                 \
    [[nodiscard]]                                                \
    inline RecordType clone() const {                            \
        return _clone<RecordType>();                             \
    }

class Param : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Param, DType::Param)

    template <
        typename ValueType, 
        typename HelpType, 
        typename ValidationType
    >
    Param(
        const ValueType& value,
        const HelpType& help,
        const ValidationType& validation
    ) {
        int status;

        Argument argValue(value);
        Argument argHelp(help);
        Argument argValidation(validation);

        DESCRIPTOR_PARAM(dsc,
            argValue.getDescriptor(),
            argHelp.getDescriptor(),
            argValidation.getDescriptor()
        );

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        argValue.free();
        argHelp.free();
        argValidation.free();
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValue() const {
        return getDescriptorAt<ValueType>(0);
    }

    template <typename HelpType = Data>
    [[nodiscard]]
    inline HelpType getHelp() const {
        return getDescriptorAt<HelpType>(1);
    }

    template <typename ValidationType = Data>
    [[nodiscard]]
    inline ValidationType getValidation() const {
        return getDescriptorAt<ValidationType>(2);
    }


}; // class Param

class Signal : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Signal, DType::Signal)

    template <
        typename ValueType, 
        typename RawType, 
        typename DimensionType
    >
    Signal(
        const ValueType& value,
        const RawType& raw,
        const DimensionType& dimension = {}
    ) {
        int status;

        Argument argValue(value);
        Argument argRaw(raw);
        Argument argDimension(dimension);

        DESCRIPTOR_SIGNAL_1(dsc,
            argValue.getDescriptor(),
            argRaw.getDescriptor(),
            argDimension.getDescriptor()
        );

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }
        
        argValue.free();
        argRaw.free();
        argDimension.free();
    }

    template <
        typename ValueType,
        typename RawType,
        typename ...DimensionTypes
    >
    Signal(
        const ValueType& value,
        const RawType& raw,
        const DimensionTypes& ...dimensions
    ) {
        int status;

        Argument argValue(value);
        Argument argRaw(raw);
        std::vector<Argument> argDimensions = { Argument(dimensions)... };

        DESCRIPTOR_SIGNAL(dsc,
            sizeof...(dimensions),
            argValue.getDescriptor(),
            argRaw.getDescriptor()
        );

        for (size_t i = 0; i < argDimensions.size(); ++i) {
            dsc.dimensions[i] = argDimensions[i].getDescriptor();
        }

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        argValue.free();
        argRaw.free();

        for (size_t i = 0; i < argDimensions.size(); ++i) {
            argDimensions[i].free();
        }
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValue() const {
        return getDescriptorAt<ValueType>(0);
    }

    template <typename RawType = Data>
    [[nodiscard]]
    inline RawType getRaw() const {
        return getDescriptorAt<RawType>(1);
    }

    template <typename DimensionType = Data>
    [[nodiscard]]
    inline DimensionType getDimensionAt(size_t index = 0) const {
        return getDescriptorAt<DimensionType>(2 + index);
    }

    template <typename DimensionType = Data>
    [[nodiscard]]
    inline DimensionType getDimensions() const {
        std::vector<DimensionType> dims;
        size_t count = getNumDescriptors() - 2;
        for (size_t i = 0; i < count; ++i) {
            dims.push_back(getDimensionAt(i));
        }
        return dims;
    }

}; // class Signal

class Dimension : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Dimension, DType::Dimension)

    template <typename WindowType, typename AxisType>
    Dimension(const WindowType& window, const AxisType& axis)
    {
        int status;

        Argument tmpWindow(window);
        Argument tmpAxis(axis);

        DESCRIPTOR_DIMENSION(dsc,
            tmpWindow.getDescriptor(),
            tmpAxis.getDescriptor()
        );

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        tmpWindow.free();
        tmpAxis.free();
    }

    template <typename WindowType = Data>
    [[nodiscard]]
    inline WindowType getWindow() const {
        return getDescriptorAt<WindowType>(0);
    }

    template <typename AxisType = Data>
    [[nodiscard]]
    inline AxisType getAxis() const {
        return getDescriptorAt<AxisType>(1);
    }

}; // class Dimension

class Window : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Window, DType::Window)
    
    template <
        typename StartIndexType,
        typename EndIndexType,
        typename ValueType
    >
    Window(
        const StartIndexType& startIndex,
        const EndIndexType& endIndex,
        const ValueType& valueAtIndex0
    ) {
        int status;

        Argument argStartIndex(startIndex);
        Argument argEndIndex(endIndex);
        Argument argValueAtIndex0(valueAtIndex0);

        DESCRIPTOR_WINDOW(dsc,
            argStartIndex.getDescriptor(),
            argEndIndex.getDescriptor(),
            argValueAtIndex0.getDescriptor()
        );

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        argStartIndex.free();
        argEndIndex.free();
        argValueAtIndex0.free();
    }

    template <typename StartIndexType = Data>
    [[nodiscard]]
    inline StartIndexType getStartIndex() const {
        return getDescriptorAt<StartIndexType>(0);
    }

    template <typename EndIndexType = Data>
    [[nodiscard]]
    inline EndIndexType getEndIndex() const {
        return getDescriptorAt<EndIndexType>(1);
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValueAtIndex0() const {
        return getDescriptorAt<ValueType>(2);
    }

}; // class Window

class Function : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Function, DType::Function)

    template <typename ...ArgTypes>
    Function(opcode_t opcode, const ArgTypes& ...args)
    {
        int status;

        // TODO: #define MAX_ARGS 255 ?
        static_assert(sizeof...(args) <= 255);

        std::vector<Argument> argList = { Argument(args)... };

        DESCRIPTOR_FUNCTION(dsc, &opcode, sizeof...(args));

        for (size_t i = 0; i < argList.size(); ++i) {
            dsc.arguments[i] = argList[i].getDescriptor();
        }

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        for (auto& arg : argList) {
            arg.free();
        }
    }

    Data call() const;

}; // class Function

// TODO: Conglom?

class Range : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(Range, DType::Range)

    template <
        typename BeginType = Data,
        typename EndingType = Data,
        typename DeltaType = Data
    >
    Range(
        const BeginType& begin,
        const EndingType& ending,
        const DeltaType& delta
    ) {
        int status;

        Argument argBegin(begin);
        Argument argEnding(ending);
        Argument argDelta(delta);

        DESCRIPTOR_RANGE(dsc,
            argBegin.getDescriptor(),
            argEnding.getDescriptor(),
            argDelta.getDescriptor()
        );

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        argBegin.free();
        argEnding.free();
        argDelta.free();
    }

    template <typename BeginType = Data>
    [[nodiscard]]
    inline BeginType getBegin() const {
        return getDescriptorAt<BeginType>(0);
    }

    template <typename EndingType = Data>
    [[nodiscard]]
    inline EndingType getEnding() const {
        return getDescriptorAt<EndingType>(1);
    }

    template <typename DeltaType = Data>
    [[nodiscard]]
    inline DeltaType getDelta() const {
        return getDescriptorAt<DeltaType>(2);
    }

    // TODO: isFinite() ?
    // TODO: begin/end/size

}; // class Range

// TODO: Action
// TODO: Dispatch
// TODO: Routine?
// TODO: Method?
// TODO: Event?

class WithUnits : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(WithUnits, DType::WithUnits)

    template <typename ValueType, typename UnitsType>
    WithUnits(const ValueType& value, const UnitsType& units)
    {
        int status;

        Argument argValue(value);
        Argument argUnits(units);

        DESCRIPTOR_WITH_UNITS(dsc,
            argValue.getDescriptor(),
            argUnits.getDescriptor()
        );

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        argValue.free();
        argUnits.free();
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValue() const {
        return getDescriptorAt<ValueType>(0);
    }

    template <typename UnitsType = Data>
    [[nodiscard]]
    inline UnitsType getUnits() const {
        return getDescriptorAt<UnitsType>(1);
    }

}; // class WithUnits

// TODO: Call?

class WithError : public Record
{
public:

    MDSPLUS_RECORD_BOOTSTRAP(WithError, DType::WithError)

    template <typename ValueType, typename ErrorType>
    WithError(const ValueType& value, const ErrorType& error)
    {
        int status;

        Argument argValue(value);
        Argument argError(error);

        DESCRIPTOR_WITH_ERROR(dsc,
            argValue.getDescriptor(),
            argError.getDescriptor()
        );

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        argValue.free();
        argError.free();
    }

    template <typename ValueType = Data>
    [[nodiscard]]
    inline ValueType getValue() const {
        return getDescriptorAt<ValueType>(0);
    }

    template <typename ErrorType = Data>
    [[nodiscard]]
    inline ErrorType getError() const {
        return getDescriptorAt<ErrorType>(1);
    }

}; // class WithError

// TODO: List
// TODO: Tuple
// TODO: Dictionary
// TODO: Opaque?

} // namespace mdsplus

#endif // MDSPLUS_RECORD_HPP