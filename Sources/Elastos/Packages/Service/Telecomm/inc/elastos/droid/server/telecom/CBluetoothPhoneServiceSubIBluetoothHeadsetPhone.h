
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CBLUETOOTHPHONESERVICESUBIBLUETOOTHHEADSETPHONE_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CBLUETOOTHPHONESERVICESUBIBLUETOOTHHEADSETPHONE_H__

#include "_Elastos_Droid_Server_Telecom_CBluetoothPhoneServiceSubIBluetoothHeadsetPhone.h"
#include "elastos/droid/server/telecom/BluetoothPhoneService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Binder implementation of IBluetoothHeadsetPhone. Implements the command interface that the
 * bluetooth headset code uses to control call.
 */
CarClass(CBluetoothPhoneServiceSubIBluetoothHeadsetPhone)
    , public BluetoothPhoneService::SubIBluetoothHeadsetPhone
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CBLUETOOTHPHONESERVICEINNERIBLUETOOTHHEADSETPHONE_H__
