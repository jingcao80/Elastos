
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHSTATECHANGECALLBACKWRAPPER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHSTATECHANGECALLBACKWRAPPER_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothStateChangeCallbackWrapper.h"
#include "CBluetoothAdapter.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothStateChangeCallbackWrapper)
{
public:
    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IInterface* callback);

private:
    AutoPtr<CBluetoothAdapter::IBluetoothStateChangeCallback> mCallback;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHSTATECHANGECALLBACKWRAPPER_H__
