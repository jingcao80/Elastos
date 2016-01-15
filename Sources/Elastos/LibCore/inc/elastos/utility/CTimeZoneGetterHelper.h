#ifndef __ELASTOS_UTILITY_CTIMEZONEGETTERHELPER_H__
#define __ELASTOS_UTILITY_CTIMEZONEGETTERHELPER_H__

#include "_Elastos_Utility_CTimeZoneGetterHelper.h"
#include "Singleton.h"


namespace Elastos {
namespace Utility {

CarClass(CTimeZoneGetterHelper)
    , public Singleton
    , public ITimeZoneGetterHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ ITimeZoneGetter** instance);

    CARAPI SetInstance(
        /* [in] */ ITimeZoneGetter* instance);
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CTIMEZONEGETTERHELPER_H__
