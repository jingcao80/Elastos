
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/settings/CCryptKeeperSettings.h"
#include "elastos/droid/settings/ChooseLockSettingsHelper.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "../R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Settings {

const String CCryptKeeperSettings::TAG("CCryptKeeperSettings");

const Int32 CCryptKeeperSettings::KEYGUARD_REQUEST = 55;

const Int32 CCryptKeeperSettings::MIN_BATTERY_LEVEL = 80;

//===============================================================================
//                 CCryptKeeperSettings::InitBroadcastReceiver
//===============================================================================

CCryptKeeperSettings::InitBroadcastReceiver::InitBroadcastReceiver(
    /* [in] */ CCryptKeeperSettings* host)
    : mHost(host)
{}

CCryptKeeperSettings::InitBroadcastReceiver::~InitBroadcastReceiver()
{}

ECode CCryptKeeperSettings::InitBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_BATTERY_CHANGED)) {
        Int32 level;
        intent->GetInt32Extra(IBatteryManager::EXTRA_LEVEL, 0, &level);
        Int32 plugged;
        intent->GetInt32Extra(IBatteryManager::EXTRA_PLUGGED, 0, &plugged);
        Int32 invalidCharger;
        intent->GetInt32Extra(
                IBatteryManager::EXTRA_INVALID_CHARGER, 0, &invalidCharger);

        const Boolean levelOk = level >= MIN_BATTERY_LEVEL;
        const Boolean pluggedOk =
            ((plugged & IBatteryManager::BATTERY_PLUGGED_ANY) != 0) && invalidCharger == 0;

        // Update UI elements based on power/battery status
        IView::Probe(mHost->mInitiateButton)->SetEnabled(levelOk && pluggedOk);
        mHost->mPowerWarning->SetVisibility(pluggedOk ? IView::GONE : IView::VISIBLE);
        mHost->mBatteryWarning->SetVisibility(levelOk ? IView::GONE : IView::VISIBLE);
    }
    return NOERROR;
}

//===============================================================================
//                 CCryptKeeperSettings::InitButtonOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CCryptKeeperSettings::InitButtonOnClickListener, Object, IViewOnClickListener)

CCryptKeeperSettings::InitButtonOnClickListener::InitButtonOnClickListener(
    /* [in] */ CCryptKeeperSettings* host)
    : mHost(host)
{}

CCryptKeeperSettings::InitButtonOnClickListener::~InitButtonOnClickListener()
{}

ECode CCryptKeeperSettings::InitButtonOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (!mHost->RunKeyguardConfirmation(KEYGUARD_REQUEST)) {
        // TODO replace (or follow) this dialog with an explicit launch into password UI
        AutoPtr<IActivity> activity;
        mHost->GetActivity((IActivity**)&activity);
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
        builder->SetTitle(R::string::crypt_keeper_dialog_need_password_title);
        builder->SetMessage(R::string::crypt_keeper_dialog_need_password_message);
        builder->SetPositiveButton(Elastos::Droid::R::string::ok, NULL);
        AutoPtr<IAlertDialog> dialog1, dialog2;
        builder->Create((IAlertDialog**)&dialog1);
        builder->Show((IAlertDialog**)&dialog2);
    }
    return NOERROR;
}

//===============================================================================
//                 CCryptKeeperSettings
//===============================================================================

CAR_OBJECT_IMPL(CCryptKeeperSettings)

CCryptKeeperSettings::CCryptKeeperSettings()
{
    mIntentReceiver = new InitBroadcastReceiver(this);
    mInitiateListener = new InitButtonOnClickListener(this);
}

CCryptKeeperSettings::~CCryptKeeperSettings()
{}

ECode CCryptKeeperSettings::constructor()
{
    return Fragment::constructor();
}

