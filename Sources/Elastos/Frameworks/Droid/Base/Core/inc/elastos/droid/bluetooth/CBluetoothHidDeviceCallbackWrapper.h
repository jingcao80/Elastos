#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICECALLBACKWRAPPER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICECALLBACKWRAPPER_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHidDeviceCallbackWrapper.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHidDeviceCallbackWrapper)
    , public Object
    , public IIBluetoothHidDeviceCallback
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBluetoothHidDeviceCallbackWrapper();

    CARAPI constructor(
        /* [in] */ IBluetoothHidDeviceCallback* cb);

    CARAPI OnAppStatusChanged(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ IBluetoothHidDeviceAppConfiguration* config,
        /* [in] */ Boolean registered);

    CARAPI OnConnectionStateChanged(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 state);

    CARAPI OnGetReport(
        /* [in] */ Byte type,
        /* [in] */ Byte id,
        /* [in] */ Int32 bufferSize);

    CARAPI OnSetReport(
        /* [in] */ Byte type,
        /* [in] */ Byte id,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI OnSetProtocol(
        /* [in] */ Byte protocol);

    CARAPI OnIntrData(
        /* [in] */ Byte reportId,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI OnVirtualCableUnplug();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IBluetoothHidDeviceCallback> mCallback;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif