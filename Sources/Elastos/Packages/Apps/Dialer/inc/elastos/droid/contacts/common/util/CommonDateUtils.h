
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_COMMONDATEUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_COMMONDATEUTILS_H__

#include "_Elastos.Droid.Dialer.h"

using Elastos::Text::ISimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Common date utilities.
 */
class CommonDateUtils
{
public:
    // All the SimpleDateFormats in this class use the UTC timezone
    static const AutoPtr<ISimpleDateFormat> NO_YEAR_DATE_FORMAT;
    static const AutoPtr<ISimpleDateFormat> FULL_DATE_FORMAT;
    static const AutoPtr<ISimpleDateFormat> DATE_AND_TIME_FORMAT;
    static const AutoPtr<ISimpleDateFormat> NO_YEAR_DATE_AND_TIME_FORMAT;

    /**
     * Exchange requires 8:00 for birthdays
     */
    static const Int32 DEFAULT_HOUR = 8;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_COMMONDATEUTILS_H__