ECode CCryptKeeperSettings::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    inflater->Inflate(R::layout::crypt_keeper_settings, NULL, (IView**)&mContentView);

    CIntentFilter::New((IIntentFilter**)&mIntentFilter);
    mIntentFilter->AddAction(IIntent::ACTION_BATTERY_CHANGED);

    AutoPtr<IView> tmp;
    mContentView->FindViewById(R::id::initiate_encrypt, (IView**)&tmp);
    mInitiateButton = IButton::Probe(tmp);
    tmp->SetOnClickListener(mInitiateListener);
    tmp->SetEnabled(FALSE);

    mContentView->FindViewById(R::id::warning_unplugged, (IView**)&mPowerWarning);
    mContentView->FindViewById(R::id::warning_low_charge, (IView**)&mBatteryWarning);

    *result = mContentView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CCryptKeeperSettings::OnResume()
{
    Fragment::OnResume();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> stickyIntent;
    return IContext::Probe(activity)->RegisterReceiver(mIntentReceiver, mIntentFilter, (IIntent**)&stickyIntent);
}

ECode CCryptKeeperSettings::OnPause()
{
    Fragment::OnPause();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    return IContext::Probe(activity)->UnregisterReceiver(mIntentReceiver);
}

ECode CCryptKeeperSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnActivityCreated(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    if (IDevicePolicyManager::ACTION_START_ENCRYPTION.Equals(action)) {
        AutoPtr<IInterface> obj;
        IContext::Probe(activity)->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
        IDevicePolicyManager* dpm = IDevicePolicyManager::Probe(obj);
        if (dpm != NULL) {
            Int32 status;
            dpm->GetStorageEncryptionStatus(&status);
            if (status != IDevicePolicyManager::ENCRYPTION_STATUS_INACTIVE) {
                // There is nothing to do here, so simply Finish() (which returns to caller)
                activity->Finish();
            }
        }
    }
    return NOERROR;
}

Boolean CCryptKeeperSettings::RunKeyguardConfirmation(
    /* [in] */ Int32 request)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IResources> res;
    IContext::Probe(activity)->GetResources((IResources**)&res);
    AutoPtr<ChooseLockSettingsHelper> helper = new ChooseLockSettingsHelper();
    helper->constructor(activity, this);

    Int32 quality;
    if ((helper->Utils()->GetKeyguardStoredPasswordQuality(&quality), quality)
            == IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) {
        ShowFinalConfirmation(IStorageManager::CRYPT_TYPE_DEFAULT, String(""));
        return TRUE;
    }

    AutoPtr<ICharSequence> cs1, cs2;
    res->GetText(R::string::master_clear_gesture_prompt, (ICharSequence**)&cs1);
    res->GetText(R::string::crypt_keeper_confirm_encrypt, (ICharSequence**)&cs2);
    return helper->LaunchConfirmationActivity(request, cs1, cs2, TRUE);
}

ECode CCryptKeeperSettings::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    Fragment::OnActivityResult(requestCode, resultCode, data);

    if (requestCode != KEYGUARD_REQUEST) {
        return NOERROR;
    }

    // If the user entered a valid keyguard trace, present the final
    // confirmation prompt; otherwise, go back to the initial state.
    if (resultCode == IActivity::RESULT_OK && data != NULL) {
        Int32 type;
        data->GetInt32Extra(ChooseLockSettingsHelper::EXTRA_KEY_TYPE, -1, &type);
        String password;
        data->GetStringExtra(ChooseLockSettingsHelper::EXTRA_KEY_PASSWORD, &password);
        if (!TextUtils::IsEmpty(password)) {
            ShowFinalConfirmation(type, password);
        }
    }
    return NOERROR;
}

void CCryptKeeperSettings::ShowFinalConfirmation(
    /* [in] */ Int32 type,
    /* [in] */ const String& password)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPreference> preference;
    CPreference::New(IContext::Probe(activity), (IPreference**)&preference);
    preference->SetFragment(String("Elastos.Droid.Settings.CCryptKeeperConfirm"));
    preference->SetTitle(R::string::crypt_keeper_confirm_title);
    AutoPtr<IBundle> extras;
    preference->GetExtras((IBundle**)&extras);
    extras->PutInt32(String("type"), type);
    extras->PutString(String("password"), password);
    Boolean res;
    ((CSettingsActivity*)activity.Get())->OnPreferenceStartFragment(NULL, preference, &res);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos