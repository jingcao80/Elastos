
#include "elastos/droid/systemui/statusbar/policy/BluetoothControllerImpl.h"
#include "elastos/droid/systemui/statusbar/policy/BluetoothUtil.h"
#include "elastos/droid/systemui/statusbar/policy/PairedDevice.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Bluetooth::EIID_IBluetoothProfileServiceListener;
using Elastos::Droid::Bluetooth::IBluetoothManager;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CSparseBooleanArray;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

BluetoothControllerImpl::Receiver::Receiver(
    /* [in] */ BluetoothControllerImpl* host)
    : mHost(host)
{}

void BluetoothControllerImpl::Receiver::Register()
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IBluetoothAdapter::ACTION_STATE_CHANGED);
    filter->AddAction(IBluetoothAdapter::ACTION_CONNECTION_STATE_CHANGED);
    filter->AddAction(IBluetoothDevice::ACTION_BOND_STATE_CHANGED);
    filter->AddAction(IBluetoothDevice::ACTION_ALIAS_CHANGED);
    AutoPtr<IIntent> i;
    mHost->mContext->RegisterReceiver(this, filter, (IIntent**)&i);
}

ECode BluetoothControllerImpl::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    AutoPtr<IParcelable> p;
    intent->GetParcelableExtra(IBluetoothDevice::EXTRA_DEVICE, (IParcelable**)&p);
    AutoPtr<IBluetoothDevice> device = IBluetoothDevice::Probe(p);

    if (action.Equals(IBluetoothAdapter::ACTION_STATE_CHANGED)) {
        Int32 v = 0;
        intent->GetInt32Extra(IBluetoothAdapter::EXTRA_STATE, IBluetoothAdapter::ERROR, &v);
        mHost->SetAdapterState(v);
        if (DEBUG) Logger::D(TAG, "ACTION_STATE_CHANGED %d", mHost->mEnabled);
    }
    else if (action.Equals(IBluetoothAdapter::ACTION_CONNECTION_STATE_CHANGED)) {
        AutoPtr<DeviceInfo> info = mHost->UpdateInfo(device);
        Int32 state = 0;
        intent->GetInt32Extra(IBluetoothAdapter::EXTRA_CONNECTION_STATE,
                IBluetoothAdapter::ERROR, &state);
        if (state != IBluetoothAdapter::ERROR) {
            info->mConnectionState = state;
        }
        mHost->mLastDevice = device;
        if (DEBUG) Logger::D(TAG, "ACTION_CONNECTION_STATE_CHANGED %s %s"
                , BluetoothUtil::ConnectionStateToString(state).string(), BluetoothUtil::DeviceToString(device).string());
        mHost->SetConnecting(info->mConnectionState == IBluetoothAdapter::STATE_CONNECTING);
    }
    else if (action.Equals(IBluetoothDevice::ACTION_ALIAS_CHANGED)) {
        mHost->UpdateInfo(device);
        mHost->mLastDevice = device;
    }
    else if (action.Equals(IBluetoothDevice::ACTION_BOND_STATE_CHANGED)) {
        if (DEBUG) Logger::D(TAG, "ACTION_BOND_STATE_CHANGED %p", device.Get());
        // we'll update all bonded devices below
    }
    mHost->UpdateBondedBluetoothDevices();
    return NOERROR;
}

BluetoothControllerImpl::DeviceInfo::DeviceInfo()
    : mConnectionState(IBluetoothAdapter::STATE_DISCONNECTED)
    , mBonded(FALSE)
{}

CAR_INTERFACE_IMPL(BluetoothControllerImpl::ServiceListener, Object, IBluetoothProfileServiceListener);
BluetoothControllerImpl::ServiceListener::ServiceListener(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ const String& action,
    /* [in] */ Boolean connect)
    : mDevice(device)
    , mAction(action)
    , mConnect(connect)
{}

ECode BluetoothControllerImpl::ServiceListener::OnServiceConnected(
    /* [in] */ Int32 profile,
    /* [in] */ IBluetoothProfile* proxy)
{
    if (DEBUG) Logger::D(TAG, "onServiceConnected %s", BluetoothUtil::ProfileToString(profile).string());
    AutoPtr<IBluetoothUtilProfile> p = BluetoothUtil::GetProfile(proxy);
    if (p == NULL) {
        Logger::W(TAG, "Unable get get Profile for %s", BluetoothUtil::ProfileToString(profile).string());
    }
    else {
        Boolean ok = FALSE;
        if (mConnect) {
            p->Connect(mDevice, &ok);
        }
        else {
            p->Disconnect(mDevice, &ok);
        }
        if (DEBUG) Logger::D(TAG, mAction + " %s %s", BluetoothUtil::ProfileToString(profile).string()
                , (ok ? "succeeded" : "failed"));
    }
    return NOERROR;
}

ECode BluetoothControllerImpl::ServiceListener::OnServiceDisconnected(
    /* [in] */ Int32 profile)
{
    if (DEBUG) Logger::D(TAG, "onServiceDisconnected %s", BluetoothUtil::ProfileToString(profile).string());
    return NOERROR;
}

const String BluetoothControllerImpl::TAG("BluetoothController");
const Boolean BluetoothControllerImpl::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
CAR_INTERFACE_IMPL(BluetoothControllerImpl, Object, IBluetoothController);
BluetoothControllerImpl::BluetoothControllerImpl(
    /* [in] */ IContext* context)
    : mEnabled(FALSE)
    , mConnecting(FALSE)
{
    CArrayList::New((IArrayList**)&mCallbacks);
    mReceiver = new Receiver(this);
    CArrayMap::New((IArrayMap**)&mDeviceInfo);
    mContext = context;
    AutoPtr<IInterface> obj;
    Logger::D(TAG, "TODO: need BLUETOOTH_SERVICE.");
    // context->GetSystemService(IContext::BLUETOOTH_SERVICE, (IInterface**)&obj);
    if (obj != NULL) {
        AutoPtr<IBluetoothManager> bluetoothManager = IBluetoothManager::Probe(obj);
        bluetoothManager->GetAdapter((IBluetoothAdapter**)&mAdapter);
        if (mAdapter == NULL) {
            Logger::W(TAG, "Default BT adapter not found");
            assert(0);
            // return;
        }
    }

    mReceiver->Register();
    Int32 state = 0;
    if (mAdapter != NULL) {
        mAdapter->GetState(&state);
    }
    SetAdapterState(state);
    UpdateBondedBluetoothDevices();
}

ECode BluetoothControllerImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("BluetoothController state:"));
    pw->Print(String("  mAdapter="));
    pw->Println(mAdapter);
    pw->Print(String("  mEnabled="));
    pw->Println(mEnabled);
    pw->Print(String("  mConnecting="));
    pw->Println(mConnecting);
    pw->Print(String("  mLastDevice="));
    pw->Println(mLastDevice);
    pw->Print(String("  mCallbacks.size="));
    Int32 size = 0;
    mCallbacks->GetSize(&size);
    pw->Println(size);
    pw->Print(String("  mDeviceInfo.size="));
    mDeviceInfo->GetSize(&size);
    pw->Println(size);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> key;
        mDeviceInfo->GetKeyAt(i, (IInterface**)&key);
        AutoPtr<IBluetoothDevice> device = IBluetoothDevice::Probe(key);
        AutoPtr<IInterface> value;
        mDeviceInfo->GetValueAt(i, (IInterface**)&value);
        AutoPtr<DeviceInfo> info = (DeviceInfo*)IObject::Probe(value);
        pw->Print(String("    "));
        pw->Print(BluetoothUtil::DeviceToString(device));
        pw->Print('(');
        pw->Print(BluetoothUtil::UuidsToString(device));
        pw->Print(')');
        pw->Print(String("    "));
        pw->Println(InfoToString(info));
    }
    return NOERROR;
}

String BluetoothControllerImpl::InfoToString(
    /* [in] */ DeviceInfo* info)
{
    if (info == NULL) {
        return String(NULL);
    }
    String str;
    str.AppendFormat("connectionState=%s,bonded=%d"
            , BluetoothUtil::ConnectionStateToString(info->mConnectionState).string()
            , info->mBonded);
    return str;
}

ECode BluetoothControllerImpl::AddStateChangedCallback(
    /* [in] */ IBluetoothControllerCallback* cb)
{
    mCallbacks->Add(cb);
    FireStateChange(cb);
    return NOERROR;
}

ECode BluetoothControllerImpl::RemoveStateChangedCallback(
    /* [in] */ IBluetoothControllerCallback* cb)
{
    mCallbacks->Remove(cb);
    return NOERROR;
}

ECode BluetoothControllerImpl::IsBluetoothEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    Boolean tmp = FALSE;
    *enabled = mAdapter != NULL && (mAdapter->IsEnabled(&tmp), tmp);
    return NOERROR;
}

ECode BluetoothControllerImpl::IsBluetoothConnected(
    /* [out] */ Boolean* connected)
{
    VALIDATE_NOT_NULL(connected);
    Int32 v = 0;
    *connected = mAdapter != NULL
            && (mAdapter->GetConnectionState(&v), v) == IBluetoothAdapter::STATE_CONNECTED;
    return NOERROR;
}

ECode BluetoothControllerImpl::IsBluetoothConnecting(
    /* [out] */ Boolean* connecting)
{
    VALIDATE_NOT_NULL(connecting);
    Int32 v = 0;
    *connecting = mAdapter != NULL
            && (mAdapter->GetConnectionState(&v), v) == IBluetoothAdapter::STATE_CONNECTING;
    return NOERROR;
}

ECode BluetoothControllerImpl::SetBluetoothEnabled(
    /* [in] */ Boolean enabled)
{
    if (mAdapter != NULL) {
        Boolean tmp = FALSE;
        if (enabled) {
            mAdapter->Enable(&tmp);
        }
        else {
            mAdapter->Disable(&tmp);
        }
    }
    return NOERROR;
}

ECode BluetoothControllerImpl::IsBluetoothSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);
    *supported = mAdapter != NULL;
    return NOERROR;
}

ECode BluetoothControllerImpl::GetPairedDevices(
    /* [out] */ ISet** devices)
{
    VALIDATE_NOT_NULL(devices);
    AutoPtr<IArraySet> rt;  /*<PairedDevice*/
    CArraySet::New((IArraySet**)&rt);
    Int32 size = 0;
    mDeviceInfo->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> key;
        mDeviceInfo->GetKeyAt(i, (IInterface**)&key);
        AutoPtr<IBluetoothDevice> device = IBluetoothDevice::Probe(key);
        AutoPtr<IInterface> value;
        mDeviceInfo->GetValueAt(i, (IInterface**)&value);
        AutoPtr<DeviceInfo> info = (DeviceInfo*)IObject::Probe(value);

        if (!info->mBonded) continue;
        AutoPtr<IBluetoothControllerPairedDevice> paired = new PairedDevice();
        String str;
        device->GetAddress(&str);
        paired->SetId(str);
        paired->SetTag(device);
        device->GetAliasName(&str);
        paired->SetName(str);
        paired->SetState(ConnectionStateToPairedDeviceState(info->mConnectionState));
        ISet::Probe(rt)->Add(paired);
    }
    *devices = ISet::Probe(rt);
    REFCOUNT_ADD(*devices);
    return NOERROR;
}

Int32 BluetoothControllerImpl::ConnectionStateToPairedDeviceState(
    /* [in] */ Int32 state)
{
    if (state == IBluetoothAdapter::STATE_CONNECTED) return IBluetoothControllerPairedDevice::STATE_CONNECTED;
    if (state == IBluetoothAdapter::STATE_CONNECTING) return IBluetoothControllerPairedDevice::STATE_CONNECTING;
    if (state == IBluetoothAdapter::STATE_DISCONNECTING) return IBluetoothControllerPairedDevice::STATE_DISCONNECTING;
    return IBluetoothControllerPairedDevice::STATE_DISCONNECTED;
}

ECode BluetoothControllerImpl::Connect(
    /* [in] */ IBluetoothControllerPairedDevice* pd)
{
    Connect(pd, TRUE);
    return NOERROR;
}

ECode BluetoothControllerImpl::Disconnect(
    /* [in] */ IBluetoothControllerPairedDevice* pd)
{
    Connect(pd, FALSE);
    return NOERROR;
}

