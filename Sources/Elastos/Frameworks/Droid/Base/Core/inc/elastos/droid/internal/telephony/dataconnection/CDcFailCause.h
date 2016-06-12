
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCFAILCAUSE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCFAILCAUSE_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDcFailCause.h"
#include "elastos/droid/internal/telephony/dataconnection/DcFailCause.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * Returned as the reason for a connection failure as defined
 * by RIL_DataCallFailCause in ril.h and some local errors.
 */
CarClass(CDcFailCause)
    , public DcFailCause
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCFAILCAUSE_H__
