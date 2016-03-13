
#include "elastos/droid/systemui/statusbar/policy/HotspotControllerImpl.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/R.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String HotspotControllerImpl::TAG("HotspotController");
const Boolean HotspotControllerImpl::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);

HotspotControllerImpl::Receiver::Receiver(
    /* [in] */ HotspotControllerImpl* host)
    : mRegistered(FALSE)
    , mHost(host)
{}

ECode HotspotControllerImpl::Receiver::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening && !mRegistered) {
        if (DEBUG) Logger::D(TAG, "Registering receiver");
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        filter->AddAction(IWifiManager::WIFI_AP_STATE_CHANGED_ACTION);

        AutoPtr<IIntent> i;
        mHost->mContext->RegisterReceiver(this, filter, (IIntent**)&i);
        mRegistered = TRUE;
    }
    else if (!listening && mRegistered) {
        if (DEBUG) Logger::D(TAG, "Unregistering receiver");
        mHost->mContext->UnregisterReceiver(this);
        mRegistered = FALSE;
    }
    return NOERROR;
}

ECode HotspotControllerImpl::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (DEBUG) {
        String action;
        intent->GetAction(&action);
        Logger::D(TAG, "onReceive %s", action.string());
    }
    Boolean value = FALSE;
    mHost->FireCallback((mHost->IsHotspotEnabled(&value), value));
    return NOERROR;
}


CAR_INTERFACE_IMPL(HotspotControllerImpl, Object, IHotspotController);
HotspotControllerImpl::HotspotControllerImpl(
    /* [in] */ IContext* context)
{
    CArrayList/*<IHotspotControllerCallback>*/::New((IArrayList**)&mCallbacks);
    mReceiver = new Receiver(this);
    mContext = context;

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);

    obj = NULL;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    mConnectivityManager = IConnectivityManager::Probe(obj);
}

ECode HotspotControllerImpl::AddCallback(
    /* [in] */ IHotspotControllerCallback* callback)
{
    Boolean value = FALSE;
    if (callback == NULL || (mCallbacks->Contains(callback, &value), value)) {
        return NOERROR;
    }

    if (DEBUG) Logger::D(TAG, "addCallback %p", callback);
    mCallbacks->Add(callback);

    mCallbacks->IsEmpty(&value);
    mReceiver->SetListening(!value);
    return NOERROR;
}

ECode HotspotControllerImpl::RemoveCallback(
    /* [in] */ IHotspotControllerCallback* callback)
{
    if (callback == NULL) return NOERROR;
    if (DEBUG) Logger::D(TAG, "removeCallback %p", callback);
    mCallbacks->Remove(callback);

    Boolean value = FALSE;
    mCallbacks->IsEmpty(&value);
    mReceiver->SetListening(!value);
    return NOERROR;
}

ECode HotspotControllerImpl::IsHotspotEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 value = 0;
    mWifiManager->GetWifiApState(&value);
    *result = value == IWifiManager::WIFI_AP_STATE_ENABLED;
    return NOERROR;
}

ECode HotspotControllerImpl::IsHotspotSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);

    Int32 value = 0;
    helper->GetCurrentUser(&value);
    const Boolean isSecondaryUser = value != IUserHandle::USER_OWNER;
    Boolean tmp = FALSE;
    *result = !isSecondaryUser && (mConnectivityManager->IsTetheringSupported(&tmp), tmp);
    return NOERROR;
}

ECode HotspotControllerImpl::IsProvisioningNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Keep in sync with other usage of config_mobile_hotspot_provision_app.
    // TetherSettings#isProvisioningNeeded and
    // ConnectivityManager#enforceTetherChangePermission
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > provisionApp;
    res->GetStringArray(Elastos::Droid::R::array::config_mobile_hotspot_provision_app,
        (ArrayOf<String>**)&provisionApp);

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean value = FALSE;
    sp->GetBoolean(String("net.tethering.noprovisioning"), FALSE, &value);
    if (value || provisionApp == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    *result = (provisionApp->GetLength() == 2);
    return NOERROR;
}

ECode HotspotControllerImpl::SetHotspotEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    // This needs to be kept up to date with Settings (WifiApEnabler.setSoftapEnabled)
    // in case it is turned on in settings and off in qs (or vice versa).
    // Disable Wifi if enabling tethering.
    Int32 wifiState = 0;
    mWifiManager->GetWifiState(&wifiState);
    Boolean tmp = FALSE;
    if (enabled && ((wifiState == IWifiManager::WIFI_STATE_ENABLING) ||
                (wifiState == IWifiManager::WIFI_STATE_ENABLED))) {
        mWifiManager->SetWifiEnabled(FALSE, &tmp);
        Elastos::Droid::Provider::Settings::Global::PutInt32(cr, ISettingsGlobal::WIFI_SAVED_STATE, 1, &tmp);
    }

    mWifiManager->SetWifiApEnabled(NULL, enabled, &tmp);

    // If needed, restore Wifi on tether disable.
    if (!enabled) {
        if (Elastos::Droid::Provider::Settings::Global::GetInt32(cr, ISettingsGlobal::WIFI_SAVED_STATE, 0) == 1) {
            mWifiManager->SetWifiEnabled(TRUE, &tmp);
            Elastos::Droid::Provider::Settings::Global::PutInt32(cr, ISettingsGlobal::WIFI_SAVED_STATE, 0, &tmp);
        }
    }
    return NOERROR;
}

void HotspotControllerImpl::FireCallback(
    /* [in] */ Boolean isEnabled)
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> callback;
        ator->GetNext((IInterface**)&callback);
        IHotspotControllerCallback::Probe(callback)->OnHotspotChanged(isEnabled);
    }
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
