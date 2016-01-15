
#include "CBluetoothStateChangeCallbackWrapper.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

ECode CBluetoothStateChangeCallbackWrapper::OnBluetoothStateChange(
    /* [in] */ Boolean on)
{
    mCallback->OnBluetoothStateChange(on);
    return NOERROR;
}

ECode CBluetoothStateChangeCallbackWrapper::constructor(
    /* [in] */ IInterface* callback)
{
    mCallback = (CBluetoothAdapter::IBluetoothStateChangeCallback*)callback;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
