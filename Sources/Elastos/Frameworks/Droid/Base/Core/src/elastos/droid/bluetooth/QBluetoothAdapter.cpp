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

#include "elastos/droid/bluetooth/QBluetoothAdapter.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/bluetooth/CLeLppClientWrapper.h"
#include "elastos/droid/bluetooth/CQBluetoothManagerCallback.h"
#include "elastos/droid/bluetooth/CBluetoothManagerCallback.h"

#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

const String QBluetoothAdapter::TAG("QBluetoothAdapter");
const Boolean QBluetoothAdapter::DBG = FALSE;
const Boolean QBluetoothAdapter::VDBG = FALSE;

AutoPtr<QBluetoothAdapter> QBluetoothAdapter::sAdapter;
AutoPtr<IBluetoothAdapter> QBluetoothAdapter::mAdapter;


CAR_INTERFACE_IMPL_3(QBluetoothAdapter::LeLppClientWrapper, Object, ILeLppClientWrapper, IIQBluetoothAdapterCallback, IBinder)

ECode QBluetoothAdapter::LeLppClientWrapper::constructor(
    /* [in] */ IQBluetoothAdapter* adapter,
    /* [in] */ IIQBluetooth* adapterService,
    /* [in] */ const String& address,
    /* [in] */ ILeLppCallback* callback)
{
    IWeakReferenceSource* weakSource = IWeakReferenceSource::Probe(adapter);
    if (weakSource != NULL) {
        weakSource->GetWeakReference((IWeakReference**)&mAdapter);
    }
    mQBluetoothAdapterService = adapterService;
    mDevice = address;
    mClient = callback;
    return NOERROR;
}

QBluetoothAdapter::LeLppClientWrapper::LeLppClientWrapper()
{}

ECode QBluetoothAdapter::LeLppClientWrapper::Register2(
    /* [in] */ Boolean add,
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    if(mQBluetoothAdapterService != NULL) {
        // try {
        if(FAILED(mQBluetoothAdapterService->RegisterLeLppRssiMonitorClient(mDevice, this, add, result)))
        {
            SLOGGERE("QBluetoothAdapter", "LeLppClientWrapper RemoteException: Register2")
        }
        // } catch (RemoteException e) {
            // Log.w(TAG, "", e);
        // }
    }
    return NOERROR;
}

ECode QBluetoothAdapter::LeLppClientWrapper::WriteRssiThreshold(
    /* [in] */ Byte min,
    /* [in] */ Byte max)
{
    if(mQBluetoothAdapterService != NULL) {
        // try {
        if(FAILED(mQBluetoothAdapterService->WriteLeLppRssiThreshold(mDevice, min, max)))
        {
            SLOGGERE("QBluetoothAdapter", "LeLppClientWrapper RemoteException: WriteRssiThreshold")
        }
        // } catch (RemoteException e) {
            // Log.w(TAG, "", e);
        // }
    }
    return NOERROR;
}

ECode QBluetoothAdapter::LeLppClientWrapper::EnableMonitor(
    /* [in] */ Boolean enable)
{
    if(mQBluetoothAdapterService != NULL) {
        // try {
        if(FAILED(mQBluetoothAdapterService->EnableLeLppRssiMonitor(mDevice, enable)))
        {
            SLOGGERE("QBluetoothAdapter", "LeLppClientWrapper RemoteException: EnableMonitor")
        }
        // } catch (RemoteException e) {
            // Log.w(TAG, "", e);
        // }
    }
    return NOERROR;
}

ECode QBluetoothAdapter::LeLppClientWrapper::ReadRssiThreshold()
{
    if(mQBluetoothAdapterService != NULL) {
        // try {
        if(FAILED(mQBluetoothAdapterService->ReadLeLppRssiThreshold(mDevice)))
        {
            SLOGGERE("QBluetoothAdapter", "LeLppClientWrapper RemoteException: ReadRssiThreshold")
        }
        // } catch (RemoteException e) {
            // Log.w(TAG, "", e);
        // }
    }
    return NOERROR;
}

ECode QBluetoothAdapter::LeLppClientWrapper::OnWriteRssiThreshold(
    /* [in] */ const String& address,
    /* [in] */ Int32 status)
{
    if(mClient != NULL) {
        // try {
        if(FAILED(mClient->OnWriteRssiThreshold(status)))
        {
            SLOGGERE("QBluetoothAdapter", "Unhandled exception: OnWriteRssiThreshold")
        }
        // } catch (Exception ex) {
        //     Log.w(TAG, "Unhandled exception: " + ex);
        // }
    }
    return NOERROR;
}

ECode QBluetoothAdapter::LeLppClientWrapper::OnReadRssiThreshold(
    /* [in] */ const String& address,
    /* [in] */ Int32 low,
    /* [in] */ Int32 upper,
    /* [in] */ Int32 alert,
    /* [in] */ Int32 status)
{
    if(mClient != NULL) {
        // try {
        if(FAILED(mClient->OnReadRssiThreshold(low, upper, alert, status)))
        {
            SLOGGERE("QBluetoothAdapter", "Unhandled exception: OnReadRssiThreshold")
        }
        // } catch (Exception ex) {
        //     Log.w(TAG, "Unhandled exception: " + ex);
        // }
    }
    return NOERROR;
}

ECode QBluetoothAdapter::LeLppClientWrapper::OnEnableRssiMonitor(
    /* [in] */ const String& address,
    /* [in] */ Int32 enable,
    /* [in] */ Int32 status)
{
    if(mClient != NULL) {
        // try {
        if(FAILED(mClient->OnEnableRssiMonitor(enable, status)))
        {
            SLOGGERE("QBluetoothAdapter", "Unhandled exception: OnEnableRssiMonitor")
        }
        // } catch (Exception ex) {
        //     Log.w(TAG, "Unhandled exception: " + ex);
        // }
    }
    return NOERROR;
}

ECode QBluetoothAdapter::LeLppClientWrapper::OnRssiThresholdEvent(
    /* [in] */ const String& address,
    /* [in] */ Int32 evtType,
    /* [in] */ Int32 rssi)
{
    if(mClient != NULL) {
        // try {
        if(FAILED(mClient->OnRssiThresholdEvent(evtType, rssi)))
        {
            SLOGGERE("QBluetoothAdapter", "Unhandled exception: OnRssiThresholdEvent")
        }
        // } catch (Exception ex) {
        //     Log.w(TAG, "Unhandled exception: " + ex);
        // }
    }
    return NOERROR;
}

ECode QBluetoothAdapter::LeLppClientWrapper::OnUpdateLease(
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    if(mClient != NULL) {
        // try {
        if(FAILED(mClient->OnUpdateLease(result)))
        {
            SLOGGERE("QBluetoothAdapter", "Unhandled exception: OnUpdateLease")
        }
        // } catch (Exception ex) {
        //     Log.w(TAG, "Unhandled exception: " + ex);
        // }
    }
    return NOERROR;
}

ECode QBluetoothAdapter::LeLppClientWrapper::ToString(
    /* [out] */ String* str)
{
    *str = "QBluetoothAdapter::LeLppClientWrapper";
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(QBluetoothAdapter::BluetoothManagerCallback, Object, IIBluetoothManagerCallback, IBinder)

ECode QBluetoothAdapter::BluetoothManagerCallback::constructor(
        /* [in] */ IQBluetoothAdapter* host)
{
    mAdapter = (QBluetoothAdapter*)host;
    return NOERROR;
}

ECode QBluetoothAdapter::BluetoothManagerCallback::OnBluetoothServiceUp(
        /* [in] */ IIBluetooth* bluetoothService)
{
    // {    AutoLock syncLock(mManagerCallback);
    AutoLock lock(*this);
        //initialize the global params again
    mAdapter->mService = bluetoothService;
    String str("NULL"), qStr("NULL"), mStr("NULL");
    AutoPtr<IObject> obj = IObject::Probe(mAdapter->mService);
    AutoPtr<IObject> qObj = IObject::Probe(mAdapter->mQService);
    AutoPtr<IObject> mObj = IObject::Probe(mAdapter->mManagerService);
    if (obj != NULL) {
        obj->ToString(&str);
    }
    if (qObj != NULL) {
        qObj->ToString(&qStr);
    }
    if (mObj != NULL) {
        mObj->ToString(&mStr);
    }
    SLOGGERI(TAG,"onQBluetoothServiceUp: Adapter ON: mService: %s, mQService: %s, ManagerService: %s",
        str.string(), qStr.string(), mStr.string())
    // }
    return NOERROR;
}

ECode QBluetoothAdapter::BluetoothManagerCallback::OnBluetoothServiceDown()
{
    // {    AutoLock syncLock(mManagerCallback);
    AutoLock lock(*this);
        //initialize the global params again
    mAdapter->mService = NULL;
    String str("NULL"), qStr("NULL"), mStr("NULL");
    AutoPtr<IObject> obj = IObject::Probe(mAdapter->mService);
    AutoPtr<IObject> qObj = IObject::Probe(mAdapter->mQService);
    if (obj != NULL) {
        obj->ToString(&str);
    }
    if (qObj != NULL) {
        qObj->ToString(&qStr);
    }

    SLOGGERI(TAG,"onQBluetoothServiceUp: Adapter ON: mService: %s, mQService: %s",
        str.string(), qStr.string())
    // }
    return NOERROR;
}

ECode QBluetoothAdapter::BluetoothManagerCallback::ToString(
    /* [out] */ String* str)
{
    *str = "QBluetoothAdapter::BluetoothManagerCallback";
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(QBluetoothAdapter::QBluetoothManagerCallback, Object, IIQBluetoothManagerCallback, IBinder)

ECode QBluetoothAdapter::QBluetoothManagerCallback::constructor(
    /* [in] */ IQBluetoothAdapter* host)
{
    mAdapter = (QBluetoothAdapter*)host;
    return NOERROR;
}

ECode QBluetoothAdapter::QBluetoothManagerCallback::OnQBluetoothServiceUp(
    /* [in] */ IIQBluetooth* qcbluetoothService)
{
    // {    AutoLock syncLock(mManagerCallback);
    AutoLock lock(*this);
        //initialize the global params again
    mAdapter->mQService = qcbluetoothService;
    String str("NULL"), qStr("NULL"), mStr("NULL");
    AutoPtr<IObject> obj = IObject::Probe(mAdapter->mService);
    AutoPtr<IObject> qObj = IObject::Probe(mAdapter->mQService);
    AutoPtr<IObject> mObj = IObject::Probe(mAdapter->mManagerService);
    if (obj != NULL) {
        obj->ToString(&str);
    }
    if (qObj != NULL) {
        qObj->ToString(&qStr);
    }
    if (mObj != NULL) {
        mObj->ToString(&mStr);
    }
    SLOGGERI(TAG,"onQBluetoothServiceUp: Adapter ON: mService: %s, mQService: %s, ManagerService: %s",
        str.string(), qStr.string(), mStr.string())
    // }
    return NOERROR;
}

ECode QBluetoothAdapter::QBluetoothManagerCallback::OnQBluetoothServiceDown()
{
    // {    AutoLock syncLock(mManagerCallback);
    AutoLock lock(*this);
        //initialize the global params again
    mAdapter->mQService = NULL;
    String str("NULL"), qStr("NULL");
    AutoPtr<IObject> obj = IObject::Probe(mAdapter->mService);
    AutoPtr<IObject> qObj = IObject::Probe(mAdapter->mQService);
    if (obj != NULL) {
        obj->ToString(&str);
    }
    if (qObj != NULL) {
        qObj->ToString(&qStr);
    }

    SLOGGERI(TAG,"onQBluetoothServiceUp: Adapter OFF: mService: %s, mQService: %s",
        str.string(), qStr.string())
    // }
    return NOERROR;
}

ECode QBluetoothAdapter::QBluetoothManagerCallback::ToString(
    /* [out] */ String* str)
{
    *str = "QBluetoothAdapter::QBluetoothManagerCallback";
    return NOERROR;
}

CAR_INTERFACE_IMPL(QBluetoothAdapter, Object, IQBluetoothAdapter)

AutoPtr<QBluetoothAdapter> QBluetoothAdapter::GetDefaultAdapter()
{
    AutoLock lock(sLock);
    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    AutoPtr<IIBluetoothManager> managerService = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    sAdapter = new QBluetoothAdapter();
    sAdapter->constructor(managerService);
    return sAdapter;
}

QBluetoothAdapter::QBluetoothAdapter()
{
    CQBluetoothManagerCallback::New(this, (IIQBluetoothManagerCallback**)&mManagerCallback);
    CBluetoothManagerCallback::New(this, (IIBluetoothManagerCallback**)&mAdapterServiceCallback);
}

QBluetoothAdapter::~QBluetoothAdapter()
{
    // try {
    if (FAILED(mManagerService->UnregisterQAdapter(mManagerCallback)))
    {
        SLOGGERE("QBluetoothAdapter", "~QBluetoothAdapter RemoteException: UnregisterQAdapter")
    }
    // } catch (RemoteException e) {
        // Log.e(TAG, "", e);
    // } finally {
        // super.finalize();
    // }
}

ECode QBluetoothAdapter::constructor(
    /* [in] */ IIBluetoothManager* managerService)
{
    if (managerService == NULL) {
        SLOGGERE("QBluetoothAdapter", "bluetooth manager service is null")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    //mService = managerService.registerAdapter(mManagerCallback);
    mService = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothService(mAdapterServiceCallback);
    //mQService=managerService.getQBluetooth();
    if (FAILED(managerService->RegisterQAdapter(mManagerCallback, (IIQBluetooth**)&mQService)))
    {
        SLOGGERE("QBluetoothAdapter", "constructor RemoteException: RegisterQAdapter")
    }
    IObject* obj = IObject::Probe(mQService);
    String str("NULL");
    if (obj != NULL)
        obj->ToString(&str);
    SLOGGERI(TAG,"mQService= : %s", str.string())
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    mManagerService = managerService;
    return NOERROR;
}

/** @hide */
ECode QBluetoothAdapter::RegisterLppClient(
    /* [in] */ ILeLppCallback* client,
    /* [in] */ const String& address,
    /* [in] */ Boolean add,
    /* [out] */ Boolean* result)
{
    AutoLock lock(mMapLock);
    // {    AutoLock syncLock(mLppClients);
    if (add) {
        if(mLppClients->Find(client) != mLppClients->End()) {
            SLOGGERE(TAG, "Lpp Client has been already registered");
            *result = FALSE;
            return NOERROR;
        }

        AutoPtr<ILeLppClientWrapper> wrapper;
        CLeLppClientWrapper::New(this, mQService, address, client, (ILeLppClientWrapper**)&wrapper);
        Boolean reg;
        if(wrapper != NULL && (wrapper->Register2(TRUE, &reg), !reg)) {
            (*mLppClients)[client] = wrapper;
            *result = TRUE;
            return NOERROR;
        }
        *result = FALSE;
        return NOERROR;
    } else {
        AutoPtr<ILeLppClientWrapper> wrapper = (*mLppClients)[client];
        mLppClients->Erase(client);
        if (wrapper != NULL) {
            Boolean reg;
            wrapper->Register2(FALSE, &reg);
            *result = TRUE;
            return NOERROR;
        }
        *result = FALSE;
        return NOERROR;
    }
    // }
}

/**
 * Write the rssi threshold for a connected remote device.
 *
 * <p>The {@link BluetoothGattCallback#onWriteRssiThreshold} callback will be
 * invoked when the write request has been sent.
 *
 * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
 *
 * @param min    The lower limit of rssi threshold
 * @param max    The upper limit of rssi threshold
 * @return true, if the rssi threshold writing request has been sent tsuccessfully
 */
/** @hide */
ECode QBluetoothAdapter::WriteRssiThreshold(
    /* [in] */ ILeLppCallback* client,
    /* [in] */ Int32 min,
    /* [in] */ Int32 max,
    /* [out] */ Boolean* result)
{
    AutoPtr<ILeLppClientWrapper> wrapper;
    {
        AutoLock lock(mMapLock);
        wrapper = (*mLppClients)[client];
    }
    if (wrapper == NULL)
    *result = FALSE;
    return NOERROR;

    wrapper->WriteRssiThreshold((Byte)min, (Byte)max);
    *result = TRUE;
    return NOERROR;
}

        /**
 * Enable rssi monitoring for a connected remote device.
 *
 * <p>The {@link BluetoothGattCallback#onEnableRssiMonitor} callback will be
 * invoked when the rssi monitor enable/disable request has been sent.
 *
 * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
 *
 * @param enable disable/enable rssi monitor
 * @return true, if the rssi monitoring request has been sent tsuccessfully
 */
 /** @hide */
ECode QBluetoothAdapter::EnableRssiMonitor(
    /* [in] */ ILeLppCallback* client,
    /* [in] */ Boolean e,
    /* [out] */ Boolean* result)
{
    AutoPtr<ILeLppClientWrapper> wrapper;
    {
        AutoLock lock(mMapLock);
        wrapper = (*mLppClients)[client];
    }
    if (wrapper == NULL)
    *result = FALSE;
    return NOERROR;

    wrapper->EnableMonitor(e);
    *result = TRUE;
    return NOERROR;
}

/**
 * Read the rssi threshold for a connected remote device.
 *
 * <p>The {@link BluetoothGattCallback#onReadRssiThreshold} callback will be
 * invoked when the rssi threshold has been read.
 *
 * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
 *
 * @return true, if the rssi threshold has been requested successfully
 */
 /** @hide */
ECode QBluetoothAdapter::ReadRssiThreshold(
    /* [in] */ ILeLppCallback* client,
    /* [out] */ Boolean* result)
{
    AutoPtr<ILeLppClientWrapper> wrapper;
    {
        AutoLock lock(mMapLock);
        wrapper = (*mLppClients)[client];
    }
    if (wrapper == NULL)
    *result = FALSE;
    return NOERROR;

    wrapper->ReadRssiThreshold();
    *result = TRUE;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos