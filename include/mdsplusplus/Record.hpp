#ifndef MDSPLUS_RECORD_HPP
#define MDSPLUS_RECORD_HPP

#include "Data.hpp"

namespace mdsplus {

template <DType DT>
class Record : public Data
{
public:

    static constexpr Class class_ = Class::R;
    static constexpr DType dtype = DT;

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

    template <typename T = Data>
    inline T getDescriptorAt(size_t index) const {
        mdsdsc_r_t * dsc = getRecordDescriptor();

        if (dsc && index < dsc->ndesc) {
            mdsdsc_xd_t xd = MDSDSC_XD_INITIALIZER;
            int status = MdsCopyDxXd(dsc->dscptrs[index], &xd);
            if (IS_NOT_OK(status)) {
                throwException(status);
            }

            mdsdsc_t * newDsc = xd.pointer;
            if (Class(newDsc->class_) == T::class_ && DType(newDsc->dtype) == T::dtype) {
                return T(std::move(xd), getTree());
            }

            return Data(std::move(xd), getTree()).releaseAndConvert<T>();
        }

        return T();
    }

}; // class Record<>

class Param : public Record<DType::Param>
{
public:

    Param() = default;

    inline Param(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Value = Data, typename _Help = Data, typename _Validation = Data>
    Param(const _Value& value, const _Help& help, const _Validation& validation)
    {
        int status;

        Argument argValue(value);
        Argument argUnits(help);
        Argument argValidation(validation);

        DESCRIPTOR_PARAM(dsc,
            argValue.getDescriptor(),
            argUnits.getDescriptor(),
            argValidation.getDescriptor()
        );

        status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        argValue.free();
        argUnits.free();
        argValidation.free();
    }

    [[nodiscard]]
    inline Param clone() const {
        return _clone<Param>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValue() const {
        return getDescriptorAt<T>(0);
    }

    // getHelp and getValidation are defined in Data

}; // class Param

class Signal : public Record<DType::Signal>
{
public:

    Signal() = default;

    inline Signal(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Value = Data, typename _Raw = Data, typename _Dimension = Data>
    Signal(const _Value& value, const _Raw& raw, const _Dimension& dimension = {})
    {
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

    template <typename _Value = Data, typename _Raw = Data, typename ..._Dimensions>
    Signal(const _Value& value, const _Raw& raw, const _Dimensions& ...dimensions)
    {
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

    [[nodiscard]]
    inline Signal clone() const {
        return _clone<Signal>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValue() const {
        return getDescriptorAt<T>(0);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getRaw() const {
        return getDescriptorAt<T>(1);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getDimensionAt(size_t index = 0) const {
        return getDescriptorAt<T>(2 + index);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getDimensions() const {
        std::vector<T> dims;
        size_t count = getNumDescriptors() - 2;
        for (size_t i = 0; i < count; ++i) {
            dims.push_back(getDimensionAt(i));
        }
        return dims;
    }

}; // class Signal

class Dimension : public Record<DType::Dimension>
{
public:

    Dimension() = default;

    Dimension(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Window, typename _Axis>
    Dimension(const _Window& window, const _Axis& axis)
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

    [[nodiscard]]
    inline Dimension clone() const {
        return _clone<Dimension>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getWindow() const {
        return getDescriptorAt<T>(0);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getAxis() const {
        return getDescriptorAt<T>(1);
    }

}; // class Dimension

class Window : public Record<DType::Window>
{
public:

    Window() = default;

    Window(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _StartIndex, typename _EndIndex, typename _ValueAtIndex0>
    Window(const _StartIndex& startIndex, const _EndIndex& endIndex, const _ValueAtIndex0& valueAtIndex0)
    {
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

    [[nodiscard]]
    inline Window clone() const {
        return _clone<Window>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getStartIndex() const {
        return getDescriptorAt<T>(0);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getEndIndex() const {
        return getDescriptorAt<T>(1);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValueAtIndex0() const {
        return getDescriptorAt<T>(2);
    }

}; // class Window

class Function : public Record<DType::Function>
{
public:

    Function() = default;

    Function(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename ..._Args>
    Function(opcode_t opcode, const _Args& ...args)
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

class Range : public Record<DType::Range>
{
public:

    Range() = default;

    Range(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Begin = Data, typename _Ending = Data, typename _Delta = Data>
    Range(const _Begin& begin, const _Ending& ending, const _Delta& delta)
    {
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

    [[nodiscard]]
    inline Range clone() const {
        return _clone<Range>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getBegin() const {
        return getDescriptorAt<T>(0);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getEnding() const {
        return getDescriptorAt<T>(1);
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getDelta() const {
        return getDescriptorAt<T>(2);
    }

    // TODO: isFinite() ?
    // TODO: begin/end/size

}; // class Range

class WithUnits : public Record<DType::WithUnits>
{
public:

    WithUnits() = default;

    inline WithUnits(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Value = Data, typename _Units = Data>
    WithUnits(const _Value& value, const _Units& units)
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

    [[nodiscard]]
    inline WithUnits clone() const {
        return _clone<WithUnits>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValue() const {
        return getDescriptorAt<T>(0);
    }

    // getUnits is defined in Data

}; // class WithUnits

class WithError : public Record<DType::WithError>
{
public:

    WithError() = default;

    inline WithError(mdsdsc_xd_t && dsc, Tree * tree = nullptr)
        : Record(std::move(dsc), tree)
    { }

    template <typename _Value = Data, typename _Error = Data>
    WithError(const _Value& value, const _Error& error)
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

    [[nodiscard]]
    inline WithError clone() const {
        return _clone<WithError>();
    }

    template <typename T = Data>
    [[nodiscard]]
    inline T getValue() const {
        return getDescriptorAt<T>(0);
    }

    // getError is defined in Data

}; // class WithError

} // namespace mdsplus

#endif // MDSPLUS_RECORD_HPP