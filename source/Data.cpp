#include <mdsplus.hpp>

namespace mdsplus {

std::string Data::decompile()
{
    int status;

    mdsdsc_t * dsc = getDescriptor();
    mdsdsc_xd_t decoXD = MDSDSC_XD_INITIALIZER;
    if (hasTree()) {
        status = _TdiIntrinsic(getTree()->getPDBID(), OPC_DECOMPILE, 1, &dsc, &decoXD);
    }
    else {
        status = TdiIntrinsic(OPC_DECOMPILE, 1, &dsc, &decoXD);
    }
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    mdsdsc_t * decoDsc = decoXD.pointer;
    assert(decoDsc->dtype == DTYPE_T);

    std::string decompile(decoDsc->pointer, decoDsc->length);
    status = MdsFree1Dx(&decoXD, nullptr);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return decompile;
}

void String::setValue(std::string_view value)
{
    int status;

    mdsdsc_s_t dsc = {
        (length_t)value.size(),
        DTYPE_T,
        CLASS_S,
        (char *)value.data()
    };

    status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

void StringArray::setValues(std::span<std::string> values, const std::vector<uint32_t>& dims /*= {}*/)
{
    int status;

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

    array_coeff dsc = {
        .length = maxSize,
        .dtype = DTYPE_T,
        .class_ = CLASS_A,
        .pointer = buffer.data(),
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
        .a0 = buffer.data(),
        .m = { 0 },
    };

    if (dims.empty()) {
        dsc.dimct = 1;
        dsc.m[0] = values.size();
    }
    else {
        for (size_t i = 0; i < dims.size(); ++i) {
            dsc.m[i] = dims[i];
        }
    }

    status = MdsCopyDxXd((mdsdsc_t *)&dsc, &_xd);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
}

Data Function::call() const
{
    int status;

    descriptor_function * dsc = (descriptor_function *)getDescriptor();

    // TODO: Tree*
    opcode_t * opcode = (opcode_t *)dsc->pointer;
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    if (hasTree()) {
        status = _TdiIntrinsic(getTree()->getPDBID(), *opcode, dsc->ndesc, dsc->arguments, &out);
    }
    else {
        status = TdiIntrinsic(*opcode, dsc->ndesc, dsc->arguments, &out);
    }
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out));
}

// UInt8Array Data::serialize() const
// {
//     mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
//     int status = MdsSerializeDscOut(getDescriptor(), &out);
//     if (IS_NOT_OK(status)) {
//         throwException(status);
//     }

//     return UInt8Array(std::move(out));
// }

} // namespace mdsplus