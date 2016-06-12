
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTRACKER_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDcTracker.h"
#include "elastos/droid/internal/telephony/dataconnection/DcTracker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * {@hide}
 */
CarClass(CDcTracker)
    , public DcTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTRACKER_H__
