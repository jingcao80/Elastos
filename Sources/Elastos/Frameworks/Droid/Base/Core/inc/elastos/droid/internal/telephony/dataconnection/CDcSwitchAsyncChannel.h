
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCSWITCHASYNCCHANNEL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCSWITCHASYNCCHANNEL_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDcSwitchAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/DcSwitchAsyncChannel.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CarClass(CDcSwitchAsyncChannel)
    , public DcSwitchAsyncChannel
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCSWITCHASYNCCHANNEL_H__
