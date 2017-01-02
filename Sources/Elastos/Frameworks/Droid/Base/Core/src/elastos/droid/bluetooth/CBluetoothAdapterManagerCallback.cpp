//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
