#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/server/ProfileManagerService.h"
#include "elastos/droid/server/ProfileTriggerHelper.h"
#include <elastos/core/Object.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IProfile;
using Elastos::Droid::App::IProfileTriggerState;
using Elastos::Droid::App::IProfileTriggerType;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiSsid;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IUUID;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {

const String ProfileTriggerHelper::TAG("ProfileTriggerHelper");

//-----------------------------------------------------------------------
//              ProfileTriggerHelper::MyContentObserver
//-----------------------------------------------------------------------
ProfileTriggerHelper::MyContentObserver::MyContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ ProfileTriggerHelper* host)
    : mHandler(handler)
    , mHost(host)
{}

ECode ProfileTriggerHelper::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->UpdateEnabled();
    return NOERROR;
}

//-----------------------------------------------------------------------
//              ProfileTriggerHelper
//-----------------------------------------------------------------------
ProfileTriggerHelper::ProfileTriggerHelper()
    : mFilterRegistered(FALSE)
{}

ECode ProfileTriggerHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIProfileManager* service)
{
    mContext = context;
    mService = service;

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);
    mLastConnectedSSID = GetActiveSSID();

    CIntentFilter::New((IIntentFilter**)&mIntentFilter);
    mIntentFilter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    mIntentFilter->AddAction(IBluetoothDevice::ACTION_ACL_CONNECTED);
    mIntentFilter->AddAction(IBluetoothDevice::ACTION_ACL_DISCONNECTED);
   // mIntentFilter.addAction(AudioManager.A2DP_ROUTE_CHANGED_ACTION);
    UpdateEnabled();

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    AutoPtr<IUri> uri;
    settingsSystem->GetUriFor(ISettingsSystem::SYSTEM_PROFILES_ENABLED, (IUri**)&uri);
    resolver->RegisterContentObserver(uri.Get(), FALSE, mSettingsObserver);
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    AutoPtr<MyContentObserver> sob = new MyContentObserver(IHandler::Probe(handler), this);
    mSettingsObserver = IContentObserver::Probe(sob);
    return NOERROR;
}

void ProfileTriggerHelper::UpdateEnabled()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);

    Int32 vol;
    settingsSystem->GetInt32(resolver.Get(), ISettingsSystem::SYSTEM_PROFILES_ENABLED, 1, &vol);
    Boolean enabled = vol == 1;
    if (enabled && !mFilterRegistered) {
        Logger::V(TAG, "Enabling");
        AutoPtr<IIntent> intent;
        mContext->RegisterReceiver(this, mIntentFilter.Get(), (IIntent**)&intent);
        mFilterRegistered = TRUE;
    }
    else if (!enabled && mFilterRegistered) {
        Logger::V(TAG, "Disabling");
        mContext->UnregisterReceiver(this);
        mFilterRegistered = FALSE;
    }
}

ECode ProfileTriggerHelper::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        String activeSSID = GetActiveSSID();
        Int32 triggerState;

        if (!activeSSID.IsNull()) {
            triggerState = IProfileTriggerState::ON_CONNECT;
            mLastConnectedSSID = activeSSID;
        }
        else {
            triggerState = IProfileTriggerState::ON_DISCONNECT;
        }
        CheckTriggers(IProfileTriggerType::WIFI, mLastConnectedSSID, triggerState);
    }
    else if (action.Equals(IBluetoothDevice::ACTION_ACL_CONNECTED)
            || action.Equals(IBluetoothDevice::ACTION_ACL_DISCONNECTED)) {
        Int32 triggerState = action.Equals(IBluetoothDevice::ACTION_ACL_CONNECTED)
                ? IProfileTriggerState::ON_CONNECT : IProfileTriggerState::ON_DISCONNECT;
        AutoPtr<IParcelable> obj;
        intent->GetParcelableExtra(IBluetoothDevice::EXTRA_DEVICE, (IParcelable**)&obj);
        AutoPtr<IBluetoothDevice> device = IBluetoothDevice::Probe(obj);

        String address;
        device->GetAddress(&address);
        CheckTriggers(IProfileTriggerType::BLUETOOTH, address, triggerState);
/*        } else if (action.equals(AudioManager.A2DP_ROUTE_CHANGED_ACTION)) {
        BluetoothDevice device = intent
                .getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        int state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE, 0);
        int triggerState = (state == BluetoothProfile.STATE_CONNECTED)
                ? Profile.TriggerState.ON_A2DP_CONNECT :
                Profile.TriggerState.ON_A2DP_DISCONNECT;

        checkTriggers(Profile.TriggerType.BLUETOOTH, device.getAddress(), triggerState);*/
    }
    return NOERROR;
}

void ProfileTriggerHelper::CheckTriggers(
    /* [in] */ Int32 type,
    /* [in] */ const String& id,
    /* [in] */ Int32 newState)
{
    AutoPtr<ICollection> collection = ((ProfileManagerService*)mService.Get())->GetProfileList();
    AutoPtr<IArrayList> profilelist;
    CArrayList::New(collection, (IArrayList**)&profilelist);
    Int32 size;
    profilelist->GetSize(&size);
    Boolean flag = FALSE;
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        profilelist->Get(i, (IInterface**)&obj);
        AutoPtr<IProfile> p = IProfile::Probe(obj);
        Int32 triggerState;
        p->GetTrigger(type, id, &triggerState);
        if (newState != triggerState) {
            continue;
        }
        AutoPtr<IProfile> ap;
        mService->GetActiveProfile((IProfile**)&ap);
        AutoPtr<IUUID> currentProfileUuid;
        ap->GetUuid((IUUID**)&currentProfileUuid);

        AutoPtr<IUUID> pUuid;
        p->GetUuid((IUUID**)&pUuid);

        currentProfileUuid->Equals(pUuid, &flag);
        if (!flag) {
            ((ProfileManagerService*)mService.Get())->SetActiveProfile(p, TRUE);
        }
    }
}

String ProfileTriggerHelper::GetActiveSSID()
{
    AutoPtr<IWifiInfo> wifiinfo;
    mWifiManager->GetConnectionInfo((IWifiInfo**)&wifiinfo);
    if (wifiinfo == NULL) {
        return String(NULL);
    }
    AutoPtr<IWifiSsid> ssid;
    wifiinfo->GetWifiSsid((IWifiSsid**)&ssid);
    if (ssid == NULL) {
        return String(NULL);
    }

    return Object::ToString(ssid.Get());
}

} // Server
} // Droid
} // Elastos