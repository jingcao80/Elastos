
#ifndef __ELASTOS_UTILITY_CTIMEZONEHELPER_H__
#define __ELASTOS_UTILITY_CTIMEZONEHELPER_H__

#include "_Elastos_Utility_CTimeZoneHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {

CarClass(CTimeZoneHelper)
    , public Singleton
    , public ITimeZoneHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAvailableIDs(
        /* [out, callee] */ ArrayOf<String>** ppArray);

    CARAPI GetAvailableIDs(
        /* [in] */ Int32 offsetMillis,
        /* [out, callee] */ ArrayOf<String>** ppArray);

    CARAPI GetDefault(
        /* [out] */ ITimeZone** ppTimezone);

    CARAPI GetTimeZone(
        /* [in] */ const String& id,
        /* [out] */ ITimeZone** ppTimezone);

    CARAPI SetDefault(
        /* [in] */ ITimeZone* pTimeZone);

    CARAPI GetGMT(
        /* [out] */ ITimeZone** timezone);

    CARAPI GetUTC(
        /* [out] */ ITimeZone** timezone);
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CTIMEZONEHELPER_H__
