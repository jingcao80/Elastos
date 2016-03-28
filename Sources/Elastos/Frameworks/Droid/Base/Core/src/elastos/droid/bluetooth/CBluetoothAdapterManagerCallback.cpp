
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/bluetooth/CBluetoothAdapterManagerCallback.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(CBluetoothAdapterManagerCallback, Object, IIBluetoothManagerCallback, IBinder);

CAR_OBJECT_IMPL(CBluetoothAdapterManagerCallback);

CBluetoothAdapterManagerCallback::CBluetoothAdapterManagerCallback()
{
}

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
    if (mHost->mLeScanClients != NULL)
        mHost->mLeScanClients->Clear();
    if (CBluetoothAdapter::sBluetoothLeAdvertiser != NULL) CBluetoothAdapter::sBluetoothLeAdvertiser->Cleanup();
    if (CBluetoothAdapter::sBluetoothLeScanner != NULL) CBluetoothAdapter::sBluetoothLeScanner->Cleanup();
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
