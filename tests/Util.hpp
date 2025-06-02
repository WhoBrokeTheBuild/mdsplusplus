#ifndef MDSPLUS_TESTS_UTIL_HPP
#define MDSPLUS_TESTS_UTIL_HPP

#include <string>
#include <filesystem>

namespace mdsplus {

inline std::string MakeTempDir()
{
    char tempdir[1024];

    // e.g. /tmp/mdspp-XXXXXX
    snprintf(
        tempdir,
        sizeof(tempdir),
        "%s%cmdspp-XXXXXX",
        std::filesystem::temp_directory_path().c_str(),
        std::filesystem::path::preferred_separator
    );

    // e.g. /tmp/mdspp-123ABC
    mkdtemp(tempdir);
    return tempdir;
}

} // namespace mdsplus

#endif // MDSPLUS_TESTS_UTIL_HPP