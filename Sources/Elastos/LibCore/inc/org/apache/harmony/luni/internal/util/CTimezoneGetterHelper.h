#ifndef __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_CTIMEZONEGETTERHELPER_H__
#define __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_CTIMEZONEGETTERHELPER_H__

#include "_Org_Apache_Harmony_Luni_Internal_Util_CTimezoneGetterHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Luni {
namespace Internal {
namespace Util {

CarClass(CTimezoneGetterHelper)
    , public Singleton
    , public ITimezoneGetterHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ ITimezoneGetter** result);

    CARAPI SetInstance(
        /* [in] */ ITimezoneGetter* getter);
};

} // namespace Util
} // namespace Internal
} // namespace Luni
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_CTIMEZONEGETTERHELPER_H__
