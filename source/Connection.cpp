#include <mdsplus.hpp>

// TODO: Move this somewhere better
extern "C" {
    int ConnectToMds(char *host);
    void DisconnectFromMds(int sockId);
    int SetCompressionLevel(int level);
    void FreeMessage(void *m);

    int SendArg(
        int id,
        unsigned char idx,
        char dtype,
        unsigned char nargs,
        unsigned short length,
        char ndims,
        int *dims,
        char *bytes
    );

    int GetAnswerInfoTO(
        int id,
        char *dtype,
        short *length,
        char *ndims,
        int *dims,
        int *numbytes,
        void **dptr,
        void **m,
        int timeout
    );
}

namespace mdsplus {

void Connection::connect(const std::string& hostspec)
{
    if (hostspec == "local") {
        _local = true;
    }
    else {
        // TODO: SetCompressionLevel
        _id = ConnectToMds(const_cast<char *>(hostspec.c_str()));
        if (_id == InvalidConnectionID) {
            // TODO:
            throw MDSplusException();
        }
    }

    _hostspec = hostspec;
}

void Connection::disconnect()
{
    _local = false;
    
    if (_id != InvalidConnectionID) {
        DisconnectFromMds(_id);
        _id = InvalidConnectionID;
    }
}

Data Connection::_get(const std::string& expression, std::vector<Argument>&& argList) const
{
    int status;
    int argIndex = 0;
    uint8_t numberOfArgs = argList.size() + 1;

    status = SendArg(
        _id,
        argIndex,
        DTYPE_T,
        numberOfArgs,
        expression.size(),
        0,
        nullptr,
        const_cast<char *>(expression.data())
    );
    if (IS_NOT_OK(status)) {
        throwException(status);
    }
    
    ++argIndex;

    for (auto& arg : argList) {
        status = MDSplusERROR;

        mdsdsc_t * dscArg = arg.getDescriptor();

        if (dscArg->class_ == CLASS_S) {
            mdsdsc_s_t * scalar = reinterpret_cast<mdsdsc_s_t *>(dscArg);
            status = SendArg(
                _id,
                argIndex,
                scalar->dtype,
                numberOfArgs,
                scalar->length,
                0,
                nullptr,
                scalar->pointer
            );
        }
        else if (dscArg->class_ == CLASS_A) {
            array_coeff * array = reinterpret_cast<array_coeff *>(dscArg);
            status = SendArg(
                _id,
                argIndex,
                array->dtype,
                numberOfArgs,
                array->length,
                array->dimct,
                reinterpret_cast<int *>(array->m),
                array->pointer
            );
        }

        // TODO: Throw exception for unsupported types

        if (IS_NOT_OK(status)) {
            throwException(status);
        }

        ++argIndex;

        arg.free();
    }

    // Switch to temporary variables and building a descriptor :(

    array_coeff * response = (array_coeff *)calloc(1, sizeof(array_coeff));
    mdsdsc_xd_t dscResponse = {
        .length = 0,
        .dtype = DTYPE_DSC,
        .class_ = CLASS_XD,
        .pointer = (mdsdsc_t *)response,
        .l_length = sizeof(array_coeff),
    };

    void * message = nullptr;
    status = GetAnswerInfoTO(
        _id,
        reinterpret_cast<char *>(&response->dtype),
        reinterpret_cast<short *>(&response->length),
        reinterpret_cast<char *>(&response->dimct),
        reinterpret_cast<int *>(response->m),
        reinterpret_cast<int *>(&response->length),
        reinterpret_cast<void **>(&response->pointer),
        &message,
        -1
    );
    if (IS_NOT_OK(status)) {
        if (response->dtype == DTYPE_T) {
            printf("%.*s\n", response->length, response->pointer);
            fflush(stdout);
        }

        throwException(status);
    }
    
    if (response->dimct == 0) {
        response->class_ = CLASS_S;
    }
    else {
        response->class_ = CLASS_A;
        
        // Calculate arsize
        arsize_t size = 1;
        for (int i = 0; i < response->dimct; ++i) {
            size *= response->m[i];
        }
        response->arsize = size * response->length;
    }

    switch (response->dtype) {
    case DTYPE_F:
        response->dtype = DTYPE_FLOAT;
        break;
    case DTYPE_D:
        response->dtype = DTYPE_DOUBLE;
        break;
    case DTYPE_FC:
        response->dtype = DTYPE_FLOAT_COMPLEX;
        break;
    case DTYPE_DC:
        response->dtype = DTYPE_DOUBLE_COMPLEX;
        break;
    default: ;
    }

    return Data(std::move(dscResponse));
}

} // namespace mdsplus