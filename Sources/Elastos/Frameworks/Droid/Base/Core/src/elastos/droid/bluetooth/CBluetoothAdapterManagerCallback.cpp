
#include "CBluetoothAdapterManagerCallback.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

ECode CBluetoothAdapterManagerCallback::OnBluetoothServiceUp(
    /* [in] */ IIBluetooth* bluetoothService)
{
    if (CBluetoothAdapter::VDBG) Logger::D(CBluetoothAdapter::TAG, "onBluetoothServiceUp: %p", bluetoothService);
    AutoLock lock(mHost->mManagerCallbackLock);
    mHost->mService = bluetoothService;
    List<AutoPtr<IIBluetoothManagerCallback> >::Iterator it = mHost->mProxyServiceStateCallbacks.Begin();
    for (; it != mHost->mProxyServiceStateCallbacks.End(); ++it) {
        AutoPtr<IIBluetoothManagerCallback> cb = *it;
        // try {
        if (cb != NULL) {
            cb->OnBluetoothServiceUp(bluetoothService);
        }
        else {
            Logger::D(CBluetoothAdapter::TAG, "onBluetoothServiceUp: cb is null!!!");
        }
        // } catch (Exception e)  { Log.e(TAG,"",e);}
    }
    return NOERROR;
}

ECode CBluetoothAdapterManagerCallback::OnBluetoothServiceDown()
{
    if (CBluetoothAdapter::VDBG) Logger::D(CBluetoothAdapter::TAG, "onBluetoothServiceDown: %p", mHost->mService.Get());
    AutoLock lock(mHost->mManagerCallbackLock);
    mHost->mService = NULL;
    List<AutoPtr<IIBluetoothManagerCallback> >::Iterator it = mHost->mProxyServiceStateCallbacks.Begin();
    for (; it != mHost->mProxyServiceStateCallbacks.End(); ++it) {
        AutoPtr<IIBluetoothManagerCallback> cb = *it;
        // try {
        if (cb != NULL) {
            cb->OnBluetoothServiceDown();
        }
        else {
            Logger::D(CBluetoothAdapter::TAG, "onBluetoothServiceDown: cb is null!!!");
        }
        // } catch (Exception e)  { Log.e(TAG,"",e);}
    }
    return NOERROR;
}

ECode CBluetoothAdapterManagerCallback::constructor(
    /* [in] */ IBluetoothAdapter* host)
{
    mHost = (CBluetoothAdapter*)host;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
