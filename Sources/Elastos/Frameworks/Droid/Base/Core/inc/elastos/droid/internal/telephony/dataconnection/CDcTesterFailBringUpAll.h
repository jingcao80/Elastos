
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTESTERFAILBRINGUPALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTESTERFAILBRINGUPALL_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDcTesterFailBringUpAll.h"
#include "elastos/droid/internal/telephony/dataconnection/DcTesterFailBringUpAll.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * A package level call that causes all DataConnection bringUp calls to fail a specific
 * number of times. Here is an example that sets counter to 2 and cause to -3 for all instances:
 *    adb shell am broadcast -a com.android.internal.telephony.dataconnection.action_fail_bringup \
 *     --ei counter 2 --ei fail_cause -3
 *
 * Also you can add a suggested retry time if desired:
 *     --ei suggested_retry_time 5000
 *
 * The fail_cause is one of {@link DcFailCause}
 */
CarClass(CDcTesterFailBringUpAll)
    , public DcTesterFailBringUpAll
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTESTERFAILBRINGUPALL_H__
