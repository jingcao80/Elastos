
#include "elastos/droid/phone/CCallFeaturesSetting.h"
#include "elastos/droid/internal/telephony/CallForwardInfo.h"
#include "R.h"
#include <elastos/droid/R.h>
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Telecomm.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Internal::Telephony::CallForwardInfo;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IDialogPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::IContactsPhones;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Telecomm::Telecom::CTelecomManagerHelper;
using Elastos::Droid::Telecomm::Telecom::ITelecomManagerHelper;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Core::CString;
using Elastos::Core::CThread;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {

const Int32 CCallFeaturesSetting::EVENT_VOICEMAIL_CHANGED        = 500;
const Int32 CCallFeaturesSetting::EVENT_FORWARDING_CHANGED       = 501;
const Int32 CCallFeaturesSetting::EVENT_FORWARDING_GET_COMPLETED = 502;
const Int32 CCallFeaturesSetting::MSG_UPDATE_VOICEMAIL_RINGTONE_SUMMARY = 1;
const Int32 CCallFeaturesSetting::VOICEMAIL_PREF_ID = 1;
const Int32 CCallFeaturesSetting::VOICEMAIL_PROVIDER_CFG_ID = 2;
const Int32 CCallFeaturesSetting::VM_NOCHANGE_ERROR = 400;
const Int32 CCallFeaturesSetting::VM_RESPONSE_ERROR = 500;
const Int32 CCallFeaturesSetting::FW_SET_RESPONSE_ERROR = 501;
const Int32 CCallFeaturesSetting::FW_GET_RESPONSE_ERROR = 502;
const Int32 CCallFeaturesSetting::VOICEMAIL_DIALOG_CONFIRM = 600;
const Int32 CCallFeaturesSetting::VOICEMAIL_FWD_SAVING_DIALOG = 601;
const Int32 CCallFeaturesSetting::VOICEMAIL_FWD_READING_DIALOG = 602;
const Int32 CCallFeaturesSetting::VOICEMAIL_REVERTING_DIALOG = 603;
const Int32 CCallFeaturesSetting::MSG_OK = 100;
const Int32 CCallFeaturesSetting::MSG_VM_EXCEPTION = 400;
const Int32 CCallFeaturesSetting::MSG_FW_SET_EXCEPTION = 401;
const Int32 CCallFeaturesSetting::MSG_FW_GET_EXCEPTION = 402;
const Int32 CCallFeaturesSetting::MSG_VM_OK = 600;
const Int32 CCallFeaturesSetting::MSG_VM_NOCHANGE = 700;

CCallFeaturesSetting::VoiceMailProviderSettings::VoiceMailProviderSettings(
    /* [in] */ const String& voicemailNumber,
    /* [in] */ const String& forwardingNumber,
    /* [in] */ Int32 timeSeconds)
    : mVoicemailNumber(voicemailNumber)
{
    if (forwardingNumber.IsNull() || forwardingNumber.GetLength() == 0) {
        mForwardingSettings = FWD_SETTINGS_DONT_TOUCH;
    }
    else {
        mForwardingSettings = ArrayOf<ICallForwardInfo*>::Alloc(FORWARDING_SETTINGS_REASONS->GetLength());
        for (Int32 i = 0; i < mForwardingSettings->GetLength(); i++) {
            AutoPtr<CallForwardInfo> fi = new CallForwardInfo();
            mForwardingSettings->Set(i, fi);
            fi->mReason = (*FORWARDING_SETTINGS_REASONS)[i];
            fi->mStatus = (fi->mReason == ICommandsInterface::CF_REASON_UNCONDITIONAL) ? 0 : 1;
            fi->mServiceClass = ICommandsInterface::SERVICE_CLASS_VOICE;
            fi->mToa = IPhoneNumberUtils::TOA_International;
            fi->mNumber = forwardingNumber;
            fi->mTimeSeconds = timeSeconds;
        }
    }
}

CAR_INTERFACE_IMPL(CCallFeaturesSetting::VoiceMailProviderSettings, Object, IVoiceMailProviderSettings)
CCallFeaturesSetting::VoiceMailProviderSettings::VoiceMailProviderSettings(
    /* [in] */ const String& voicemailNumber,
    /* [in] */ ArrayOf<ICallForwardInfo*>* infos)
    : mVoicemailNumber(voicemailNumber)
    , mForwardingSettings(infos)
{
}

ECode CCallFeaturesSetting::VoiceMailProviderSettings::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (o == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (IVoiceMailProviderSettings::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    VoiceMailProviderSettings* v = (VoiceMailProviderSettings*)IVoiceMailProviderSettings::Probe(o);

    return ((mVoicemailNumber.IsNull() && v->mVoicemailNumber.IsNull()) ||
            (!mVoicemailNumber.IsNull() && mVoicemailNumber.Equals(v->mVoicemailNumber)))
            && ForwardingSettingsEqual(mForwardingSettings, v->mForwardingSettings);
}

Boolean CCallFeaturesSetting::VoiceMailProviderSettings::ForwardingSettingsEqual(
    /* [in] */ ArrayOf<ICallForwardInfo*>* infos1,
    /* [in] */ ArrayOf<ICallForwardInfo*>* infos2)
{
    if (infos1 == infos2) return TRUE;
    if (infos1 == NULL || infos2 == NULL) return FALSE;
    if (infos1->GetLength() != infos2->GetLength()) return FALSE;
    for (Int32 i = 0; i < infos1->GetLength(); i++) {
        CallForwardInfo* i1 = (CallForwardInfo*)(*infos1)[i];
        CallForwardInfo* i2 = (CallForwardInfo*)(*infos2)[i];
        if (i1->mStatus != i2->mStatus ||
            i1->mReason != i2->mReason ||
            i1->mServiceClass != i2->mServiceClass ||
            i1->mToa != i2->mToa ||
            i1->mNumber != i2->mNumber ||
            i1->mTimeSeconds != i2->mTimeSeconds) {
            return FALSE;
        }
    }
    return TRUE;
}

ECode CCallFeaturesSetting::VoiceMailProviderSettings::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb;
    sb += mVoicemailNumber;

    assert(0);
    // return voicemailNumber + ((forwardingSettings != NULL ) ? (", " +
    //         forwardingSettings.toString()) : "");
    return NOERROR;
}

CCallFeaturesSetting::MyHandler::MyHandler(
    /* [in] */ CCallFeaturesSetting* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CCallFeaturesSetting::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_UPDATE_VOICEMAIL_RINGTONE_SUMMARY:
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
            mHost->mVoicemailNotificationRingtone->SetSummary(cchar);
            break;
    }
    return NOERROR;
}

CCallFeaturesSetting::MyHandler2::MyHandler2(
    /* [in] */ CCallFeaturesSetting* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CCallFeaturesSetting::MyHandler2::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* result = (AsyncResult*)IObject::Probe(obj);

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_FORWARDING_GET_COMPLETED: {
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            mHost->HandleForwardingSettingsReadResult(result, arg1);
            break;
        }
    }
    return NOERROR;
}

CCallFeaturesSetting::MyHandler3::MyHandler3(
    /* [in] */ CCallFeaturesSetting* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CCallFeaturesSetting::MyHandler3::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* result = (AsyncResult*)IObject::Probe(obj);
    Boolean done = FALSE;

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_VOICEMAIL_CHANGED:
            mHost->mVoicemailChangeResult = result;
            mHost->mVMChangeCompletedSuccessfully = mHost->CheckVMChangeSuccess() == NULL;
            if (DBG) {
                Log(String("VM change complete msg, VM change done = ") +
                    StringUtils::ToString(mHost->mVMChangeCompletedSuccessfully));
            }
            done = TRUE;
            break;
        case EVENT_FORWARDING_CHANGED: {
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            (*mHost->mForwardingChangeResults)[arg1] = result;
            if (result->mException != NULL) {
                Logger::W(TAG, "Error in setting fwd# %d: ", arg1 /*result->mException.getMessage()*/);
            }
            else {
                if (DBG) Log(String("Success in setting fwd# ") + StringUtils::ToString(arg1));
            }
            const Boolean completed = mHost->CheckForwardingCompleted();
            if (completed) {
                if (mHost->CheckFwdChangeSuccess() == NULL) {
                    if (DBG) Log(String("Overall fwd changes completed ok, starting vm change"));
                    mHost->SetVMNumberWithCarrier();
                }
                else {
                    Logger::W(TAG, "Overall fwd changes completed in failure. "
                            "Check if we need to try rollback for some settings.");
                    mHost->mFwdChangesRequireRollback = FALSE;
                    HashMap<Int32, AutoPtr<AsyncResult> >::Iterator ator = mHost->mForwardingChangeResults->Begin();
                    for (; ator != mHost->mForwardingChangeResults->End(); ++ator) {
                        AsyncResult* v = ator->mSecond;
                        if (v->mException == NULL) {
                            // If at least one succeeded we have to revert
                            Logger::I(TAG, "Rollback will be required");
                            mHost->mFwdChangesRequireRollback = TRUE;
                            break;
                        }
                    }
                    if (!mHost->mFwdChangesRequireRollback) {
                        Logger::I(TAG, "No rollback needed.");
                    }
                    done = TRUE;
                }
            }
            break;
        }
        default: {
            // TODO: should never reach this, may want to throw exception
        }
    }
    if (done) {
        if (DBG) Log(String("All VM provider related changes done"));
        if (mHost->mForwardingChangeResults != NULL) {
            mHost->DismissDialogSafely(VOICEMAIL_FWD_SAVING_DIALOG);
        }
        mHost->HandleSetVMOrFwdMessage();
    }
    return NOERROR;
}

CCallFeaturesSetting::MyHandler4::MyHandler4(
    /* [in] */ CCallFeaturesSetting* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CCallFeaturesSetting::MyHandler4::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* result = (AsyncResult*)IObject::Probe(obj);

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_VOICEMAIL_CHANGED:
            mHost->mVoicemailChangeResult = result;
            if (DBG) Log(String("VM revert complete msg"));
            break;
        case EVENT_FORWARDING_CHANGED: {
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            (*mHost->mForwardingChangeResults)[arg1] = result;
            if (result->mException != NULL) {
                if (DBG) {
                    Log(String("Error in reverting fwd# ") + StringUtils::ToString(arg1)
                        /*,result.exception.getMessage()*/);
                }
            }
            else {
                if (DBG) Log(String("Success in reverting fwd# ") + StringUtils::ToString(arg1));
            }
            if (DBG) Log(String("FWD revert complete msg "));
            break;
        }
        default: {
            // TODO: should never reach this, may want to throw exception
        }
    }
    const Boolean done =
        (!mHost->mVMChangeCompletedSuccessfully || mHost->mVoicemailChangeResult != NULL) &&
        (!mHost->mFwdChangesRequireRollback || mHost->CheckForwardingCompleted());
    if (done) {
        if (DBG) mHost->Log(String("All VM reverts done"));
        mHost->DismissDialogSafely(VOICEMAIL_REVERTING_DIALOG);
        mHost->OnRevertDone();
    }
    return NOERROR;
}

ECode CCallFeaturesSetting::MyRunnable::Run()
{
    if (mHost->mVoicemailNotificationRingtone != NULL) {
        AutoPtr<IContext> context;
        mHost->mPhone->GetContext((IContext**)&context);
        assert(0 && "TODO Need SettingsUtil");
        // SettingsUtil::UpdateRingtoneName(context,
        //         mVoicemailRingtoneLookupComplete,
        //         RingtoneManager.TYPE_NOTIFICATION,
        //         mHost->mVoicemailNotificationRingtone,
        //         MSG_UPDATE_VOICEMAIL_RINGTONE_SUMMARY);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CCallFeaturesSetting::PreferenceOnPreferenceChangeListener, Object, \
        IPreferenceOnPreferenceChangeListener)
CCallFeaturesSetting::PreferenceOnPreferenceChangeListener::PreferenceOnPreferenceChangeListener(
    /* [in] */ CCallFeaturesSetting* host)
    : mHost(host)
{}

ECode CCallFeaturesSetting::PreferenceOnPreferenceChangeListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* objValue,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceChange(preference, objValue, result);
}

AutoPtr<ArrayOf<ICallForwardInfo*> > CCallFeaturesSetting::FWD_SETTINGS_DONT_TOUCH;

const String CCallFeaturesSetting::TAG("CallFeaturesSetting");
const Boolean CCallFeaturesSetting::DBG = IPhoneGlobals::DBG_LEVEL >= 2;

const String CCallFeaturesSetting::UP_ACTIVITY_PACKAGE("com.android.dialer");
const String CCallFeaturesSetting::UP_ACTIVITY_CLASS("com.android.dialer.DialtactsActivity");

static AutoPtr<ArrayOf<String> > initNUM_PROJECTION()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);

    array->Set(0, IContactsContractCommonDataKindsPhone::NUMBER);
    return array;
}

const AutoPtr<ArrayOf<String> > CCallFeaturesSetting::NUM_PROJECTION = initNUM_PROJECTION();

const String CCallFeaturesSetting::VOICEMAIL_SETTING_SCREEN_PREF_KEY("button_voicemail_category_key");
const String CCallFeaturesSetting::BUTTON_VOICEMAIL_KEY("button_voicemail_key");
const String CCallFeaturesSetting::BUTTON_VOICEMAIL_PROVIDER_KEY("button_voicemail_provider_key");
const String CCallFeaturesSetting::BUTTON_VOICEMAIL_SETTING_KEY("button_voicemail_setting_key");

const String CCallFeaturesSetting::BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_KEY("button_voicemail_notification_vibrate_key");

const String CCallFeaturesSetting::BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_WHEN_KEY("button_voicemail_notification_vibrate_when_key");
const String CCallFeaturesSetting::BUTTON_VOICEMAIL_NOTIFICATION_RINGTONE_KEY("button_voicemail_notification_ringtone_key");
const String CCallFeaturesSetting::BUTTON_FDN_KEY("button_fdn_key");

const String CCallFeaturesSetting::BUTTON_DTMF_KEY("button_dtmf_settings");
const String CCallFeaturesSetting::BUTTON_RETRY_KEY("button_auto_retry_key");
const String CCallFeaturesSetting::BUTTON_TTY_KEY("button_tty_mode_key");
const String CCallFeaturesSetting::BUTTON_HAC_KEY("button_hac_key");

const String CCallFeaturesSetting::BUTTON_GSM_UMTS_OPTIONS("button_gsm_more_expand_key");
const String CCallFeaturesSetting::BUTTON_CDMA_OPTIONS("button_cdma_more_expand_key");

const String CCallFeaturesSetting::VM_NUMBERS_SHARED_PREFERENCES_NAME("vm_numbers");

const String CCallFeaturesSetting::DEFAULT_OUTGOING_ACCOUNT_KEY("default_outgoing_account");
const String CCallFeaturesSetting::PHONE_ACCOUNT_SETTINGS_KEY("phone_account_settings_preference_screen");
const String CCallFeaturesSetting::VOICEMAIL_VIBRATION_ALWAYS("always");
const String CCallFeaturesSetting::VOICEMAIL_VIBRATION_NEVER("never");

static AutoPtr<ArrayOf<Int32> > initFORWARDING_SETTINGS_REASONS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);

    (*array)[0] = ICommandsInterface::CF_REASON_UNCONDITIONAL;
    (*array)[1] = ICommandsInterface::CF_REASON_BUSY;
    (*array)[2] = ICommandsInterface::CF_REASON_NO_REPLY;
    (*array)[3] = ICommandsInterface::CF_REASON_NOT_REACHABLE;

    return array;
}

AutoPtr<ArrayOf<Int32> > CCallFeaturesSetting::FORWARDING_SETTINGS_REASONS = initFORWARDING_SETTINGS_REASONS();

CAR_INTERFACE_IMPL_5(CCallFeaturesSetting, PreferenceActivity, ICallFeaturesSetting,
        IDialogInterfaceOnClickListener, IPreferenceOnPreferenceChangeListener,
        IEditPhoneNumberPreferenceOnDialogClosedListener,
        IEditPhoneNumberPreferenceGetDefaultNumberListener)

CAR_OBJECT_IMPL(CCallFeaturesSetting)

CCallFeaturesSetting::CCallFeaturesSetting()
    : mPreviousVMProviderKey(NULL)
    , mCurrentDialogId(0)
    , mVMProviderSettingsForced(FALSE)
    , mChangingVMorFwdDueToProviderChange(FALSE)
    , mVMChangeCompletedSuccessfully(FALSE)
    , mFwdChangesRequireRollback(FALSE)
    , mVMOrFwdSetError(0)
    , mOldVmNumber(NULL)
    , mNewVMNumber(NULL)
    , mForeground(FALSE)
    , mReadingSettingsForDefaultProvider(FALSE)
    , mShowVoicemailPreference(FALSE)
{
    mVoicemailRingtoneLookupComplete = new MyHandler(this);

    mGetOptionComplete = new MyHandler2(this);

    mSetOptionComplete = new MyHandler3(this);

    mRevertOptionComplete = new MyHandler4(this);
}

ECode CCallFeaturesSetting::constructor()
{
    return PreferenceActivity::constructor();
}

ECode CCallFeaturesSetting::OnPause()
{
    PreferenceActivity::OnPause();
    mForeground = FALSE;
    return NOERROR;
}

ECode CCallFeaturesSetting::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mSubMenuVoicemailSettings)) {
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonDTMF)) {
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonTTY)) {
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonAutoRetry)) {
        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Boolean res;
        ITwoStatePreference::Probe(mButtonAutoRetry)->IsChecked(&res);
        helper->PutInt32(cr, ISettingsGlobal::CALL_AUTO_RETRY, res ?  1 : 0, &res);

        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonHAC)) {
        Boolean res;
        Int32 hac = (ITwoStatePreference::Probe(mButtonHAC)->IsChecked(&res), res) ? 1 : 0;

        // Update HAC value in Settings database
        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        ITwoStatePreference::Probe(mButtonAutoRetry)->IsChecked(&res);
        helper->PutInt32(cr, ISettingsSystem::HEARING_AID, hac, &res);

        // Update HAC Value in AudioManager
        mAudioManager->SetParameter(HAC_KEY, hac != 0 ? HAC_VAL_ON : HAC_VAL_OFF);
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mVoicemailSettings)) {
        AutoPtr<IDialog> dialog;
        mVoicemailSettings->GetDialog((IDialog**)&dialog);
        if (dialog != NULL) {
            AutoPtr<IActionBar> actionBar;
            dialog->GetActionBar((IActionBar**)&actionBar);
            actionBar->SetDisplayHomeAsUpEnabled(FALSE);
        }
        if (DBG) Log(String("onPreferenceTreeClick: Voicemail Settings Preference is clicked."));
        AutoPtr<IIntent> intent;
        preference->GetIntent((IIntent**)&intent);
        if (intent != NULL) {
            if (DBG) {
                String package;
                intent->GetPackage(&package);
                Log(String("onPreferenceTreeClick: Invoking cfg intent ") + package);
            }

            // onActivityResult() will be responsible for resetting some of variables.
            StartActivityForResult(intent, VOICEMAIL_PROVIDER_CFG_ID);
            *result = TRUE;
            return NOERROR;
        }
        else {
            if (DBG) {
                Log(String("onPreferenceTreeClick:")
                        + String(" No Intent is available. Use default behavior defined in xml."));
            }

            // There's no onActivityResult(), so we need to take care of some of variables
            // which should be reset here.
            mPreviousVMProviderKey = DEFAULT_VM_PROVIDER_KEY;
            mVMProviderSettingsForced = FALSE;

            // This should let the preference use default behavior in the xml.
            *result = FALSE;
            return NOERROR;
        }
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mVoicemailSettingsScreen)) {
        AutoPtr<IDialog> dialog;
        mVoicemailSettingsScreen->GetDialog((IDialog**)&dialog);
        if (dialog != NULL) {
            AutoPtr<IActionBar> actionBar;
            dialog->GetActionBar((IActionBar**)&actionBar);
            actionBar->SetDisplayHomeAsUpEnabled(FALSE);
        }
        *result = FALSE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CCallFeaturesSetting::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* objValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) {
        StringBuilder sb;
        sb += "onPreferenceChange(). preferenece: \"";
        sb += preference;
        sb += "\"";
        sb += ", value: \"";
        sb += objValue;
        sb += "\"";
        Log(sb.ToString());
    }

    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonDTMF)) {
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(objValue);
        String str;
        cchar->ToString(&str);
        Int32 index;
        mButtonDTMF->FindIndexOfValue(str, &index);

        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Boolean tmp = FALSE;
        helper->PutInt32(cr, ISettingsSystem::DTMF_TONE_TYPE_WHEN_DIALING, index, &tmp);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonTTY)) {
        HandleTTYChange(preference, objValue);
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mVoicemailProviders)) {
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(objValue);
        String newProviderKey;
        cchar->ToString(&newProviderKey);
        if (DBG) {
            StringBuilder sb;
            sb += "Voicemail Provider changes from \"";
            sb += mPreviousVMProviderKey;
            sb += "\" to \"";
            sb += newProviderKey;
            sb += "\".";
            Log(sb.ToString());
        }
        // If previous provider key and the new one is same, we don't need to handle it.
        if (mPreviousVMProviderKey.Equals(newProviderKey)) {
            if (DBG) Log(String("No change is made toward VM provider setting."));
            *result = TRUE;
            return NOERROR;
        }
        UpdateVMPreferenceWidgets(newProviderKey);

        AutoPtr<VoiceMailProviderSettings> newProviderSettings =
                LoadSettingsForVoiceMailProvider(newProviderKey);

        // If the user switches to a voice mail provider and we have a
        // numbers stored for it we will automatically change the
        // phone's
        // voice mail and forwarding number to the stored ones.
        // Otherwise we will bring up provider's configuration UI.

        if (newProviderSettings == NULL) {
            // Force the user into a configuration of the chosen provider
            Logger::W(TAG, "Saved preferences not found - invoking config");
            mVMProviderSettingsForced = TRUE;
            SimulatePreferenceClick(IPreference::Probe(mVoicemailSettings));
        }
        else {
            if (DBG) Log(String("Saved preferences found - switching to them"));
            // Set this flag so if we get a failure we revert to previous provider
            mChangingVMorFwdDueToProviderChange = TRUE;
            SaveVoiceMailAndForwardingNumber(newProviderKey, newProviderSettings);
        }
    }
    // always let the preference setting proceed.
    *result = TRUE;
    return NOERROR;
}

ECode CCallFeaturesSetting::OnDialogClosed(
    /* [in] */ IEditPhoneNumberPreference* preference,
    /* [in] */ Int32 buttonClicked)
{
    if (DBG) Log(String("onPreferenceClick: request preference click on dialog close: ") +
            StringUtils::ToString(buttonClicked));
    if (buttonClicked == IDialogInterface::BUTTON_NEGATIVE) {
        return NOERROR;
    }

    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mSubMenuVoicemailSettings)) {
        HandleVMBtnClickRequest();
    }
    return NOERROR;
}

ECode CCallFeaturesSetting::OnGetDefaultNumber(
    /* [in] */ IEditPhoneNumberPreference* preference,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mSubMenuVoicemailSettings)) {
        // update the voicemail number field, which takes care of the
        // mSubMenuVoicemailSettings itself, so we should return NULL.
        if (DBG) Log(String("updating default for voicemail dialog"));
        UpdateVoiceNumberField();
        *str = String(NULL);
        return NOERROR;
    }

    String vmDisplay;
    mPhone->GetVoiceMailNumber(&vmDisplay);
    if (TextUtils::IsEmpty(vmDisplay)) {
        // if there is no voicemail number, we just return NULL to
        // indicate no contribution.
        *str = String(NULL);
        return NOERROR;
    }

    // Return the voicemail number prepended with "VM: "
    if (DBG) Log(String("updating default for call forwarding dialogs"));

    String tmp;
    GetString(Elastos::Droid::Server::Telephony::R::string::voicemail_abbreviated, &tmp);
    StringBuilder sb;
    sb += tmp;
    sb += " ";
    sb += vmDisplay;
    return sb.ToString(str);
}

ECode CCallFeaturesSetting::StartActivityForResult(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode)
{
    if (requestCode == -1) {
        // this is an intent requested from the preference framework.
        return PreferenceActivity::StartActivityForResult(intent, requestCode);
    }

    if (DBG) Log(String("startSubActivity: starting requested subactivity"));
    return PreferenceActivity::StartActivityForResult(intent, requestCode);
}

void CCallFeaturesSetting::SwitchToPreviousVoicemailProvider()
{
    if (DBG) Log(String("switchToPreviousVoicemailProvider ") + mPreviousVMProviderKey);
    if (!mPreviousVMProviderKey.IsNull()) {
        if (mVMChangeCompletedSuccessfully || mFwdChangesRequireRollback) {
            // we have to revert with carrier
            if (DBG) {
                StringBuilder sb;
                sb += "Needs to rollback.";
                sb += " mVMChangeCompletedSuccessfully=";
                sb += mVMChangeCompletedSuccessfully;
                sb += ", mFwdChangesRequireRollback=";
                sb += mFwdChangesRequireRollback;
                Log(sb.ToString());
            }

            ShowDialogIfForeground(VOICEMAIL_REVERTING_DIALOG);
            AutoPtr<VoiceMailProviderSettings> prevSettings =
                    LoadSettingsForVoiceMailProvider(mPreviousVMProviderKey);
            if (prevSettings == NULL) {
                // prevSettings never becomes NULL since it should be already loaded!
                Logger::E(TAG, "VoiceMailProviderSettings for the key \"%d\" becomes NULL, which is unexpected.",
                        mPreviousVMProviderKey.string());
                if (DBG) {
                    Logger::E(TAG,
                            "mVMChangeCompletedSuccessfully: %d, mFwdChangesRequireRollback: %d",
                            mVMChangeCompletedSuccessfully, mFwdChangesRequireRollback);
                }
            }
            if (mVMChangeCompletedSuccessfully) {
                mNewVMNumber = prevSettings->mVoicemailNumber;
                Logger::I(TAG, "VM change is already completed successfully.Have to revert VM back to"
                        "%s again.", mNewVMNumber.string());

                AutoPtr<IMessageHelper> helper;
                CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
                AutoPtr<IMessage> m;
                helper->Obtain(mRevertOptionComplete, EVENT_VOICEMAIL_CHANGED, (IMessage**)&m);
                String ss;
                mPhone->GetVoiceMailAlphaTag(&ss);
                mPhone->SetVoiceMailNumber(ss, mNewVMNumber, m);
            }
            if (mFwdChangesRequireRollback) {
                Logger::I(TAG, "Requested to rollback Fwd changes.");
                AutoPtr<ArrayOf<ICallForwardInfo*> > prevFwdSettings = prevSettings->mForwardingSettings;
                if (prevFwdSettings != NULL) {
                    AutoPtr<HashMap<Int32, AutoPtr<AsyncResult> > > results = mForwardingChangeResults;
                    ResetForwardingChangeState();
                    for (Int32 i = 0; i < prevFwdSettings->GetLength(); i++) {
                        CallForwardInfo* fi = (CallForwardInfo*)(*prevFwdSettings)[i];
                        if (DBG) {
                            StringBuilder sb;
                            sb += "Reverting fwd #: ";
                            sb += i;
                            sb += ": ";
                            sb += TO_CSTR(fi);
                            Log(sb.ToString());
                        }
                        // Only revert the settings for which the update
                        // succeeded
                        HashMap<Int32, AutoPtr<AsyncResult> >::Iterator ator = results->Find(fi->mReason);
                        AsyncResult* result = NULL;
                        if (ator != results->End()) {
                            result = ator->mSecond;
                        }
                        if (result != NULL && result->mException == NULL) {
                            mExpectedChangeResultReasons->Insert(fi->mReason);

                            AutoPtr<IMessage> m;
                            mRevertOptionComplete->ObtainMessage(
                                            EVENT_FORWARDING_CHANGED, i, 0, (IMessage**)&m);
                            mPhone->SetCallForwardingOption(
                                    (fi->mStatus == 1 ?
                                            ICommandsInterface::CF_ACTION_REGISTRATION :
                                            ICommandsInterface::CF_ACTION_DISABLE),
                                    fi->mReason,
                                    fi->mNumber,
                                    fi->mTimeSeconds,
                                    m);
                        }
                    }
                }
            }
        }
        else {
            if (DBG) Log(String("No need to revert"));
            OnRevertDone();
        }
    }
}

void CCallFeaturesSetting::OnRevertDone()
{
    if (DBG) Log(String("Flipping provider key back to ") + mPreviousVMProviderKey);
    mVoicemailProviders->SetValue(mPreviousVMProviderKey);
    UpdateVMPreferenceWidgets(mPreviousVMProviderKey);
    UpdateVoiceNumberField();
    if (mVMOrFwdSetError != 0) {
        ShowVMDialog(mVMOrFwdSetError);
        mVMOrFwdSetError = 0;
    }
}

ECode CCallFeaturesSetting::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (DBG) {
        StringBuilder sb;
        sb += "onActivityResult: requestCode: ";
        sb += requestCode;
        sb += ", resultCode: ";
        sb += resultCode;
        sb += ", data: ";
        sb += data;
        Log(sb.ToString());
    }
    // there are cases where the contact picker may end up sending us more than one
    // request.  We want to ignore the request if we're not in the correct state.
    if (requestCode == VOICEMAIL_PROVIDER_CFG_ID) {
        Boolean failure = FALSE;

        // No matter how the processing of result goes lets clear the flag
        if (DBG) Log(String("mVMProviderSettingsForced: ") + StringUtils::ToString(mVMProviderSettingsForced));
        const Boolean isVMProviderSettingsForced = mVMProviderSettingsForced;
        mVMProviderSettingsForced = FALSE;

        String vmNum;
        if (resultCode != RESULT_OK) {
            if (DBG) Log(String("onActivityResult: vm provider cfg result not OK."));
            failure = TRUE;
        }
        else {
            if (data == NULL) {
                if (DBG) Log(String("onActivityResult: vm provider cfg result has no data"));
                failure = TRUE;
            }
            else {
                Boolean res;
                if (data->GetBooleanExtra(SIGNOUT_EXTRA, FALSE, &res), res) {
                    if (DBG) Log(String("Provider requested signout"));
                    if (isVMProviderSettingsForced) {
                        if (DBG) Log(String("Going back to previous provider on signout"));
                        SwitchToPreviousVoicemailProvider();
                    }
                    else {
                        String victim = GetCurrentVoicemailProviderKey();
                        if (DBG) Log(String("Relaunching activity and ignoring ") + victim);
                        AutoPtr<IIntent> i;
                        CIntent::New(ACTION_ADD_VOICEMAIL, (IIntent**)&i);
                        i->PutExtra(IGNORE_PROVIDER_EXTRA, victim);
                        i->SetFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
                        StartActivity(i);
                    }
                    return NOERROR;
                }
                data->GetStringExtra(VM_NUMBER_EXTRA, &vmNum);
                if (vmNum.IsNull() || vmNum.GetLength() == 0) {
                    if (DBG) Log(String("onActivityResult: vm provider cfg result has no vmnum"));
                    failure = TRUE;
                }
            }
        }
        if (failure) {
            if (DBG) Log(String("Failure in return from voicemail provider"));
            if (isVMProviderSettingsForced) {
                SwitchToPreviousVoicemailProvider();
            }
            else {
                if (DBG) Log(String("Not switching back the provider since this is not forced config"));
            }
            return NOERROR;
        }
        mChangingVMorFwdDueToProviderChange = isVMProviderSettingsForced;
        String fwdNum;
        data->GetStringExtra(FWD_NUMBER_EXTRA, &fwdNum);

        // TODO: It would be nice to load the current network setting for this and
        // send it to the provider when it's config is invoked so it can use this as default
        Int32 fwdNumTime;
        data->GetInt32Extra(FWD_NUMBER_TIME_EXTRA, 20, &fwdNumTime);

        if (DBG) {
            StringBuilder sb;
            sb += "onActivityResult: vm provider cfg result ";
            sb += !fwdNum.IsNull() ? "has" : " does not have";
            sb += " forwarding number";
            Log(sb.ToString());
        }
        AutoPtr<VoiceMailProviderSettings> setting = new VoiceMailProviderSettings(vmNum, fwdNum, fwdNumTime);
        SaveVoiceMailAndForwardingNumber(GetCurrentVoicemailProviderKey(), setting);
        return NOERROR;
    }

    if (requestCode == VOICEMAIL_PREF_ID) {
        if (resultCode != RESULT_OK) {
            if (DBG) Log(String("onActivityResult: contact picker result not OK."));
            return NOERROR;
        }

        AutoPtr<ICursor> cursor;
        //try
        {
            AutoPtr<IContentResolver> contentResolver;
            GetContentResolver((IContentResolver**)&contentResolver);
            AutoPtr<IUri> uri;
            data->GetData((IUri**)&uri);
            ECode ec = contentResolver->Query(uri,
                NUM_PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
            if (FAILED(ec)) {
                if (cursor != NULL) {
                    ICloseable::Probe(cursor)->Close();
                }
            }

            Boolean res;
            if ((cursor == NULL) || (cursor->MoveToFirst(&res), !res)) {
                if (DBG) Log(String("onActivityResult: bad contact data, no results found."));
                return NOERROR;
            }
            String str;
            ec = cursor->GetString(0, &str);
            if (FAILED(ec)) {
                if (cursor != NULL) {
                    ICloseable::Probe(cursor)->Close();
                }
            }
            assert(0 && "TODO Need CEditPhoneNumberPreference");
            // ec = ((CEditPhoneNumberPreference*)mSubMenuVoicemailSettings.Get())->OnPickActivityResult(str);
            if (FAILED(ec)) {
                if (cursor != NULL) {
                    ICloseable::Probe(cursor)->Close();
                }
            }
            return NOERROR;
        }
        // finally {
            // if (cursor != NULL) {
            //     cursor->Close();
            // }
        //}
    }

    return PreferenceActivity::OnActivityResult(requestCode, resultCode, data);
}

void CCallFeaturesSetting::HandleVMBtnClickRequest()
{
    // normally called on the dialog close.

    // Since we're stripping the formatting out on the getPhoneNumber()
    // call now, we won't need to do so here anymore.

    String number;
    assert(0 && "TODO CEditPhoneNumberPreference");
    // ((CEditPhoneNumberPreference*)mSubMenuVoicemailSettings.Get())->GetPhoneNumber(&number);
    AutoPtr<VoiceMailProviderSettings> setting = new VoiceMailProviderSettings(number, FWD_SETTINGS_DONT_TOUCH);
    SaveVoiceMailAndForwardingNumber(GetCurrentVoicemailProviderKey(), setting);
}

void CCallFeaturesSetting::ShowDialogIfForeground(
    /* [in] */ Int32 id)
{
    if (mForeground) {
        ShowDialog(id);
    }
}

void CCallFeaturesSetting::DismissDialogSafely(
    /* [in] */ Int32 id)
{
    //try {
    DismissDialog(id);
    //} catch (IllegalArgumentException e) {
        // This is expected in the case where we were in the background
        // at the time we would normally have shown the dialog, so we didn't
        // show it.
    //}
}

void CCallFeaturesSetting::SaveVoiceMailAndForwardingNumber(
    /* [in] */ const String& key,
    /* [in] */ VoiceMailProviderSettings* newSettings)
{
    String str;
    newSettings->ToString(&str);
    if (DBG) Log(String("saveVoiceMailAndForwardingNumber: ") + str);
    mNewVMNumber = newSettings->mVoicemailNumber;
    // empty vm number == clearing the vm number ?
    if (mNewVMNumber == NULL) {
        mNewVMNumber = String("");
    }

    mNewFwdSettings = newSettings->mForwardingSettings;
    if (DBG) {
        StringBuilder sb;
        sb += "newFwdNumber ";
        sb += mNewFwdSettings != NULL ? mNewFwdSettings->GetLength() : 0;
        sb += " settings";
        Log(sb.ToString());
    }

    // No fwd settings on CDMA
    Int32 type;
    mPhone->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
        if (DBG) Log(String("ignoring forwarding setting since this is CDMA phone"));
        mNewFwdSettings = FWD_SETTINGS_DONT_TOUCH;
    }

    //throw a warning if the vm is the same and we do not touch forwarding.
    if (mNewVMNumber.Equals(mOldVmNumber) && mNewFwdSettings == FWD_SETTINGS_DONT_TOUCH) {
        ShowVMDialog(MSG_VM_NOCHANGE);
        return;
    }

    MaybeSaveSettingsForVoicemailProvider(key, newSettings);
    mVMChangeCompletedSuccessfully = FALSE;
    mFwdChangesRequireRollback = FALSE;
    mVMOrFwdSetError = 0;
    if (!key.Equals(mPreviousVMProviderKey)) {
        mReadingSettingsForDefaultProvider =
                mPreviousVMProviderKey.Equals(DEFAULT_VM_PROVIDER_KEY);
        if (DBG) Log(String("Reading current forwarding settings"));
        mForwardingReadResults = ArrayOf<ICallForwardInfo*>::Alloc(FORWARDING_SETTINGS_REASONS->GetLength());
        for (Int32 i = 0; i < FORWARDING_SETTINGS_REASONS->GetLength(); i++) {
            mForwardingReadResults->Set(i, NULL);
            AutoPtr<IMessage> message;
            mGetOptionComplete->ObtainMessage(EVENT_FORWARDING_GET_COMPLETED, i, 0, (IMessage**)&message);
            mPhone->GetCallForwardingOption((*FORWARDING_SETTINGS_REASONS)[i], message);
        }
        ShowDialogIfForeground(VOICEMAIL_FWD_READING_DIALOG);
    }
    else {
        SaveVoiceMailAndForwardingNumberStage2();
    }
}

void CCallFeaturesSetting::HandleForwardingSettingsReadResult(
    /* [in] */ AsyncResult* ar,
    /* [in] */ Int32 idx)
{
    if (DBG) Logger::D(TAG, "handleForwardingSettingsReadResult: %d", idx);
    AutoPtr<IThrowable> error;
    if (ar->mException != NULL) {
        if (DBG) Logger::D(TAG, "FwdRead: ar.exception="
                /*ar->mException.getMessage()*/);
        error = ar->mException;
    }
    if (IThrowable::Probe(ar->mUserObj)) {
        if (DBG) Logger::D(TAG, "FwdRead: userObj=%s", TO_CSTR(IThrowable::Probe(ar->mUserObj))
                /*((Throwable)ar.userObj).getMessage()*/);
        error = IThrowable::Probe(ar->mUserObj);
    }

    // We may have already gotten an error and decided to ignore the other results.
    if (mForwardingReadResults == NULL) {
        if (DBG) Logger::D(TAG, "ignoring fwd reading result: %d", idx);
        return;
    }

    // In case of error ignore other results, show an error dialog
    if (error != NULL) {
        if (DBG) Logger::D(TAG, "Error discovered for fwd read : %d", idx);
        mForwardingReadResults = NULL;
        DismissDialogSafely(VOICEMAIL_FWD_READING_DIALOG);
        ShowVMDialog(MSG_FW_GET_EXCEPTION);
        return;
    }

    // Get the forwarding info
    AutoPtr<ArrayOf<ICallForwardInfo*> > cfInfoArray;
    assert(0 && "TODO");
    // cfInfoArray = (CallForwardInfo[]) ar.result;
    CallForwardInfo* fi = NULL;
    for (Int32 i = 0 ; i < cfInfoArray->GetLength(); i++) {
        if ((((CallForwardInfo*)(*cfInfoArray)[i])->mServiceClass & ICommandsInterface::SERVICE_CLASS_VOICE) != 0) {
            fi = (CallForwardInfo*)(*cfInfoArray)[i];
            break;
        }
    }
    if (fi == NULL) {
        // In case we go nothing it means we need this reason disabled
        // so create a CallForwardInfo for capturing this
        if (DBG) Logger::D(TAG, "Creating default info for %d", idx);
        fi = new CallForwardInfo();
        ((CallForwardInfo*)fi)->mStatus = 0;
        ((CallForwardInfo*)fi)->mReason = (*FORWARDING_SETTINGS_REASONS)[idx];
        ((CallForwardInfo*)fi)->mServiceClass = ICommandsInterface::SERVICE_CLASS_VOICE;
    }
    else {
        // if there is not a forwarding number, ensure the entry is set to "not active."
        if (fi->mNumber.IsNull() || fi->mNumber.GetLength() == 0) {
            fi->mStatus = 0;
        }

        if (DBG) {
            String str;
            fi->ToString(&str);
            Logger::D(TAG, "Got  %s for %d", str.string(), idx);
        }
    }
    mForwardingReadResults->Set(idx, fi);

    // Check if we got all the results already
    Boolean done = TRUE;
    for (Int32 i = 0; i < mForwardingReadResults->GetLength(); i++) {
        if ((*mForwardingReadResults)[i] == NULL) {
            done = FALSE;
            break;
        }
    }
    if (done) {
        if (DBG) Logger::D(TAG, "Done receiving fwd info");
        DismissDialogSafely(VOICEMAIL_FWD_READING_DIALOG);
        if (mReadingSettingsForDefaultProvider) {
            AutoPtr<VoiceMailProviderSettings> setting = new VoiceMailProviderSettings(mOldVmNumber,
                    mForwardingReadResults);
            MaybeSaveSettingsForVoicemailProvider(DEFAULT_VM_PROVIDER_KEY, setting);
            mReadingSettingsForDefaultProvider = FALSE;
        }
        SaveVoiceMailAndForwardingNumberStage2();
    }
    else {
        if (DBG) Logger::D(TAG, "Not done receiving fwd info");
    }
}

AutoPtr<ICallForwardInfo> CCallFeaturesSetting::InfoForReason(
    /* [in] */ ArrayOf<ICallForwardInfo*>* infos,
    /* [in] */ Int32 reason)
{
    AutoPtr<ICallForwardInfo> result;
    if (NULL != infos) {
        for (Int32 i = 0; i < infos->GetLength(); i++) {
            AutoPtr<ICallForwardInfo> info = (*infos)[i];

            if (((CallForwardInfo*)info.Get())->mReason == reason) {
                result = info;
                break;
            }
        }
    }
    return result;
}

Boolean CCallFeaturesSetting::IsUpdateRequired(
    /* [in] */ ICallForwardInfo* oldInfo,
    /* [in] */ ICallForwardInfo* newInfo)
{
    Boolean result = TRUE;
    if (0 == ((CallForwardInfo*)newInfo)->mStatus) {
        // If we're disabling a type of forwarding, and it's already
        // disabled for the account, don't make any change
        if (oldInfo != NULL && ((CallForwardInfo*)oldInfo)->mStatus == 0) {
            result = FALSE;
        }
    }
    return result;
}

void CCallFeaturesSetting::ResetForwardingChangeState()
{
    mForwardingChangeResults = new HashMap<Int32, AutoPtr<AsyncResult> >();
    mExpectedChangeResultReasons = new HashSet<Int32>();
}

void CCallFeaturesSetting::SaveVoiceMailAndForwardingNumberStage2()
{
    mForwardingChangeResults = NULL;
    mVoicemailChangeResult = NULL;
    if (mNewFwdSettings != FWD_SETTINGS_DONT_TOUCH) {
        ResetForwardingChangeState();
        for (Int32 i = 0; i < mNewFwdSettings->GetLength(); i++) {
            CallForwardInfo* fi = (CallForwardInfo*)(*mNewFwdSettings)[i];

            Boolean doUpdate = IsUpdateRequired(InfoForReason(
                        mForwardingReadResults, fi->mReason), fi);

            if (doUpdate) {
                if (DBG) {
                    StringBuilder sb;
                    sb += "Setting fwd #: ";
                    sb += i;
                    String str;
                    fi->ToString(&str);
                    sb += str;
                    Log(sb.ToString());
                }
                mExpectedChangeResultReasons->Insert(i);

                AutoPtr<IMessage> message;
                mSetOptionComplete->ObtainMessage(
                                EVENT_FORWARDING_CHANGED, fi->mReason, 0, (IMessage**)&message);
                mPhone->SetCallForwardingOption(
                        fi->mStatus == 1 ?
                                ICommandsInterface::CF_ACTION_REGISTRATION :
                                ICommandsInterface::CF_ACTION_DISABLE,
                        fi->mReason,
                        fi->mNumber,
                        fi->mTimeSeconds,
                        message);
            }
        }
        ShowDialogIfForeground(VOICEMAIL_FWD_SAVING_DIALOG);
    }
    else {
        if (DBG) Log(String("Not touching fwd #"));
        SetVMNumberWithCarrier();
    }
}

void CCallFeaturesSetting::SetVMNumberWithCarrier()
{
    if (DBG) Log(String("save voicemail #: ") + mNewVMNumber);

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> message;
    helper->Obtain(mSetOptionComplete, EVENT_VOICEMAIL_CHANGED, (IMessage**)&message);

    String ss;
    mPhone->GetVoiceMailAlphaTag(&ss);
    mPhone->SetVoiceMailNumber(ss, mNewVMNumber, message);
}

Boolean CCallFeaturesSetting::CheckForwardingCompleted()
{
    Boolean result;
    if (mForwardingChangeResults == NULL) {
        result = TRUE;
    }
    else {
        // return TRUE iff there is a change result for every reason for
        // which we expected a result
        result = TRUE;
        HashSet<Int32>::Iterator ator = mExpectedChangeResultReasons->Begin();
        for (; ator != mExpectedChangeResultReasons->End(); ++ator) {
            Int32 reason = *ator;

            HashMap<Int32, AutoPtr<AsyncResult> >::Iterator it = mForwardingChangeResults->Find(reason);
            if (it == mForwardingChangeResults->End()) {
                result = FALSE;
                break;
            }
        }
    }
    return result;
}

String CCallFeaturesSetting::CheckFwdChangeSuccess()
{
    String result;
    HashMap<Int32, AutoPtr<AsyncResult> >::Iterator it = mForwardingChangeResults->Begin();
    for (; it != mForwardingChangeResults->End(); ++it) {
        AutoPtr<IThrowable> exception = it->mSecond->mException;
        if (exception != NULL) {
            exception->GetMessage(&result);
            if (result == NULL) {
                result = "";
            }
            break;
        }
    }

    return result;
}

String CCallFeaturesSetting::CheckVMChangeSuccess()
{
    if (mVoicemailChangeResult->mException != NULL) {
        String msg;
        mVoicemailChangeResult->mException->GetMessage(&msg);
        if (msg.IsNull()) {
            return String("");
        }
        return msg;
    }
    return String(NULL);
}

void CCallFeaturesSetting::HandleSetVMOrFwdMessage()
{
    if (DBG) {
        Log(String("handleSetVMMessage: set VM request complete"));
    }
    Boolean success = TRUE;
    Boolean fwdFailure = FALSE;
    String exceptionMessage("");
    if (mForwardingChangeResults != NULL) {
        exceptionMessage = CheckFwdChangeSuccess();
        if (exceptionMessage != NULL) {
            success = FALSE;
            fwdFailure = TRUE;
        }
    }
    if (success) {
        exceptionMessage = CheckVMChangeSuccess();
        if (exceptionMessage != NULL) {
            success = FALSE;
        }
    }
    if (success) {
        if (DBG) Log(String("change VM success!"));
        HandleVMAndFwdSetSuccess(MSG_VM_OK);
    }
    else {
        if (fwdFailure) {
            Logger::W(TAG, "Failed to change fowarding setting. Reason: %s", exceptionMessage.string());
            HandleVMOrFwdSetError(MSG_FW_SET_EXCEPTION);
        }
        else {
            Logger::W(TAG, "Failed to change voicemail. Reason: %s", exceptionMessage.string());
            HandleVMOrFwdSetError(MSG_VM_EXCEPTION);
        }
    }
}

void CCallFeaturesSetting::HandleVMOrFwdSetError(
    /* [in] */ Int32 msgId)
{
    if (mChangingVMorFwdDueToProviderChange) {
        mVMOrFwdSetError = msgId;
        mChangingVMorFwdDueToProviderChange = FALSE;
        SwitchToPreviousVoicemailProvider();
        return;
    }
    mChangingVMorFwdDueToProviderChange = FALSE;
    ShowVMDialog(msgId);
    UpdateVoiceNumberField();
}

void CCallFeaturesSetting::HandleVMAndFwdSetSuccess(
    /* [in] */ Int32 msg)
{
    if (DBG) {
        Log(String("handleVMAndFwdSetSuccess(). current voicemail provider key: ")
                + GetCurrentVoicemailProviderKey());
    }
    mPreviousVMProviderKey = GetCurrentVoicemailProviderKey();
    mChangingVMorFwdDueToProviderChange = FALSE;
    ShowVMDialog(msg);
    UpdateVoiceNumberField();
}

void CCallFeaturesSetting::UpdateVoiceNumberField()
{
    if (DBG) {
        Log(String("updateVoiceNumberField(). mSubMenuVoicemailSettings=") + TO_CSTR(mSubMenuVoicemailSettings));
    }
    if (mSubMenuVoicemailSettings == NULL) {
        return;
    }

    mPhone->GetVoiceMailNumber(&mOldVmNumber);
    if (mOldVmNumber.IsNull()) {
        mOldVmNumber = String("");
    }
    mSubMenuVoicemailSettings->SetPhoneNumber(mOldVmNumber);
    String summary;
    if (mOldVmNumber.GetLength() > 0) {
        summary = mOldVmNumber;
    }
    else {
        GetString(Elastos::Droid::Server::Telephony::R::string::voicemail_number_not_set, &summary);
    }

    AutoPtr<ICharSequence> cs;
    CString::New(summary, (ICharSequence**)&cs);
    IPreference::Probe(mSubMenuVoicemailSettings)->SetSummary(cs);
}

void CCallFeaturesSetting::OnPrepareDialog(
    /* [in] */ Int32 id,
    /* [in] */ IDialog* dialog)
{
    PreferenceActivity::OnPrepareDialog(id, dialog);
    mCurrentDialogId = id;
}

AutoPtr<IDialog> CCallFeaturesSetting::OnCreateDialog(
    /* [in] */ Int32 id)
{
    if ((id == VM_RESPONSE_ERROR) || (id == VM_NOCHANGE_ERROR) ||
        (id == FW_SET_RESPONSE_ERROR) || (id == FW_GET_RESPONSE_ERROR) ||
            (id == VOICEMAIL_DIALOG_CONFIRM)) {

        AutoPtr<IAlertDialogBuilder> b;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&b);

        Int32 msgId;
        Int32 titleId = Elastos::Droid::Server::Telephony::R::string::error_updating_title;
        switch (id) {
            case VOICEMAIL_DIALOG_CONFIRM:
                msgId = Elastos::Droid::Server::Telephony::R::string::vm_changed;
                titleId = Elastos::Droid::Server::Telephony::R::string::voicemail;
                // Set Button 2
                b->SetNegativeButton(Elastos::Droid::Server::Telephony::R::string::close_dialog, this);
                break;
            case VM_NOCHANGE_ERROR:
                // even though this is technically an error,
                // keep the title friendly.
                msgId = Elastos::Droid::Server::Telephony::R::string::no_change;
                titleId = Elastos::Droid::Server::Telephony::R::string::voicemail;
                // Set Button 2
                b->SetNegativeButton(Elastos::Droid::Server::Telephony::R::string::close_dialog, this);
                break;
            case VM_RESPONSE_ERROR:
                msgId = Elastos::Droid::Server::Telephony::R::string::vm_change_failed;
                // Set Button 1
                b->SetPositiveButton(Elastos::Droid::Server::Telephony::R::string::close_dialog, this);
                break;
            case FW_SET_RESPONSE_ERROR:
                msgId = Elastos::Droid::Server::Telephony::R::string::fw_change_failed;
                // Set Button 1
                b->SetPositiveButton(Elastos::Droid::Server::Telephony::R::string::close_dialog, this);
                break;
            case FW_GET_RESPONSE_ERROR:
                msgId = Elastos::Droid::Server::Telephony::R::string::fw_get_in_vm_failed;
                b->SetPositiveButton(Elastos::Droid::Server::Telephony::R::string::alert_dialog_yes, this);
                b->SetNegativeButton(Elastos::Droid::Server::Telephony::R::string::alert_dialog_no, this);
                break;
            default:
                msgId = Elastos::Droid::Server::Telephony::R::string::exception_error;
                // Set Button 3, tells the activity that the error is
                // not recoverable on dialog exit.
                b->SetNeutralButton(Elastos::Droid::Server::Telephony::R::string::close_dialog, this);
                break;
        }

        AutoPtr<ICharSequence> title;
        GetText(titleId, (ICharSequence**)&title);
        b->SetTitle(title);

        AutoPtr<ICharSequence> text;
        GetText(msgId, (ICharSequence**)&text);
        String message;
        text->ToString(&message);

        text = NULL;
        CString::New(message, (ICharSequence**)&text);
        b->SetMessage(text);
        b->SetCancelable(FALSE);
        AutoPtr<IAlertDialog> dialog;
        b->Create((IAlertDialog**)&dialog);

        // make the dialog more obvious by bluring the background.
        AutoPtr<IWindow> window;
        IDialog::Probe(dialog)->GetWindow((IWindow**)&window);
        window->AddFlags(IWindowManagerLayoutParams::FLAG_BLUR_BEHIND);

        return IDialog::Probe(dialog);
    }
    else if (id == VOICEMAIL_FWD_SAVING_DIALOG || id == VOICEMAIL_FWD_READING_DIALOG ||
            id == VOICEMAIL_REVERTING_DIALOG) {
        AutoPtr<IProgressDialog> dialog;
        CProgressDialog::New(this, (IProgressDialog**)&dialog);
        AutoPtr<ICharSequence> title;
        GetText(Elastos::Droid::Server::Telephony::R::string::updating_title, (ICharSequence**)&title);
        IDialog::Probe(dialog)->SetTitle(title);
        dialog->SetIndeterminate(TRUE);
        IDialog::Probe(dialog)->SetCancelable(FALSE);

        AutoPtr<ICharSequence> message;
        GetText(id == VOICEMAIL_FWD_SAVING_DIALOG ? Elastos::Droid::Server::Telephony::R::string::updating_settings :
                (id == VOICEMAIL_REVERTING_DIALOG ? Elastos::Droid::Server::Telephony::R::string::reverting_settings :
                Elastos::Droid::Server::Telephony::R::string::reading_settings), (ICharSequence**)&message);
        IAlertDialog::Probe(dialog)->SetMessage(message);
        return IDialog::Probe(dialog);
    }

    return NULL;
}

ECode CCallFeaturesSetting::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
   dialog->Dismiss();
    switch (which){
        case IDialogInterface::BUTTON_NEUTRAL:
            if (DBG) Log(String("Neutral button"));
            break;
        case IDialogInterface::BUTTON_NEGATIVE:
            if (DBG) Log(String("Negative button"));
            if (mCurrentDialogId == FW_GET_RESPONSE_ERROR) {
                // We failed to get current forwarding settings and the user
                // does not wish to continue.
                SwitchToPreviousVoicemailProvider();
            }
            break;
        case IDialogInterface::BUTTON_POSITIVE:
            if (DBG) Log(String("Positive button"));
            if (mCurrentDialogId == FW_GET_RESPONSE_ERROR) {
                // We failed to get current forwarding settings but the user
                // wishes to continue changing settings to the new vm provider
                SaveVoiceMailAndForwardingNumberStage2();
            }
            else {
                Finish();
            }
            return NOERROR;
        default: {
            // just let the dialog close and go back to the input
        }
    }
    // In all dialogs, all buttons except BUTTON_POSITIVE lead to the end of user interaction
    // with settings UI. If we were called to explicitly configure voice mail then
    // we finish the settings activity here to come back to whatever the user was doing.
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    if (action.Equals(ACTION_ADD_VOICEMAIL)) {
        Finish();
    }
    return NOERROR;
}

void CCallFeaturesSetting::ShowVMDialog(
    /* [in] */ Int32 msgStatus)
{
    switch (msgStatus) {
        // It's a bit worrisome to punt in the error cases here when we're
        // not in the foreground; maybe toast instead?
        case MSG_VM_EXCEPTION:
            ShowDialogIfForeground(VM_RESPONSE_ERROR);
            break;
        case MSG_FW_SET_EXCEPTION:
            ShowDialogIfForeground(FW_SET_RESPONSE_ERROR);
            break;
        case MSG_FW_GET_EXCEPTION:
            ShowDialogIfForeground(FW_GET_RESPONSE_ERROR);
            break;
        case MSG_VM_NOCHANGE:
            ShowDialogIfForeground(VM_NOCHANGE_ERROR);
            break;
        case MSG_VM_OK:
            ShowDialogIfForeground(VOICEMAIL_DIALOG_CONFIRM);
            break;
        case MSG_OK:
        default: {
            // This should never happen.
        }
    }
}

ECode CCallFeaturesSetting::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceActivity::OnCreate(icicle);
    if (DBG) {
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);
        Log(String("onCreate(). Intent: ") + TO_CSTR(intent));
    }

    assert(0 && "TODO :need PhoneGlobals");
    // mPhone = PhoneGlobals::GetPhone();
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);

    // create intent to bring up contact list
    CIntent::New(IIntent::ACTION_GET_CONTENT, (IIntent**)&mContactListIntent);
    mContactListIntent->SetType(IContactsPhones::CONTENT_ITEM_TYPE);

    mVoicemailRingtoneLookupRunnable = new MyRunnable(this);

    // Show the voicemail preference in onResume if the calling intent specifies the
    // ACTION_ADD_VOICEMAIL action.

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    mShowVoicemailPreference = (icicle == NULL) && action.Equals(ACTION_ADD_VOICEMAIL);
    return NOERROR;
}

void CCallFeaturesSetting::InitPhoneAccountPreferences()
{
    FindPreference(PHONE_ACCOUNT_SETTINGS_KEY, (IPreference**)&mPhoneAccountSettingsPreference);

    AutoPtr<ITelecomManagerHelper> helper;
    CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
    AutoPtr<ITelecomManager> telecomManager;
    helper->From(this, (ITelecomManager**)&telecomManager);

    Int32 count;
    telecomManager->GetAllPhoneAccountsCount(&count);
    AutoPtr<IList> l;
    telecomManager->GetSimCallManagers((IList**)&l);
    Boolean b = FALSE;
    l->IsEmpty(&b);
    assert(0 && "TODO Need SipUtil");
    if (count <= 1 && b /*&& !SipUtil::IsVoipSupported(this)*/) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        Boolean tmp = FALSE;
        IPreferenceGroup::Probe(screen)->RemovePreference(mPhoneAccountSettingsPreference, &tmp);
    }
}

Boolean CCallFeaturesSetting::CanLaunchIntent(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IPackageManager> pm;
    GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IResolveInfo> ri;
    pm->ResolveActivity(intent, IPackageManager::GET_ACTIVITIES, (IResolveInfo**)&ri);
    return ri.Get() != NULL;
}

ECode CCallFeaturesSetting::OnResume()
{
    PreferenceActivity::OnResume();
    mForeground = TRUE;

    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    if (preferenceScreen != NULL) {
        IPreferenceGroup::Probe(preferenceScreen)->RemoveAll();
    }

    AddPreferencesFromResource(Elastos::Droid::Server::Telephony::R::xml::call_feature_setting);
    InitPhoneAccountPreferences();

    // get buttons
    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);

    AutoPtr<IPreference> preference;
    FindPreference(BUTTON_VOICEMAIL_KEY, (IPreference**)&preference);
    mSubMenuVoicemailSettings = IEditPhoneNumberPreference::Probe(preference);
    if (mSubMenuVoicemailSettings != NULL) {
        mSubMenuVoicemailSettings->SetParentActivity(this, VOICEMAIL_PREF_ID, this);
        mSubMenuVoicemailSettings->SetDialogOnClosedListener(this);
        IDialogPreference::Probe(mSubMenuVoicemailSettings)->SetDialogTitle(Elastos::Droid::Server::Telephony::R::string::voicemail_settings_number_label);
    }

    AutoPtr<IPreference> preference2;
    FindPreference(BUTTON_DTMF_KEY, (IPreference**)&preference2);
    mButtonDTMF = IListPreference::Probe(preference2);

    AutoPtr<IPreference> preference3;
    FindPreference(BUTTON_RETRY_KEY, (IPreference**)&preference3);
    mButtonAutoRetry = ICheckBoxPreference::Probe(preference3);

    AutoPtr<IPreference> preference4;
    FindPreference(BUTTON_HAC_KEY, (IPreference**)&preference4);
    mButtonHAC = ICheckBoxPreference::Probe(preference4);

    AutoPtr<IPreference> preference5;
    FindPreference(BUTTON_TTY_KEY, (IPreference**)&preference5);
    mButtonTTY = IListPreference::Probe(preference5);

    AutoPtr<IPreference> preference6;
    FindPreference(BUTTON_VOICEMAIL_PROVIDER_KEY, (IPreference**)&preference6);
    mVoicemailProviders = IListPreference::Probe(preference6);

    if (mVoicemailProviders != NULL) {
        AutoPtr<PreferenceOnPreferenceChangeListener> l = new PreferenceOnPreferenceChangeListener(this);
        IPreference::Probe(mVoicemailProviders)->SetOnPreferenceChangeListener(l);

        AutoPtr<IPreference> preference7;
        FindPreference(VOICEMAIL_SETTING_SCREEN_PREF_KEY, (IPreference**)&preference7);
        mVoicemailSettingsScreen = IPreferenceScreen::Probe(preference7);

        AutoPtr<IPreference> preference8;
        FindPreference(BUTTON_VOICEMAIL_SETTING_KEY, (IPreference**)&preference8);
        mVoicemailSettings = IPreferenceScreen::Probe(preference8);

        FindPreference(BUTTON_VOICEMAIL_NOTIFICATION_RINGTONE_KEY, (IPreference**)&mVoicemailNotificationRingtone);

        AutoPtr<IPreference> preference9;
        FindPreference(BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_KEY, (IPreference**)&preference9);
        mVoicemailNotificationVibrate = ICheckBoxPreference::Probe(preference9);

        InitVoiceMailProviders();
    }

    Boolean res = FALSE;
    if (mButtonDTMF != NULL) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        if (resources->GetBoolean(Elastos::Droid::Server::Telephony::R::bool_::dtmf_type_enabled, &res), res) {
            AutoPtr<PreferenceOnPreferenceChangeListener> l = new PreferenceOnPreferenceChangeListener(this);
            IPreference::Probe(mButtonDTMF)->SetOnPreferenceChangeListener(l);
        }
        else {
            IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mButtonDTMF), &res);
            mButtonDTMF = NULL;
        }
    }

    if (mButtonAutoRetry != NULL) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        if (resources->GetBoolean(Elastos::Droid::Server::Telephony::R::bool_::auto_retry_enabled, &res), res) {
            AutoPtr<PreferenceOnPreferenceChangeListener> l = new PreferenceOnPreferenceChangeListener(this);
            IPreference::Probe(mButtonAutoRetry)->SetOnPreferenceChangeListener(l);
        }
        else {
            IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mButtonAutoRetry), &res);
            mButtonAutoRetry = NULL;
        }
    }

    if (mButtonHAC != NULL) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        if (resources->GetBoolean(Elastos::Droid::Server::Telephony::R::bool_::hac_enabled, &res), res) {
            AutoPtr<PreferenceOnPreferenceChangeListener> l = new PreferenceOnPreferenceChangeListener(this);
            IPreference::Probe(mButtonHAC)->SetOnPreferenceChangeListener(l);
        }
        else {
            IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mButtonHAC), &res);
            mButtonHAC = NULL;
        }
    }

    if (mButtonTTY != NULL) {
        AutoPtr<ITelecomManagerHelper> helper;
        CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
        AutoPtr<ITelecomManager> telecomManager;
        helper->From(this, (ITelecomManager**)&telecomManager);
        if (telecomManager != NULL && (telecomManager->IsTtySupported(&res), res)) {
            AutoPtr<PreferenceOnPreferenceChangeListener> l = new PreferenceOnPreferenceChangeListener(this);
            IPreference::Probe(mButtonTTY)->SetOnPreferenceChangeListener(l);
        }
        else {
            IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mButtonTTY), &res);
            mButtonTTY = NULL;
        }
    }

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    if (resources->GetBoolean(Elastos::Droid::Server::Telephony::R::bool_::world_phone, &res), !res) {
        AutoPtr<IPreference> options;
        AutoPtr<ICharSequence> cs;
        CString::New(BUTTON_CDMA_OPTIONS, (ICharSequence**)&cs);
        IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&options);
        if (options != NULL) {
            IPreferenceGroup::Probe(prefSet)->RemovePreference(options, &res);
        }
        options = NULL;
        cs = NULL;
        CString::New(BUTTON_GSM_UMTS_OPTIONS, (ICharSequence**)&cs);
        IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&options);
        if (options != NULL) {
            IPreferenceGroup::Probe(prefSet)->RemovePreference(options, &res);
        }

        Int32 phoneType;
        mPhone->GetPhoneType(&phoneType);
        if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
            AutoPtr<IPreference> fdnButton;
            cs = NULL;
            CString::New(BUTTON_FDN_KEY, (ICharSequence**)&cs);
            IPreferenceGroup::Probe(prefSet)->FindPreference(cs, (IPreference**)&fdnButton);
            if (fdnButton != NULL) {
                IPreferenceGroup::Probe(prefSet)->RemovePreference(fdnButton, &res);
            }
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            if (resources->GetBoolean(Elastos::Droid::Server::Telephony::R::bool_::config_voice_privacy_disable, &res), !res) {
                AddPreferencesFromResource(Elastos::Droid::Server::Telephony::R::xml::cdma_call_privacy);
            }
        }
        else if (phoneType == IPhoneConstants::PHONE_TYPE_GSM) {
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            if (resources->GetBoolean(Elastos::Droid::Server::Telephony::R::bool_::config_additional_call_setting, &res), res) {
                AddPreferencesFromResource(Elastos::Droid::Server::Telephony::R::xml::gsm_umts_call_options);
            }
        }
        else {
            //throw new IllegalStateException("Unexpected phone type: " + phoneType);
            Logger::E("CCallFeaturesSetting", "Unexpected phone type: %d", phoneType);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }

    // check the intent that started this activity and pop up the voicemail
    // dialog if we've been asked to.
    // If we have at least one non default VM provider registered then bring up
    // the selection for the VM provider, otherwise bring up a VM number dialog.
    // We only bring up the dialog the first time we are called (not after orientation change)
    if (mShowVoicemailPreference && mVoicemailProviders != NULL) {
        Int32 size = mVMProvidersData.GetSize();
        if (DBG) {
            Log(String("ACTION_ADD_VOICEMAIL Intent is thrown. current VM data size: ")
                    + StringUtils::ToString(size));
        }
        if (size > 1) {
            SimulatePreferenceClick(IPreference::Probe(mVoicemailProviders));
        }
        else {
            Boolean tmp = FALSE;
            AutoPtr<ICharSequence> cs;
            CString::New(DEFAULT_VM_PROVIDER_KEY, (ICharSequence**)&cs);
            OnPreferenceChange(IPreference::Probe(mVoicemailProviders), cs, &tmp);
            mVoicemailProviders->SetValue(DEFAULT_VM_PROVIDER_KEY);
        }
        mShowVoicemailPreference = FALSE;
    }

    UpdateVoiceNumberField();
    mVMProviderSettingsForced = FALSE;

    if (mButtonDTMF != NULL) {
        AutoPtr<IContentResolver> contentResolver;
        GetContentResolver((IContentResolver**)&contentResolver);
        AutoPtr<ISettingsSystem> helper;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
        Int32 dtmf;
        helper->GetInt32(contentResolver, ISettingsSystem::DTMF_TONE_TYPE_WHEN_DIALING,
                IConstants::DTMF_TONE_TYPE_NORMAL, &dtmf);
        mButtonDTMF->SetValueIndex(dtmf);
    }

    if (mButtonAutoRetry != NULL) {
        AutoPtr<IContentResolver> contentResolver;
        GetContentResolver((IContentResolver**)&contentResolver);
        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 autoretry;
        helper->GetInt32(contentResolver, ISettingsGlobal::CALL_AUTO_RETRY, 0, &autoretry);
        ITwoStatePreference::Probe(mButtonAutoRetry)->SetChecked(autoretry != 0);
    }

    if (mButtonHAC != NULL) {
        AutoPtr<IContentResolver> contentResolver;
        GetContentResolver((IContentResolver**)&contentResolver);
        AutoPtr<ISettingsSystem> helper;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
        Int32 hac;
        helper->GetInt32(contentResolver, ISettingsSystem::HEARING_AID,
                0, &hac);
        ITwoStatePreference::Probe(mButtonHAC)->SetChecked(hac != 0);
    }

    if (mButtonTTY != NULL) {
        AutoPtr<IContentResolver> contentResolver;
        GetContentResolver((IContentResolver**)&contentResolver);
        AutoPtr<ISettingsSecure> helper;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&helper);
        Int32 settingsTtyMode;
        helper->GetInt32(contentResolver, ISettingsSecure::PREFERRED_TTY_MODE,
                ITelecomManager::TTY_MODE_OFF, &settingsTtyMode);

        mButtonTTY->SetValue(StringUtils::ToString(settingsTtyMode));
        UpdatePreferredTtyModeSummary(settingsTtyMode);
    }

    AutoPtr<IContext> context;
    mPhone->GetContext((IContext**)&context);
    AutoPtr<IPreferenceManagerHelper> pmHelper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&pmHelper);
    AutoPtr<ISharedPreferences> prefs;
    pmHelper->GetDefaultSharedPreferences(context, (ISharedPreferences**)&prefs);
    if (MigrateVoicemailVibrationSettingsIfNeeded(prefs, &res), res) {
        prefs->GetBoolean(
                BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_KEY, FALSE, &res);
        ITwoStatePreference::Probe(mVoicemailNotificationVibrate)->SetChecked(res);
    }

    // Look up the voicemail ringtone name asynchronously and update its preference.
    AutoPtr<IThread> t;
    CThread::New(mVoicemailRingtoneLookupRunnable, (IThread**)&t);
    return t->Start();
}

ECode CCallFeaturesSetting::MigrateVoicemailVibrationSettingsIfNeeded(
    /* [in] */ ISharedPreferences* prefs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    if (prefs->Contains(BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_KEY, &res), !res) {
        String vibrateWhen;
        prefs->GetString(
                BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_WHEN_KEY, VOICEMAIL_VIBRATION_NEVER, &vibrateWhen);
        // If vibrateWhen is always, then voicemailVibrate should be True.
        // otherwise if vibrateWhen is "only in silent mode", or "never", then
        // voicemailVibrate = False.
        Boolean voicemailVibrate = vibrateWhen.Equals(VOICEMAIL_VIBRATION_ALWAYS);
        AutoPtr<ISharedPreferencesEditor> editor;
        prefs->Edit((ISharedPreferencesEditor**)&editor);
        editor->PutBoolean(BUTTON_VOICEMAIL_NOTIFICATION_VIBRATE_KEY, voicemailVibrate);
        editor->Commit(&res);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

Boolean CCallFeaturesSetting::IsAirplaneModeOn()
{
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    Int32 tmp;
    helper->GetInt32(contentResolver, ISettingsSystem::AIRPLANE_MODE_ON,
            0, &tmp);

    return tmp != 0;
}

void CCallFeaturesSetting::HandleTTYChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* objValue)
{
    AutoPtr<ICharSequence> cchar = ICharSequence::Probe(objValue);
    String str;
    cchar->ToString(&str);
    Int32 buttonTtyMode = 0;
    StringUtils::Parse(str, &buttonTtyMode);

    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsSecure> helper;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&helper);
    Int32 settingsTtyMode;
    helper->GetInt32(contentResolver, ISettingsSecure::PREFERRED_TTY_MODE,
            ITelecomManager::TTY_MODE_OFF, &settingsTtyMode);

    if (DBG) Log(String("handleTTYChange: requesting set TTY mode enable (TTY) to") +
            StringUtils::ToString(buttonTtyMode));

    if (buttonTtyMode != settingsTtyMode) {
        switch(buttonTtyMode) {
            case ITelecomManager::TTY_MODE_OFF:
            case ITelecomManager::TTY_MODE_FULL:
            case ITelecomManager::TTY_MODE_HCO:
            case ITelecomManager::TTY_MODE_VCO: {
                Boolean tmp = FALSE;
                helper->PutInt32(contentResolver, ISettingsSecure::PREFERRED_TTY_MODE,
                    buttonTtyMode, &tmp);
                break;
            }
            default: {
                buttonTtyMode = ITelecomManager::TTY_MODE_OFF;
            }
        }

        mButtonTTY->SetValue(StringUtils::ToString(buttonTtyMode));
        UpdatePreferredTtyModeSummary(buttonTtyMode);
        AutoPtr<IIntent> ttyModeChanged;
        CIntent::New(ITelecomManager::ACTION_TTY_PREFERRED_MODE_CHANGED, (IIntent**)&ttyModeChanged);
        ttyModeChanged->PutExtra(ITelecomManager::EXTRA_TTY_PREFERRED_MODE, buttonTtyMode);
        AutoPtr<IUserHandleHelper> uHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uHelper);
        AutoPtr<IUserHandle> ALL;
        uHelper->GetALL((IUserHandle**)&ALL);
        SendBroadcastAsUser(ttyModeChanged, ALL);
    }
}

void CCallFeaturesSetting::UpdatePreferredTtyModeSummary(
    /* [in] */ Int32 TtyMode)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ArrayOf<String> > txts;
    resources->GetStringArray(Elastos::Droid::Server::Telephony::R::array::tty_mode_entries, (ArrayOf<String>**)&txts);
    switch(TtyMode) {
        case ITelecomManager::TTY_MODE_OFF:
        case ITelecomManager::TTY_MODE_HCO:
        case ITelecomManager::TTY_MODE_VCO:
        case ITelecomManager::TTY_MODE_FULL: {
            AutoPtr<ICharSequence> cs;
            CString::New((*txts)[TtyMode], (ICharSequence**)&cs);
            IPreference::Probe(mButtonTTY)->SetSummary(cs);
            break;
        }
        default: {
            IPreference::Probe(mButtonTTY)->SetEnabled(FALSE);
            AutoPtr<ICharSequence> cs;
            CString::New((*txts)[ITelecomManager::TTY_MODE_OFF], (ICharSequence**)&cs);
            IPreference::Probe(mButtonTTY)->SetSummary(cs);
            break;
        }
    }
}

void CCallFeaturesSetting::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

void CCallFeaturesSetting::UpdateVMPreferenceWidgets(
    /* [in] */ const String& currentProviderSetting)
{
    const String key = currentProviderSetting;
    VoiceMailProvider* provider = NULL;
    HashMap<String, AutoPtr<VoiceMailProvider> >::Iterator ator = mVMProvidersData.Find(key);
    if (ator != mVMProvidersData.End()) {
        provider = ator->mSecond;
    }

    /* This is the case when we are coming up on a freshly wiped phone and there is no
     persisted value for the list preference mVoicemailProviders.
     In this case we want to show the UI asking the user to select a voicemail provider as
     opposed to silently falling back to default one. */
    if (provider == NULL) {
        if (DBG) {
            StringBuilder sb;
            sb += "updateVMPreferenceWidget: provider for the key \"";
            sb += key;
            sb += "\" is NULL.";
            Log(sb.ToString());
        }
        String str;
        GetString(Elastos::Droid::Server::Telephony::R::string::sum_voicemail_choose_provider, &str);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        IPreference::Probe(mVoicemailProviders)->SetSummary(cs);
        IPreference::Probe(mVoicemailSettings)->SetEnabled(FALSE);
        IPreference::Probe(mVoicemailSettings)->SetIntent(NULL);

        IPreference::Probe(mVoicemailNotificationVibrate)->SetEnabled(FALSE);
    }
    else {
        if (DBG) {
            Log(String("updateVMPreferenceWidget: provider for the key \"") + key + "\".."
                    + "name: " + provider->mName
                    + ", intent: " + TO_CSTR(provider->mIntent));
        }
        String providerName = provider->mName;
        AutoPtr<ICharSequence> cs;
        CString::New(providerName, (ICharSequence**)&cs);
        IPreference::Probe(mVoicemailProviders)->SetSummary(cs);
        IPreference::Probe(mVoicemailSettings)->SetEnabled(TRUE);
        IPreference::Probe(mVoicemailSettings)->SetIntent(provider->mIntent);

        IPreference::Probe(mVoicemailNotificationVibrate)->SetEnabled(TRUE);
    }
}

