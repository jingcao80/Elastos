
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTESTERDEACTIVATEALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTESTERDEACTIVATEALL_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDcTesterDeactivateAll.h"
#include "elastos/droid/internal/telephony/dataconnection/DcTesterDeactivateAll.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * To bring down all DC's send the following intent:
 *
 * adb shell am broadcast -a com.android.internal.telephony.dataconnection.action_deactivate_all
 */
CarClass(CDcTesterDeactivateAll)
    , public DcTesterDeactivateAll
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTESTERDEACTIVATEALL_H__
