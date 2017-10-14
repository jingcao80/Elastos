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
#include "Elastos.Droid.Content.h"
#include "elastos/droid/bluetooth/BluetoothManager.h"
#include "elastos/droid/bluetooth/BluetoothGattServer.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                           BluetoothManager
//=====================================================================
const String BluetoothManager::TAG("BluetoothManager");
const Boolean BluetoothManager::DBG = true;
const Boolean BluetoothManager::VDBG = true;

CAR_INTERFACE_IMPL(BluetoothManager, Object, IBluetoothManager);

BluetoothManager::BluetoothManager()
{
}

ECode BluetoothManager::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IContext> appContext;
    context->GetApplicationContext((IContext**)&appContext);
    if (context == NULL) {
        //throw new IllegalArgumentException(
        //        "context not associated with any application (using a mock context?)");
        Logger::E("BluetoothManager", "context not associated with any application (using a mock context?)");
    }
    // Legacy api - getDefaultAdapter does not take in the context
    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    return NOERROR;
}

ECode BluetoothManager::GetAdapter(
    /* [out] */ IBluetoothAdapter** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdapter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothManager::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 profile,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG,"getConnectionState()");

    //List<BluetoothDevice> connectedDevices = getConnectedDevices(profile);
    AutoPtr<IList> connectedDevices;
    GetConnectedDevices(profile, (IList**)&connectedDevices);
    Int32 size;
    connectedDevices->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        connectedDevices->Get(i, (IInterface**)&obj);
        IBluetoothDevice* connectedDevice = IBluetoothDevice::Probe(obj);
        Boolean eq = FALSE;
        if (IObject::Probe(device)->Equals(connectedDevice, &eq), eq) {
            *result = IBluetoothProfile::STATE_CONNECTED;
            return NOERROR;
        }
    }

    *result = IBluetoothProfile::STATE_DISCONNECTED;
    return NOERROR;
}

ECode BluetoothManager::GetConnectedDevices(
    /* [in] */ Int32 profile,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG,"getConnectedDevices");
    if (profile != IBluetoothProfile::GATT && profile != IBluetoothProfile::GATT_SERVER) {
        //throw new IllegalArgumentException("Profile not supported: " + profile);
        Logger::E(TAG, "Profile not supported: %d", profile);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //List<BluetoothDevice> connectedDevices = new ArrayList<BluetoothDevice>();
    AutoPtr<IList> connectedDevices;
    CArrayList::New((IList**)&connectedDevices);

    //try {
    AutoPtr<IIBluetoothManager> managerService = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    AutoPtr<IIBluetoothGatt> iGatt;
    managerService->GetBluetoothGatt((IIBluetoothGatt**)&iGatt);
    if (iGatt == NULL) {
        *result = connectedDevices;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(1);
    states->Set(0, IBluetoothProfile::STATE_CONNECTED);
    iGatt->GetDevicesMatchingConnectionStates(
        states, result);
    //} catch (RemoteException e) {
    //    Log.e(TAG,"",e);
    //}

    return NOERROR;
}

ECode BluetoothManager::GetDevicesMatchingConnectionStates(
    /* [in] */ Int32 profile,
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG,"getDevicesMatchingConnectionStates");

    if (profile != IBluetoothProfile::GATT && profile != IBluetoothProfile::GATT_SERVER) {
        //throw new IllegalArgumentException("Profile not supported: " + profile);
        Logger::E(TAG, "Profile not supported: %d", profile);
        return NOERROR;
    }

    //List<BluetoothDevice> devices = new ArrayList<BluetoothDevice>();
    AutoPtr<IList> devices;
    CArrayList::New((IList**)&devices);
    //try {
        AutoPtr<IIBluetoothManager> managerService = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
        AutoPtr<IIBluetoothGatt> iGatt;
        managerService->GetBluetoothGatt((IIBluetoothGatt**)&iGatt);
        if (iGatt == NULL) {
            *result = devices;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        iGatt->GetDevicesMatchingConnectionStates(states, result);
    //} catch (RemoteException e) {
    //    Log.e(TAG,"",e);
    //}
    return NOERROR;
}

ECode BluetoothManager::OpenGattServer(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothGattServerCallback* callback,
    /* [out] */ IBluetoothGattServer** result)
{
    VALIDATE_NOT_NULL(result);
    return OpenGattServer(context, callback, IBluetoothDevice::TRANSPORT_AUTO, result);
}

ECode BluetoothManager::OpenGattServer(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothGattServerCallback* callback,
    /* [in] */ Int32 transport,
    /* [out] */ IBluetoothGattServer** result)
{
    VALIDATE_NOT_NULL(result);
    if (context == NULL || callback == NULL) {
        //throw new IllegalArgumentException("null parameter: " + context + " " + callback);
        Logger::E("BluetoothManager", "null parameter const or callback");
        *result = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // TODO(Bluetooth) check whether platform support BLE
    //     Do the check here or in GattServer?

    //try {
        AutoPtr<IIBluetoothManager> managerService = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
        AutoPtr<IIBluetoothGatt> iGatt;
        managerService->GetBluetoothGatt((IIBluetoothGatt**)&iGatt);
        if (iGatt == NULL) {
            Logger::E(TAG, "Fail to get GATT Server connection");
            *result = NULL;
            return NOERROR;
        }
        AutoPtr<IBluetoothGattServer> mGattServer = new BluetoothGattServer(context, iGatt,transport);
        Boolean regStatus;
        ((BluetoothGattServer*)mGattServer.Get())->RegisterCallback(callback, &regStatus);
        *result = regStatus? mGattServer : NULL;
        if(*result) {
            REFCOUNT_ADD(*result);
        }
    //} catch (RemoteException e) {
    //    Log.e(TAG,"",e);
    //    return null;
    //}
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
