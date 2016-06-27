#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_COMMON_MORESTRINGS_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_COMMON_MORESTRINGS_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Common {

/**
 * Static utility methods for Strings.
 */
class MoreStrings
    : public Object
{
public:
    TO_STRING_IMPL("MoreStrings")

    static CARAPI_(String) ToSafeString(
        /* [in] */ const String& value);
};

} // namespace Common
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_COMMON_MORESTRINGS_H__