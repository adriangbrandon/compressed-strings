
#include "libcds-utils/libcdsBasics.h"
#include "libcds-utils/cppUtils.h"

namespace cds_utils
{

    uint transform(const string & s) {
        stringstream ss;
        ss << s;
        uint ret;
        ss >> ret;
        return ret;
    }

};
