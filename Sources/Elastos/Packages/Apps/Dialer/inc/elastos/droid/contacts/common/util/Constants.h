
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CONSTANTS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CONSTANTS_H__

#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

class Constants
{
public:
    /**
     * Log tag for performance measurement.
     * To enable: adb shell setprop log.tag.ContactsPerf VERBOSE
     */
    static const String PERFORMANCE_TAG;

    // Used for lookup URI that contains an encoded JSON string.
    static const String LOOKUP_URI_ENCODED;
};

} // Util
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_CONSTANTS_H__
