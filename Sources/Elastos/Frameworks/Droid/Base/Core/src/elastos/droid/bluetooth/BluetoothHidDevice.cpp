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

#include "elastos/droid/bluetooth/BluetoothHidDevice.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/bluetooth/CBluetoothHidDeviceAppConfiguration.h"
#include "elastos/droid/bluetooth/CBluetoothHidDeviceCallbackWrapper.h"
#include "elastos/droid/bluetooth/CBluetoothHidDeviceStateChangeCallback.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

const String BluetoothHidDevice::TAG("BluetoothHidDevice");

CAR_INTERFACE_IMPL(BluetoothHidDevice::HidDeviceServiceConnection, Object, IServiceConnection)

CAR_INTERFACE_IMPL_2(BluetoothHidDevice, Object, IBluetoothHidDevice, IBluetoothProfile)

BluetoothHidDevice::HidDeviceServiceConnection::HidDeviceServiceConnection(
    /* [in] */ BluetoothHidDevice* owner)
{
    mOwner = owner;
}

ECode BluetoothHidDevice::HidDeviceServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    SLOGGERD(TAG, "onServiceConnected()")

    mOwner->mService = IIBluetoothHidDevice::Probe(service);

    if (mOwner->mServiceListener != NULL) {
        return mOwner->mServiceListener->OnServiceConnected(IBluetoothProfile::HID_DEVICE,
            mOwner);
    }
    return NOERROR;
}

ECode BluetoothHidDevice::HidDeviceServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    SLOGGERD(TAG, "OnServiceDisconnected()")

    mOwner->mService = NULL;

    if (mOwner->mServiceListener != NULL) {
        return mOwner->mServiceListener->OnServiceDisconnected(IBluetoothProfile::HID_DEVICE);
    }
    return NOERROR;
}

BluetoothHidDevice::BluetoothHidDevice(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* listener)
{
    SLOGGERD(TAG, "BluetoothHidDevice()")

    mContext = context;
    mServiceListener = listener;
    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    CBluetoothHidDeviceStateChangeCallback::New(this,
        (IIBluetoothStateChangeCallback**)&mBluetoothStateChangeCallback);
    CBluetoothAdapter* adtImpl = (CBluetoothAdapter*)mAdapter.Get();
    AutoPtr<IIBluetoothManager> mgr = adtImpl->GetBluetoothManager();

    if (mgr != NULL) {
        // try {
            mgr->RegisterStateChangeCallback(mBluetoothStateChangeCallback);
        // } catch (RemoteException e) {
            // e.printStackTrace();
        // }
    }

    Boolean res;
    DoBind(&res);
}

ECode BluetoothHidDevice::DoBind(
    /* [out] */ Boolean* result)
{
    // using the android service intent action name .Do not make instead with elastos name;
    // zhang.jingcheng link to "packages/apps/Bluetooth/AndroidManifest.xml"
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.bluetooth.IBluetoothHidDevice"), (IIntent**)&intent);
    // Intent intent = new Intent(IBluetoothHidDevice.class.getName());
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IComponentName> comp;
    intent->ResolveSystemService(pm, 0, (IComponentName**)&comp);
    intent->SetComponent(comp);
    Boolean bindResult;
    mContext->BindService(intent, mConnection, 0, &bindResult);
    if (comp == NULL || !bindResult) {
        String str;
        intent->ToString(&str);
        SLOGGERE(TAG, "Could not bind to Bluetooth HID Device Service with %s", str.string())
        *result = FALSE;
        return NOERROR;
    }
    SLOGGERD(TAG, "Bound to HID Device Service")
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothHidDevice::Close()
{
    SLOGGERV(TAG, "Close()")

    CBluetoothAdapter* adtImpl = (CBluetoothAdapter*)mAdapter.Get();
    AutoPtr<IIBluetoothManager> mgr = adtImpl->GetBluetoothManager();
    if (mgr != NULL) {
        // try {
            mgr->UnregisterStateChangeCallback(mBluetoothStateChangeCallback);
        // } catch (RemoteException e) {
            // e.printStackTrace();
        // }
    }

    AutoLock lock(mConnectionLock);
    if (mService != NULL) {
        mService = NULL;
        // try {
        if (FAILED(mContext->UnbindService(mConnection)))
        {
            SLOGGERE(TAG,"close: could not unbind HID Dev service")
        }
        // } catch (IllegalArgumentException e) {
            // Log.e(TAG,"close: could not unbind HID Dev service: ", e);
        // }
   }

    mServiceListener = NULL;
    return NOERROR;
}

ECode BluetoothHidDevice::GetConnectedDevices(
    /* [out] */ IList** devices)
{
    SLOGGERV(TAG, "GetConnectedDevices()")
    *devices = NULL;
    return NOERROR;
}

ECode BluetoothHidDevice::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** devices)
{
    SLOGGERV(TAG, "getDevicesMatchingConnectionStates()")
    *devices = NULL;
    return NOERROR;
}

ECode BluetoothHidDevice::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    String str;
    device->GetAddress(&str);
    SLOGGERV(TAG, "getConnectionState(): device= %s", str.string())

    *state = STATE_DISCONNECTED;
    return NOERROR;
}

ECode BluetoothHidDevice::RegisterApp(
    /* [in] */ IBluetoothHidDeviceAppSdpSettings* sdp,
    /* [in] */ IBluetoothHidDeviceAppQosSettings* inQos,
    /* [in] */ IBluetoothHidDeviceAppQosSettings* outQos,
    /* [in] */ IBluetoothHidDeviceCallback* pCallback,
    /* [out] */ Boolean* result)
{
    SLOGGERV(TAG, "RegisterApp()")

    *result = FALSE;

    if (sdp == NULL || pCallback == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    if (mService != NULL) {
        // try {
            AutoPtr<IBluetoothHidDeviceAppConfiguration> config;
            CBluetoothHidDeviceAppConfiguration::New((IBluetoothHidDeviceAppConfiguration**)&config);
            AutoPtr<IIBluetoothHidDeviceCallback> cbw;
            CBluetoothHidDeviceCallbackWrapper::New(pCallback, (IIBluetoothHidDeviceCallback**)&cbw);
            mService->RegisterApp(config, sdp, inQos, outQos, cbw, result);
        // } catch (RemoteException e) {
            // Log.e(TAG, e.toString());
        // }
    } else {
        SLOGGERW(TAG, "Proxy not attached to service")
    }

    return NOERROR;
}

ECode BluetoothHidDevice::UnregisterApp(
    /* [in] */ IBluetoothHidDeviceAppConfiguration* config,
    /* [out] */ Boolean* result)
{
    SLOGGERV(TAG, "UnregisterApp()")

    *result = FALSE;

    if (mService != NULL) {
        // try {
            mService->UnregisterApp(config, result);
        // } catch (RemoteException e) {
            // Log.e(TAG, e.toString());
        // }
    } else {
        SLOGGERW(TAG, "Proxy not attached to service")
    }

    return NOERROR;
}

/**
 * Sends report to remote host using interrupt channel.
 *
 * @param id Report Id, as defined in descriptor. Can be 0 in case Report Id
 *            are not defined in descriptor.
 * @param data Report data, not including Report Id.
     * @return
     */
ECode BluetoothHidDevice::SendReport(
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Boolean* result)
{
    SLOGGERV(TAG, "SendReport(): id=%d", id);

    *result = FALSE;

    if (mService != NULL) {
        // try {
        mService->SendReport(id, data, result);
        // } catch (RemoteException e) {
            // Log.e(TAG, e.toString());
        // }
    } else {
        SLOGGERW(TAG, "Proxy not attached to service")
    }

    return NOERROR;
}

    /**
     * Sends report to remote host as reply for GET_REPORT request from
     * {@link BluetoothHidDeviceCallback#onGetReport(byte, byte, int)}.
     *
     * @param type Report Type, as in request.
     * @param id Report Id, as in request.
     * @param data Report data, not including Report Id.
     * @return
     */
ECode BluetoothHidDevice::ReplyReport(
    /* [in] */ Byte type,
    /* [in] */ Byte id,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Boolean* result)
{
    SLOGGERV(TAG, "ReplyReport():type = %d, id = %d", type, id);

    *result = FALSE;

    if (mService != NULL) {
        // try {
        mService->ReplyReport(type, id, data, result);
        // } catch (RemoteException e) {
            // Log.e(TAG, e.toString());
        // }
    } else {
        SLOGGERW(TAG, "Proxy not attached to service")
    }

    return NOERROR;
}

    /**
     * Sends error handshake message as reply for invalid SET_REPORT request
     * from {@link BluetoothHidDeviceCallback#onSetReport(byte, byte, byte[])}.
     *
     * @param error Error to be sent for SET_REPORT via HANDSHAKE.
     * @return
     */
ECode BluetoothHidDevice::ReportError(
    /* [in] */ Byte error,
    /* [out] */ Boolean* result)
{
    SLOGGERV(TAG, "ReportError(): id = %d", error);

    *result = FALSE;

    if (mService != NULL) {
        // try {
        mService->ReportError(error, result);
        // } catch (RemoteException e) {
            // Log.e(TAG, e.toString());
        // }
    } else {
        SLOGGERW(TAG, "Proxy not attached to service")
    }

    return NOERROR;
}

    /**
     * Sends Virtual Cable Unplug to currently connected host.
     *
     * @return
     */
ECode BluetoothHidDevice::Unplug(
    /* [out] */ Boolean* result)
{
    SLOGGERV(TAG, "Unplug()");

    *result = FALSE;

    if (mService != NULL) {
        // try {
        mService->Unplug(result);
        // } catch (RemoteException e) {
            // Log.e(TAG, e.toString());
        // }
    } else {
        SLOGGERW(TAG, "Proxy not attached to service")
    }

    return NOERROR;
}

    /**
     * Initiates connection to host which currently has Virtual Cable
     * established with device.
     *
     * @return
     */
ECode BluetoothHidDevice::Connect(
    /* [out] */ Boolean* result)
{
    SLOGGERV(TAG, "Connect()");

    *result = FALSE;

    if (mService != NULL) {
        // try {
        mService->Connect(result);
        // } catch (RemoteException e) {
            // Log.e(TAG, e.toString());
        // }
    } else {
        SLOGGERW(TAG, "Proxy not attached to service")
    }

    return NOERROR;
}

    /**
     * Disconnects from currently connected host.
     *
     * @return
     */
ECode BluetoothHidDevice::Disconnect(
    /* [out] */ Boolean* result)
{
    SLOGGERV(TAG, "Disconnect()");

    *result = FALSE;

    if (mService != NULL) {
        // try {
        mService->Disconnect(result);
        // } catch (RemoteException e) {
            // Log.e(TAG, e.toString());
        // }
    } else {
        SLOGGERW(TAG, "Proxy not attached to service")
    }

    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos