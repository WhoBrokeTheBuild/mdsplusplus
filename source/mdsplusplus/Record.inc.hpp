#ifndef MDSPLUS_RECORD_INC_HPP
#define MDSPLUS_RECORD_INC_HPP

#include "Record.hpp"

namespace mdsplus {

inline Data Function::call() const
{
    int status;

    descriptor_function * dsc = (descriptor_function *)getDescriptor();

    // TODO: Tree*
    opcode_t * opcode = (opcode_t *)dsc->pointer;
    mdsdsc_xd_t out = MDSDSC_XD_INITIALIZER;
    _intrinsic(*opcode, dsc->ndesc, dsc->arguments, &out);
    if (IS_NOT_OK(status)) {
        throwException(status);
    }

    return Data(std::move(out));
}


} // namespace mdsplus

#endif // MDSPLUS_RECORD_INC_HPP