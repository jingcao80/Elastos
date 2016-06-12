
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDDSSCHEDULER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDDSSCHEDULER_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDdsScheduler.h"
#include "elastos/droid/internal/telephony/dataconnection/DdsScheduler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/* {@hide} */
CarClass(CDdsScheduler)
    , public DdsScheduler
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDDSSCHEDULER_H__
