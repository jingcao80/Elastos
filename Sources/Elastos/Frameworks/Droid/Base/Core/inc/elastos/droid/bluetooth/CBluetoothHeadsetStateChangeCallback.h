
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHeadsetStateChangeCallback.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/bluetooth/BluetoothHeadset.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHeadsetStateChangeCallback)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CBluetoothHeadsetStateChangeCallback();

    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IInterface* host);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info);
        return Object::ToString(info);
    }
private:
    BluetoothHeadset* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETSTATECHANGECALLBACK_H__
