
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CCDMAAPNPROFILETRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CCDMAAPNPROFILETRACKER_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CCdmaApnProfileTracker.h"
#include "elastos/droid/internal/telephony/dataconnection/CdmaApnProfileTracker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * {@hide}
 */
CarClass(CCdmaApnProfileTracker)
    , public CdmaApnProfileTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CCDMAAPNPROFILETRACKER_H__