void BluetoothControllerImpl::Connect(
    /* [in] */ IBluetoothControllerPairedDevice* pd,
    /* [in] */ Boolean connect)
{
    AutoPtr<IInterface> tag;
    pd->GetTag((IInterface**)&tag);
    if (mAdapter == NULL || pd == NULL || tag == NULL) return;
    AutoPtr<IInterface> o;
    pd->GetTag((IInterface**)&o);
    AutoPtr<IBluetoothDevice> device = IBluetoothDevice::Probe(o);
    String action = connect ? String("connect") : String("disconnect");
    if (DEBUG) Logger::D(TAG, action + " %s", BluetoothUtil::DeviceToString(device).string());
    AutoPtr<ArrayOf<IParcelUuid*> > uuids;
    device->GetUuids((ArrayOf<IParcelUuid*>**)&uuids);
    if (uuids == NULL) {
        Logger::W(TAG, "No uuids returned, aborting %s for %s", action.string()
            , BluetoothUtil::DeviceToString(device).string());
        return;
    }
    AutoPtr<ISparseBooleanArray> profiles;
    CSparseBooleanArray::New((ISparseBooleanArray**)&profiles);
    for (Int32 i = 0; i < uuids->GetLength(); i++) {
        AutoPtr<IParcelUuid> uuid = (*uuids)[i];
        Int32 profile = BluetoothUtil::UuidToProfile(uuid);
        if (profile == 0) {
            Logger::W(TAG, "Device %s has an unsupported uuid: %s", BluetoothUtil::DeviceToString(device).string()
                    , BluetoothUtil::UuidToString(uuid).string());
            continue;
        }
        Int32 profileState = 0;
        mAdapter->GetProfileConnectionState(profile, &profileState);
        Boolean t = FALSE;
        if (DEBUG && (profiles->Get(profile, &t), !t)) {
            Logger::D(TAG, "Profile %s state = %s", BluetoothUtil::ProfileToString(profile).string()
                , BluetoothUtil::ProfileStateToString(profileState).string());
        }
        Boolean connected = profileState == IBluetoothProfile::STATE_CONNECTED;
        if (connect != connected) {
            profiles->Put(profile, TRUE);
        }
    }
    Int32 size = 0;
    profiles->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        Int32 profile = 0;
        profiles->KeyAt(i, &profile);
        AutoPtr<ServiceListener> sl = new ServiceListener(device, action, connect);
        Boolean t = FALSE;
        mAdapter->GetProfileProxy(mContext, sl, profile, &t);
    }
}

ECode BluetoothControllerImpl::GetLastDeviceName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = String(NULL);
    if (mLastDevice != NULL) {
        mLastDevice->GetAliasName(name);
    }
    return NOERROR;
}

void BluetoothControllerImpl::UpdateBondedBluetoothDevices()
{
    if (mAdapter == NULL) return;
    AutoPtr<ArrayOf<IBluetoothDevice*> > bondedDevices;
    mAdapter->GetBondedDevices((ArrayOf<IBluetoothDevice*>**)&bondedDevices);
    AutoPtr<ICollection> collection;
    mDeviceInfo->GetValues((ICollection**)&collection);
    AutoPtr<IIterator> ator;
    collection->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<DeviceInfo> info = (DeviceInfo*)IObject::Probe(obj);
        info->mBonded = FALSE;
    }

    Int32 bondedCount = 0;
    AutoPtr<IBluetoothDevice> lastBonded;
    if (bondedDevices != NULL) {
        for (Int32 i = 0; i < bondedDevices->GetLength(); i++) {
            AutoPtr<IBluetoothDevice> bondedDevice = (*bondedDevices)[i];
            Int32 s = 0;
            Boolean bonded = (bondedDevice->GetBondState(&s), s) != IBluetoothDevice::BOND_NONE;
            AutoPtr<DeviceInfo> info = UpdateInfo(bondedDevice);
            info->mBonded = bonded;
            if (bonded) {
                bondedCount++;
                lastBonded = bondedDevice;
            }
        }
    }
    if (mLastDevice == NULL && bondedCount == 1) {
        mLastDevice = lastBonded;
    }
    FirePairedDevicesChanged();
}

void BluetoothControllerImpl::FirePairedDevicesChanged()
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IBluetoothControllerCallback> cb = IBluetoothControllerCallback::Probe(obj);
        cb->OnBluetoothPairedDevicesChanged();
    }
}

void BluetoothControllerImpl::SetAdapterState(
    /* [in] */ Int32 adapterState)
{
    Boolean enabled = adapterState == IBluetoothAdapter::STATE_ON;
    if (mEnabled == enabled) return;
    mEnabled = enabled;
    FireStateChange();
}

void BluetoothControllerImpl::SetConnecting(
    /* [in] */ Boolean connecting)
{
    if (mConnecting == connecting) return;
    mConnecting = connecting;
    FireStateChange();
}

void BluetoothControllerImpl::FireStateChange()
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IBluetoothControllerCallback> cb = IBluetoothControllerCallback::Probe(obj);
        FireStateChange(cb);
    }
}

void BluetoothControllerImpl::FireStateChange(
    /* [in] */ IBluetoothControllerCallback* cb)
{
    cb->OnBluetoothStateChange(mEnabled, mConnecting);
}

AutoPtr<BluetoothControllerImpl::DeviceInfo> BluetoothControllerImpl::UpdateInfo(
    /* [in] */ IBluetoothDevice* device)
{
    AutoPtr<IInterface> obj;
    mDeviceInfo->Get(device, (IInterface**)&obj);
    AutoPtr<DeviceInfo> info = (DeviceInfo*)IObject::Probe(obj);
    if (info == NULL) {
        info = new DeviceInfo();
    }
    mDeviceInfo->Put(device, (IObject*)info->Probe(EIID_IObject));
    return info;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
