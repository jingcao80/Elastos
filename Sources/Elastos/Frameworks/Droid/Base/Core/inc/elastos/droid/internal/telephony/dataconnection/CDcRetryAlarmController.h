
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCRETRYALARMCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCRETRYALARMCONTROLLER_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDcRetryAlarmController.h"
#include "elastos/droid/internal/telephony/dataconnection/DcRetryAlarmController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * The Data Connection Retry Alarm Controller.
 */
CarClass(CDcRetryAlarmController)
    , public DcRetryAlarmController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCRETRYALARMCONTROLLER_H__
