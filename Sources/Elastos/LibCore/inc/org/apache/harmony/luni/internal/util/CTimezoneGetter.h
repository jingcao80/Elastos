#ifndef __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_CTIMEZONEGETTER_H__
#define __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_CTIMEZONEGETTER_H__

#include "_Org_Apache_Harmony_Luni_Internal_Util_CTimezoneGetter.h"
#include "org/apache/harmony/luni/internal/util/TimezoneGetter.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Luni {
namespace Internal {
namespace Util {

CarClass(CTimezoneGetter)
    , public TimezoneGetter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Util
} // namespace Internal
} // namespace Luni
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_CTIMEZONEGETTER_H__
