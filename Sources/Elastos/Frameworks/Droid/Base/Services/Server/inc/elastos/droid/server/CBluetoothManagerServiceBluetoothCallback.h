
#ifndef __ELASTOS_DROID_SERVER_CBLUETOOTH_MANAGER_SERVICE_BLUETOOTHCALLBACK
#define __ELASTOS_DROID_SERVER_CBLUETOOTH_MANAGER_SERVICE_BLUETOOTHCALLBACK

#include "_Elastos_Droid_Server_CBluetoothManagerServiceBluetoothCallback.h"
#include "elastos/droid/server/CBluetoothManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CBluetoothManagerServiceBluetoothCallback)
    , public CBluetoothManagerService::BluetoothCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CBLUETOOTH_MANAGER_SERVICE_BLUETOOTHCALLBACK
