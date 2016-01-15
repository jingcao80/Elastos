
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTIMEUNITHELPER_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTIMEUNITHELPER_H__

#include "_Elastos_Utility_Concurrent_CTimeUnitHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CTimeUnitHelper)
    , public Singleton
    , public ITimeUnitHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetNANOSECONDS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetMICROSECONDS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetMILLISECONDS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetSECONDS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetMINUTES(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetHOURS(
        /* [out] */ ITimeUnit** unit);

    CARAPI GetDAYS(
        /* [out] */ ITimeUnit** unit);
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTIMEUNITHELPER_H__
