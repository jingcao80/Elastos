
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CPHONEQUERY_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CPHONEQUERY_H__

#include "_Elastos_Apps_Dialer_CallLog_CPhoneQuery.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

/**
 * The query to look up the {@link ContactInfo} for a given number in the Call Log.
 */
CarClass(CPhoneQuery)
    , public Singleton
    , public IPhoneQuery
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Get_PROJECTION(
        /* [out, callee] */ ArrayOf<String>** result);

public:
    static const String _PROJECTION[];
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CPHONEQUERY_H__
