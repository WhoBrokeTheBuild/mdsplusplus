#ifndef MDSPLUS_DATA_INC_HPP
#define MDSPLUS_DATA_INC_HPP

#include "Data.hpp"
#include "DataView.hpp"
#include "Tree.hpp"

namespace mdsplus {

inline std::string to_string(const Class& class_)
{
    switch (class_) {
        case Class::Missing: return"Class::Missing";
        case Class::S: return"Class::S";
        case Class::D: return"Class::D";
        case Class::A: return"Class::A";
        case Class::XD: return"Class::XD";
        case Class::XS: return"Class::XS";
        case Class::R: return"Class::R";
        case Class::CA: return"Class::CA";
        case Class::APD: return"Class::APD";
    }

    return "?";
}

inline std::string to_string(const DType& dtype)
{
    switch (dtype) {
        case DType::Missing: return "DType::Missing";
        case DType::BU: return "DType::BU";
        case DType::WU: return "DType::WU";
        case DType::LU: return "DType::LU";
        case DType::QU: return "DType::QU";
        case DType::B: return "DType::B";
        case DType::W: return "DType::W";
        case DType::L: return "DType::L";
        case DType::Q: return "DType::Q";
        case DType::F: return "DType::F";
        case DType::D: return "DType::D";
        case DType::FC: return "DType::FC";
        case DType::DC: return "DType::DC";
        case DType::T: return "DType::T";
        case DType::DSC: return "DType::DSC";
        case DType::OU: return "DType::OU";
        case DType::O: return "DType::O";
        case DType::G: return "DType::G";
        case DType::H: return "DType::H";
        case DType::GC: return "DType::GC";
        case DType::HC: return "DType::HC";

        case DType::Pointer: return "DType::Pointer";
        case DType::FS: return "DType::FS";
        case DType::FT: return "DType::FT";
        case DType::FSC: return "DType::FSC";
        case DType::FTC: return "DType::FTC";
        case DType::C: return "DType::C";

        case DType::List: return "DType::List";
        case DType::Tuple: return "DType::Tuple";
        case DType::Dictionary: return "DType::Dictionary";

        case DType::Ident: return "DType::Ident";
        case DType::NID: return "DType::NID";
        case DType::Path: return "DType::Path";
        case DType::Param: return "DType::Param";
        case DType::Signal: return "DType::Signal";
        case DType::Dimension: return "DType::Dimension";
        case DType::Window: return "DType::Window";
        case DType::Slope: return "DType::Slope";
        case DType::Function: return "DType::Function";
        case DType::Conglom: return "DType::Conglom";
        case DType::Range: return "DType::Range";
        case DType::Action: return "DType::Action";
        case DType::Dispatch: return "DType::Dispatch";
        case DType::Program: return "DType::Program";
        case DType::Routine: return "DType::Routine";
        case DType::Procedure: return "DType::Procedure";
        case DType::Method: return "DType::Method";
        case DType::Dependency: return "DType::Dependency";
        case DType::Condition: return "DType::Condition";
        case DType::Event: return "DType::Event";
        case DType::WithUnits: return "DType::WithUnits";
        case DType::Call: return "DType::Call";
        case DType::WithError: return "DType::WithError";
        case DType::Opaque: return "DType::Opaque";
    }

    return "?";
}

inline std::string to_string(const Data * data)
{
    assert(data);

    std::string str(data->getClassName());
    str += "(";
    str += to_string(data->getClass());
    str += ", ";
    str += to_string(data->getDType());
    str += ", `";
    str += data->decompile();
    str += "`)";

    return str;
}

inline std::string Data::decompile() const
{
    int status;

    mdsdsc_t * dsc = getDescriptor();
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    status = _intrinsic(OPC_DECOMPILE, 1, &dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    mdsdsc_t * deco = out.pointer;
    assert(deco->dtype == DTYPE_T);

    std::string decompile(deco->pointer, deco->length);

    status = MdsFree1Dx(&out, nullptr);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return decompile;
}

inline Data Data::serialize() const
{
    mdsdsc_t * dsc = getDescriptor();
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = MdsSerializeDscOut(dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
    
    return Data(std::move(out), getTree());
}

template <typename UnitsType /*= Data*/>
inline UnitsType Data::getUnits() const
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    int status = _intrinsic(OPC_UNITS_OF, 1, &dsc, &out);
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
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    int status = _intrinsic(OPC_ERROR_OF, 1, &dsc, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ErrorType>();
}

template <typename HelpType /*= Data*/>
inline HelpType Data::getHelp() const
{
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    int status = _intrinsic(OPC_HELP_OF, 1, &dsc, &out);
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
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    mdsdsc_t * dsc = getDescriptor();
    int status = _intrinsic(OPC_ERROR_OF, 1, &dsc, &out);
    if (status == TdiINVDTYDSC) {
        // Unlike UNITS_OF and ERROR_OF, this throws an error when there is no value
        return ValidationType();
    }
    else if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ValidationType>();
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Data::Compile(const std::string& expression, const ArgTypes& ...args)
{
    DataView argExp(expression);
    std::vector<DataView> argList = { DataView(args)... };

    std::vector<mdsdsc_t *> dscList = { argExp.getDescriptor() };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = TdiIntrinsic(OPC_COMPILE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

template <typename ResultType /*= Data*/, typename ...ArgTypes>
ResultType Data::Execute(const std::string& expression, const ArgTypes& ...args)
{
    DataView argExp(expression);
    std::vector<DataView> argList = { DataView(args)... };

    std::vector<mdsdsc_t *> dscList = { argExp.getDescriptor() };
    for (const auto& arg : argList) {
        dscList.push_back(arg.getDescriptor());
    }

    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    int status = TdiIntrinsic(OPC_EXECUTE, dscList.size(), dscList.data(), &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
    
    return Data(std::move(out)).releaseAndConvert<ResultType>();
}

inline int Data::_intrinsic(opcode_t opcode, int narg, mdsdsc_t *list[], mdsdsc_xd_t * out) const
{
    int status;

    if (hasTree()) {
        status = _TdiIntrinsic(getTree()->getContext(), opcode, narg, list, out);
    }
    else {
        status = TdiIntrinsic(opcode, narg, list, out);
    }

    return status;
}

template <typename ResultType>
inline ResultType Data::_convertToScalar()
{
    int status;

    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if (dsc->class_ == class_t(ResultType::__class) &&
        dsc->dtype == dtype_t(ResultType::__dtype)) {
        return ResultType(std::move(xd), getTree());
    }

    // TODO: Check edge cases for when we need to call DATA()
    if (dsc->class_ == CLASS_R) {
        mdsdsc_xd_t dataXD = MDSDSC_XD_INITIALIZER;
        status = _intrinsic(OPC_DATA, 1, &dsc, &dataXD);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        MdsFree1Dx(&xd, nullptr);

        xd = dataXD;
        dsc = xd.pointer;

        if (dsc->class_ == class_t(ResultType::__class) &&
            dsc->dtype == dtype_t(ResultType::__dtype)) {
            return ResultType(std::move(xd), getTree());
        }
    }
    
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
    return ResultType(value);
}

template <typename ResultType>
inline ResultType Data::_convertToArray()
{
    int status;

    mdsdsc_xd_t xd = release();
    mdsdsc_t * dsc = xd.pointer;

    if (dsc->class_ == class_t(ResultType::__class) &&
        dsc->dtype == dtype_t(ResultType::__dtype)) {
        return ResultType(std::move(xd), getTree());
    }

    // TODO: Check edge cases for when we need to call DATA()
    if (dsc->class_ == CLASS_R) {
        mdsdsc_xd_t dataXD = MDSDSC_XD_INITIALIZER;
        status = _intrinsic(OPC_DATA, 1, &dsc, &dataXD);
        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        MdsFree1Dx(&xd, nullptr);

        xd = dataXD;
        dsc = xd.pointer;

        if (dsc->class_ == class_t(ResultType::__class) &&
            dsc->dtype == dtype_t(ResultType::__dtype)) {
            return ResultType(std::move(xd), getTree());
        }
    }
    
    array_coeff arrayOfOne = {
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

    array_coeff * dscArray = &arrayOfOne;
    if (dsc->class_ == CLASS_A) {
        dscArray = (array_coeff *)dsc;
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
    return ResultType(values, dims);
}

} // namespace mdsplus

#endif // MDSPLUS_DATA_INC_HPP