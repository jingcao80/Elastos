
#include "elastos/droid/settings/deviceinfo/CUsbSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/UserHandle.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

//===============================================================================
//                  CUsbSettings::InitBroadcastReceiver
//===============================================================================

CUsbSettings::InitBroadcastReceiver::InitBroadcastReceiver(
    /* [in] */ CUsbSettings* host)
    : mHost(host)
{}

ECode CUsbSettings::InitBroadcastReceiver::OnReceive(
    /* [in] */ IContext* content,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IUsbManager::ACTION_USB_STATE)) {
        intent->GetBooleanExtra(IUsbManager::USB_FUNCTION_ACCESSORY, FALSE, &mHost->mUsbAccessoryMode);
        Logger::E(TAG, "UsbAccessoryMode %d", mHost->mUsbAccessoryMode);
    }
    String str;
    mHost->mUsbManager->GetDefaultFunction(&str);
    mHost->UpdateToggles(str);
    return NOERROR;
}

//===============================================================================
//                  CUsbSettings
//===============================================================================

const String CUsbSettings::TAG("CUsbSettings");
const String CUsbSettings::KEY_MTP("usb_mtp");
const String CUsbSettings::KEY_PTP("usb_ptp");

CUsbSettings::CUsbSettings()
    : mUsbAccessoryMode(FALSE)
{}

CUsbSettings::~CUsbSettings()
{}

ECode CUsbSettings::constructor()
{
    mStateReceiver = new InitBroadcastReceiver(this);

    return SettingsPreferenceFragment::constructor();
}

AutoPtr<IPreferenceScreen> CUsbSettings::CreatePreferenceHierarchy()
{
    AutoPtr<IPreferenceScreen> root;
    GetPreferenceScreen((IPreferenceScreen**)&root);
    if (root != NULL) {
        IPreferenceGroup::Probe(root)->RemoveAll();
    }
    AddPreferencesFromResource(R::xml::usb_settings);
    root = NULL;
    GetPreferenceScreen((IPreferenceScreen**)&root);

    IPreferenceGroup* _root = IPreferenceGroup::Probe(root);
    AutoPtr<IPreference> prefMtp;
    _root->FindPreference(CoreUtils::Convert(KEY_MTP), (IPreference**)&prefMtp);
    mMtp = ICheckBoxPreference::Probe(prefMtp);
    AutoPtr<IPreference> prefPtp;
    _root->FindPreference(CoreUtils::Convert(KEY_PTP), (IPreference**)&prefPtp);
    mPtp = ICheckBoxPreference::Probe(prefPtp);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);
    Boolean res;
    if (um->HasUserRestriction(IUserManager::DISALLOW_USB_FILE_TRANSFER, &res), res) {
        prefMtp->SetEnabled(FALSE);
        prefPtp->SetEnabled(FALSE);
    }

    return root;
}

ECode CUsbSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);
    mUsbManager = IUsbManager::Probe(GetSystemService(IContext::USB_SERVICE));
    return NOERROR;
}

ECode CUsbSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->UnregisterReceiver(mStateReceiver);
    return NOERROR;
}

ECode CUsbSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();

    // Make sure we reload the preference hierarchy since some of these settings
    // depend on others...
    CreatePreferenceHierarchy();

    // ACTION_USB_STATE is sticky so this will call updateToggles
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IUsbManager::ACTION_USB_STATE, (IIntentFilter**)&filter);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    IContext::Probe(activity)->RegisterReceiver(mStateReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

void CUsbSettings::UpdateToggles(
    /* [in] */ const String& function)
{
    if (IUsbManager::USB_FUNCTION_MTP.Equals(function)) {
        ITwoStatePreference::Probe(mMtp)->SetChecked(TRUE);
        ITwoStatePreference::Probe(mPtp)->SetChecked(FALSE);
    }
    else if (IUsbManager::USB_FUNCTION_PTP.Equals(function)) {
        ITwoStatePreference::Probe(mMtp)->SetChecked(FALSE);
        ITwoStatePreference::Probe(mPtp)->SetChecked(TRUE);
    }
    else  {
        ITwoStatePreference::Probe(mMtp)->SetChecked(FALSE);
        ITwoStatePreference::Probe(mPtp)->SetChecked(FALSE);
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);
    Boolean res;
    if (um->HasUserRestriction(IUserManager::DISALLOW_USB_FILE_TRANSFER, &res), res) {
        Logger::E(TAG, "USB is locked down");
        IPreference::Probe(mMtp)->SetEnabled(FALSE);
        IPreference::Probe(mPtp)->SetEnabled(FALSE);
    }
    else if (!mUsbAccessoryMode) {
        //Enable MTP and PTP switch while USB is not in Accessory Mode, otherwise disable it
        Logger::E(TAG, "USB Normal Mode");
        IPreference::Probe(mMtp)->SetEnabled(TRUE);
        IPreference::Probe(mPtp)->SetEnabled(TRUE);
    }
    else {
        Logger::E(TAG, "USB Accessory Mode");
        IPreference::Probe(mMtp)->SetEnabled(FALSE);
        IPreference::Probe(mPtp)->SetEnabled(FALSE);
    }
}

ECode CUsbSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // Don't allow any changes to take effect as the USB host will be disconnected, killing
    // the monkeys
    if (Utils::IsMonkeyRunning()) {
        *result = TRUE;
        return NOERROR;
    }
    // If this user is disallowed from using USB, don't handle their attempts to change the
    // setting.
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);
    Boolean res;
    if (um->HasUserRestriction(IUserManager::DISALLOW_USB_FILE_TRANSFER, &res), res) {
        *result = TRUE;
        return NOERROR;
    }

    String function("none");
    if (preference == IPreference::Probe(mMtp) && (ITwoStatePreference::Probe(mMtp)->IsChecked(&res), res)) {
        function = IUsbManager::USB_FUNCTION_MTP;
    }
    else if (preference == IPreference::Probe(mPtp) && (ITwoStatePreference::Probe(mPtp)->IsChecked(&res), res)) {
        function = IUsbManager::USB_FUNCTION_PTP;
    }

    mUsbManager->SetCurrentFunction(function, TRUE);
    UpdateToggles(function);

    *result = TRUE;
    return NOERROR;
}

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos