
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHSTATECHANGECALLBACKWRAPPER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHSTATECHANGECALLBACKWRAPPER_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Bluetooth_CBluetoothStateChangeCallbackWrapper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>
//#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothStateChangeCallbackWrapper)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothStateChangeCallbackWrapper();

    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IInterface* callback);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
private:
    //AutoPtr<CBluetoothAdapter::IBluetoothStateChangeCallback> mCallback;
    AutoPtr<IIBluetoothStateChangeCallback> mCallback;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHSTATECHANGECALLBACKWRAPPER_H__
