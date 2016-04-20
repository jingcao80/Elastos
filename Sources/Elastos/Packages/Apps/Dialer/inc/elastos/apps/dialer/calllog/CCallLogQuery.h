
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGQUERY_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGQUERY_H__

namespace Elastos{
namespace Apps{
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

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CALLLOGQUERY_H__
