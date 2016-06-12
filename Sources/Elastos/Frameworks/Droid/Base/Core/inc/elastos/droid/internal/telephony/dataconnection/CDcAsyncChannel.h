
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCASYNCCHANNEL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCASYNCCHANNEL_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDcAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/DcAsyncChannel.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * AsyncChannel to a DataConnection
 */
CarClass(CDcAsyncChannel)
    , public DcAsyncChannel
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCASYNCCHANNEL_H__
