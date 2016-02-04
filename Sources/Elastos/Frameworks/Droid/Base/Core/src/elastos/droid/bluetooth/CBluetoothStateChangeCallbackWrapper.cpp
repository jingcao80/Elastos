
#include "elastos/droid/bluetooth/CBluetoothStateChangeCallbackWrapper.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(CBluetoothStateChangeCallbackWrapper, Object, IIBluetoothStateChangeCallback, IBinder);

CAR_OBJECT_IMPL(CBluetoothStateChangeCallbackWrapper);

CBluetoothStateChangeCallbackWrapper::CBluetoothStateChangeCallbackWrapper()
{
}

ECode CBluetoothStateChangeCallbackWrapper::OnBluetoothStateChange(
    /* [in] */ Boolean on)
{
    mCallback->OnBluetoothStateChange(on);
    return NOERROR;
}

ECode CBluetoothStateChangeCallbackWrapper::constructor(
    /* [in] */ IInterface* callback)
{
    //mCallback = (CBluetoothAdapter::IBluetoothStateChangeCallback*)callback;
    mCallback = IIBluetoothStateChangeCallback::Probe(callback);
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
