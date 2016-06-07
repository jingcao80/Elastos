
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CBLUETOOTHPHONESERVICE_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CBLUETOOTHPHONESERVICE_H__

#include "_Elastos_Droid_Server_Telecom_CBluetoothPhoneService.h"
#include "elastos/droid/server/telecom/BluetoothPhoneService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CarClass(CBluetoothPhoneService)
    , public BluetoothPhoneService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CBLUETOOTHPHONESERVICE_H__