void CCallFeaturesSetting::InitVoiceMailProviders()
{
    if (DBG) Log(String("initVoiceMailProviders()"));

    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    context->GetSharedPreferences(
            VM_NUMBERS_SHARED_PREFERENCES_NAME, MODE_PRIVATE, (ISharedPreferences**)&mPerProviderSavedVMNumbers);

    String providerToIgnore;
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    if (action.Equals(ACTION_ADD_VOICEMAIL)) {
        Boolean res;
        if (intent->HasExtra(IGNORE_PROVIDER_EXTRA, &res), res) {
            intent->GetStringExtra(IGNORE_PROVIDER_EXTRA, &providerToIgnore);
        }
        if (DBG) Log(String("Found ACTION_ADD_VOICEMAIL. providerToIgnore=") + providerToIgnore);
        if (!providerToIgnore.IsNull()) {
            // IGNORE_PROVIDER_EXTRA implies we want to remove the choice from the list.
            DeleteSettingsForVoicemailProvider(providerToIgnore);
        }
    }

    mVMProvidersData.Clear();

    // Stick the default element which is always there
    String myCarrier;
    GetString(Elastos::Droid::Server::Telephony::R::string::voicemail_default, &myCarrier);
    AutoPtr<VoiceMailProvider> pro = new VoiceMailProvider(myCarrier, NULL);
    mVMProvidersData[DEFAULT_VM_PROVIDER_KEY] = pro;

    // Enumerate providers
    AutoPtr<IPackageManager> pm;
    GetPackageManager((IPackageManager**)&pm);
    intent = NULL;
    CIntent::New((IIntent**)&intent);
    intent->SetAction(ACTION_CONFIGURE_VOICEMAIL);
    AutoPtr<IList> resolveInfos;
    pm->QueryIntentActivities(intent, 0, (IList**)&resolveInfos);
    Int32 size;
    resolveInfos->GetSize(&size);
    Int32 len = size + 1; // +1 for the default choice we will insert.

    // Go through the list of discovered providers populating the data map
    // skip the provider we were instructed to ignore if there was one
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        resolveInfos->Get(i, (IInterface**)&obj);
        AutoPtr<IResolveInfo> ri= IResolveInfo::Probe(obj);

        AutoPtr<IActivityInfo> currentActivityInfo;
        ri->GetActivityInfo((IActivityInfo**)&currentActivityInfo);
        String key = MakeKeyForActivity(currentActivityInfo);
        if (key.Equals(providerToIgnore)) {
            if (DBG) Log(String("Ignoring key: ") + key);
            len--;
            continue;
        }
        if (DBG) Log(String("Loading key: ") + key);
        AutoPtr<ICharSequence> cchar;
        ri->LoadLabel(pm, (ICharSequence**)&cchar);
        String nameForDisplay;
        cchar->ToString(&nameForDisplay);
        AutoPtr<IIntent> providerIntent;
        CIntent::New((IIntent**)&providerIntent);
        providerIntent->SetAction(ACTION_CONFIGURE_VOICEMAIL);
        String packageName, name;
        IPackageItemInfo::Probe(currentActivityInfo)->GetPackageName(&packageName);
        IPackageItemInfo::Probe(currentActivityInfo)->GetName(&name);
        providerIntent->SetClassName(packageName, name);
        if (DBG) {
            StringBuilder sb;
            sb += "Store loaded VoiceMailProvider. key: ";
            sb += key;
            sb += " -> name: ";
            sb += nameForDisplay;
            sb += ", intent: ";
            sb += TO_CSTR(providerIntent);
            Log(sb.ToString());
        }
        AutoPtr<VoiceMailProvider> pro = new VoiceMailProvider(nameForDisplay, providerIntent);
        mVMProvidersData[key] = pro;
    }

    // Now we know which providers to display - create entries and values array for
    // the list preference
    AutoPtr<ArrayOf<ICharSequence*> > entries = ArrayOf<ICharSequence*>::Alloc(len);
    AutoPtr<ArrayOf<ICharSequence*> > values = ArrayOf<ICharSequence*>::Alloc(len);
    AutoPtr<ICharSequence> cs;
    CString::New(myCarrier, (ICharSequence**)&cs);
    entries->Set(0, cs);

    cs = NULL;
    CString::New(DEFAULT_VM_PROVIDER_KEY, (ICharSequence**)&cs);
    values->Set(0, cs);
    Int32 entryIdx = 1;
    Int32 tmp;
    resolveInfos->GetSize(&tmp);
    HashMap<String, AutoPtr<VoiceMailProvider> >::Iterator ator;
    for (Int32 i = 0; i < tmp; i++) {
        AutoPtr<IInterface> obj;
        resolveInfos->Get(i, (IInterface**)&obj);
        AutoPtr<IActivityInfo> activityInfo;
        IResolveInfo::Probe(obj)->GetActivityInfo((IActivityInfo**)&activityInfo);

        String key = MakeKeyForActivity(activityInfo);
        ator = mVMProvidersData.Find(key);
        if (ator == mVMProvidersData.End()) {
            continue;
        }

        cs = NULL;
        CString::New(ator->mSecond->mName, (ICharSequence**)&cs);
        entries->Set(entryIdx, cs);

        cs = NULL;
        CString::New(key, (ICharSequence**)&cs);
        values->Set(entryIdx, cs);
        entryIdx++;
    }

    // ListPreference is now updated.
    mVoicemailProviders->SetEntries(entries);
    mVoicemailProviders->SetEntryValues(values);

    // Remember the current Voicemail Provider key as a "previous" key. This will be used
    // when we fail to update Voicemail Provider, which requires rollback.
    // We will update this when the VM Provider setting is successfully updated.
    mPreviousVMProviderKey = GetCurrentVoicemailProviderKey();
    if (DBG) Log(String("Set up the first mPreviousVMProviderKey: ") + mPreviousVMProviderKey);

    // Finally update the preference texts.
    UpdateVMPreferenceWidgets(mPreviousVMProviderKey);
}

String CCallFeaturesSetting::MakeKeyForActivity(
    /* [in] */ IActivityInfo* ai)
{
    String name;
    IPackageItemInfo::Probe(ai)->GetName(&name);
    return name;
}

void CCallFeaturesSetting::SimulatePreferenceClick(
    /* [in] */ IPreference* preference)
{
    // Go through settings until we find our setting
    // and then simulate a click on it to bring up the dialog
    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    AutoPtr<IListAdapter> adapter;
    preferenceScreen->GetRootAdapter((IListAdapter**)&adapter);

    Int32 count;
    IAdapter::Probe(adapter)->GetCount(&count);
    for (Int32 idx = 0; idx < count; idx++) {
        AutoPtr<IInterface> obj;
        IAdapter::Probe(adapter)->GetItem(idx, (IInterface**)&obj);
        if (obj.Get() == TO_IINTERFACE(preference)) {
            AutoPtr<IListView> lv;
            GetListView((IListView**)&lv);
            Int64 id = 0;
            IAdapter::Probe(adapter)->GetItemId(idx, &id);
            IAdapterViewOnItemClickListener::Probe(preferenceScreen)->OnItemClick(IAdapterView::Probe(lv), NULL, idx, id);
            break;
        }
    }
}

void CCallFeaturesSetting::MaybeSaveSettingsForVoicemailProvider(
    /* [in] */ const String& key,
    /* [in] */ VoiceMailProviderSettings* newSettings)
{
    if (mVoicemailProviders == NULL) {
        return;
    }
    AutoPtr<VoiceMailProviderSettings> curSettings = LoadSettingsForVoiceMailProvider(key);
    Boolean tmp = FALSE;
    if (newSettings->Equals(curSettings->Probe(EIID_IInterface), &tmp), tmp) {
        if (DBG) {
            StringBuilder sb;
            sb += "maybeSaveSettingsForVoicemailProvider:";
            sb += " Not saving setting for ";
            sb += key;
            sb += " since they have not changed";
            Log(sb.ToString());
        }
        return;
    }
    if (DBG) {
        StringBuilder sb;
        sb += "Saving settings for ";
        sb += key;
        sb += ": ";
        String str;
        newSettings->ToString(&str);
        sb += str;
        Log(sb.ToString());
    }
    AutoPtr<ISharedPreferencesEditor> editor;
    mPerProviderSavedVMNumbers->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutString(key + VM_NUMBER_TAG, newSettings->mVoicemailNumber);
    String fwdKey = key + FWD_SETTINGS_TAG;
    AutoPtr<ArrayOf<ICallForwardInfo*> > s = newSettings->mForwardingSettings;
    if (s != FWD_SETTINGS_DONT_TOUCH) {
        editor->PutInt32(fwdKey + FWD_SETTINGS_LENGTH_TAG, s->GetLength());
        for (Int32 i = 0; i < s->GetLength(); i++) {
            String settingKey = fwdKey + FWD_SETTING_TAG + StringUtils::ToString(i);
            CallForwardInfo* fi = (CallForwardInfo*)(*s)[i];
            editor->PutInt32(settingKey + FWD_SETTING_STATUS, fi->mStatus);
            editor->PutInt32(settingKey + FWD_SETTING_REASON, fi->mReason);
            editor->PutString(settingKey + FWD_SETTING_NUMBER, fi->mNumber);
            editor->PutInt32(settingKey + FWD_SETTING_TIME, fi->mTimeSeconds);
        }
    }
    else {
        editor->PutInt32(fwdKey + FWD_SETTINGS_LENGTH_TAG, 0);
    }
    editor->Apply();
}

AutoPtr<CCallFeaturesSetting::VoiceMailProviderSettings> CCallFeaturesSetting::LoadSettingsForVoiceMailProvider(
    /* [in] */ const String& key)
{
    String vmNumberSetting;
    mPerProviderSavedVMNumbers->GetString(key + VM_NUMBER_TAG, String(NULL), &vmNumberSetting);
    if (vmNumberSetting.IsNull()) {
        Logger::W(TAG, "VoiceMailProvider settings for the key \"%s\""
                " was not found. Returning NULL.", key.string());
        return NULL;
    }

    AutoPtr<ArrayOf<ICallForwardInfo*> > cfi = FWD_SETTINGS_DONT_TOUCH;
    String fwdKey = key + FWD_SETTINGS_TAG;
    Int32 fwdLen;
    mPerProviderSavedVMNumbers->GetInt32(fwdKey + FWD_SETTINGS_LENGTH_TAG, 0, &fwdLen);
    if (fwdLen > 0) {
        cfi = ArrayOf<ICallForwardInfo*>::Alloc(fwdLen);
        for (Int32 i = 0; i < cfi->GetLength(); i++) {
            String settingKey = fwdKey + FWD_SETTING_TAG + StringUtils::ToString(i);
            AutoPtr<CallForwardInfo> info = new CallForwardInfo();
            cfi->Set(i, info);
            CallForwardInfo* item = (CallForwardInfo*)(*cfi)[i];
            mPerProviderSavedVMNumbers->GetInt32(settingKey + FWD_SETTING_STATUS, 0, &item->mStatus);
            mPerProviderSavedVMNumbers->GetInt32(settingKey + FWD_SETTING_REASON,
                    ICommandsInterface::CF_REASON_ALL_CONDITIONAL, &item->mReason);
            item->mServiceClass = ICommandsInterface::SERVICE_CLASS_VOICE;
            item->mToa = IPhoneNumberUtils::TOA_International;
            mPerProviderSavedVMNumbers->GetString(
                    settingKey + FWD_SETTING_NUMBER, String(""), &item->mNumber);
            mPerProviderSavedVMNumbers->GetInt32(settingKey + FWD_SETTING_TIME, 20, &item->mTimeSeconds);
        }
    }

    AutoPtr<VoiceMailProviderSettings> settings = new VoiceMailProviderSettings(vmNumberSetting, cfi);
    if (DBG) {
        String str;
        settings->ToString(&str);
        Log(String("Loaded settings for ") + key + ": " + str);
    }
    return settings;
}

void CCallFeaturesSetting::DeleteSettingsForVoicemailProvider(
    /* [in] */ const String& key)
{
    if (DBG) Log(String("Deleting settings for") + key);
    if (mVoicemailProviders == NULL) {
        return;
    }
    AutoPtr<ISharedPreferencesEditor> editor;
    mPerProviderSavedVMNumbers->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutString(key + VM_NUMBER_TAG, String(NULL));
    editor->PutInt32(key + FWD_SETTINGS_TAG + FWD_SETTINGS_LENGTH_TAG, 0);
    Boolean tmp = FALSE;
    editor->Commit(&tmp);
}

String CCallFeaturesSetting::GetCurrentVoicemailProviderKey()
{
    String key;
    mVoicemailProviders->GetValue(&key);
    return (!key.IsNull()) ? key : DEFAULT_VM_PROVIDER_KEY;
}

ECode CCallFeaturesSetting::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 itemId;
    item->GetItemId(&itemId);
    if (itemId == Elastos::Droid::R::id::home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
        OnBackPressed();
        *result = TRUE;
        return NOERROR;
    }
    return PreferenceActivity::OnOptionsItemSelected(item, result);
}

ECode CCallFeaturesSetting::GoUpToTopLevelSetting(
    /* [in] */ IActivity* activity)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IContext::Probe(activity), ECLSID_CCallFeaturesSetting, (IIntent**)&intent);
    intent->SetAction(IIntent::ACTION_MAIN);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    IContext::Probe(activity)->StartActivity(intent);
    return activity->Finish();
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos