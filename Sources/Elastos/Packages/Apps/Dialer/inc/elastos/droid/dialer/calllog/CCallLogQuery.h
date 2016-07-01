
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGQUERY_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGQUERY_H__

#include "_Elastos_Droid_Dialer_CallLog_CCallLogQuery.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * Helper class operating on call log notifications.
 */
CarClass(CCallLogQuery)
    , public Singleton
    , public ICallLogQuery
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI Get_PROJECTION(
        /* [out, callee] */ ArrayOf<String>** result);

public:
    static const String _PROJECTION[];
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLLOGQUERY_H__
