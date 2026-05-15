#ifndef MDSPLUS_VERSION_HPP
#define MDSPLUS_VERSION_HPP

extern "C" {

    // Needed for MdsRelease
    #include <mdsshr.h>

} // extern "C"

namespace mdsplus {

inline const char * GetVersion() {
    return MdsRelease();
}

} // namespace mdsplus

#endif // MDSPLUS_VERSION_HPP