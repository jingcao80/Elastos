
#include "elastos/droid/teleservice/phone/settings/PhoneAccountSettingsFragment.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipUtil.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipAccountRegistry.h"
#include "elastos/droid/R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <elastos/core/Object.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IDialogPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::Telecomm::Telecom::ITelecomManagerHelper;
using Elastos::Droid::Telecomm::Telecom::CTelecomManagerHelper;
using Elastos::Droid::TeleService::Services::Telephony::Sip::SipUtil;
using Elastos::Droid::TeleService::Services::Telephony::Sip::SipAccountRegistry;
using Elastos::Core::Object;
using Elastos::Core::CoreUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::IRunnable;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {
namespace Settings {

ECode PhoneAccountSettingsFragment::MyRunnable::Run()
{
    mHost->HandleSipReceiveCallsOption(mIsEnabled);
    return NOERROR;
}

static AutoPtr<IIntent> initCONNECTION_SERVICE_CONFIGURE_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ITelecomManager::ACTION_CONNECTION_SERVICE_CONFIGURE, (IIntent**)&intent);
    intent->AddCategory(IIntent::CATEGORY_DEFAULT);
    return intent;
}

const AutoPtr<IIntent> PhoneAccountSettingsFragment::CONNECTION_SERVICE_CONFIGURE_INTENT = initCONNECTION_SERVICE_CONFIGURE_INTENT();

const String PhoneAccountSettingsFragment::DEFAULT_OUTGOING_ACCOUNT_KEY("default_outgoing_account");

const String PhoneAccountSettingsFragment::CONFIGURE_CALL_ASSISTANT_PREF_KEY("wifi_calling_configure_call_assistant_preference");
const String PhoneAccountSettingsFragment::CALL_ASSISTANT_CATEGORY_PREF_KEY("phone_accounts_call_assistant_settings_category_key");
const String PhoneAccountSettingsFragment::SELECT_CALL_ASSISTANT_PREF_KEY("wifi_calling_call_assistant_preference");

const String PhoneAccountSettingsFragment::SIP_SETTINGS_CATEGORY_PREF_KEY("phone_accounts_sip_settings_category_key");
const String PhoneAccountSettingsFragment::USE_SIP_PREF_KEY("use_sip_calling_options_key");
const String PhoneAccountSettingsFragment::SIP_RECEIVE_CALLS_PREF_KEY("sip_receive_calls_key");

CAR_INTERFACE_IMPL_3(PhoneAccountSettingsFragment, PreferenceFragment, IPreferenceOnPreferenceChangeListener,
        IPreferenceOnPreferenceClickListener, IAccountSelectionListener)

PhoneAccountSettingsFragment::PhoneAccountSettingsFragment()
    : TAG("PhoneAccountSettingsFragment")
{
}

ECode PhoneAccountSettingsFragment::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceFragment::OnCreate(icicle);

    AutoPtr<ITelecomManagerHelper> helper;
    CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    return helper->From(IContext::Probe(activity), (ITelecomManager**)&mTelecomManager);
}

ECode PhoneAccountSettingsFragment::OnResume()
{
    PreferenceFragment::OnResume();

    AutoPtr<IPreferenceScreen> preferencescreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferencescreen);
    if (preferencescreen != NULL) {
        IPreferenceGroup::Probe(preferencescreen)->RemoveAll();
    }

    assert(0);
    //AddPreferencesFromResource(Elastos::Droid::R::xml::phone_account_settings);

    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(DEFAULT_OUTGOING_ACCOUNT_KEY);
    AutoPtr<IPreference> preference;
    IPreferenceGroup::Probe(preferencescreen)->FindPreference(cchar, (IPreference**)&preference);
    mDefaultOutgoingAccount = IAccountSelectionPreference::Probe(preference);

    Int32 count;
    mTelecomManager->GetAllPhoneAccountsCount(&count);
    if (count > 1) {
        mDefaultOutgoingAccount->SetListener(this);
        UpdateDefaultOutgoingAccountsModel();
    }
    else {
        Boolean res;
        IPreferenceGroup::Probe(preferencescreen)->RemovePreference(
                IPreference::Probe(mDefaultOutgoingAccount), &res);
    }

    AutoPtr<IList> list;
    mTelecomManager->GetSimCallManagers((IList**)&list);
    Boolean res;
    list->IsEmpty(&res);
    if (!res) {
        AutoPtr<IPreference> preference;
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(SELECT_CALL_ASSISTANT_PREF_KEY);
        IPreferenceGroup::Probe(preferencescreen)->FindPreference(cchar, (IPreference**)&preference);
        mSelectCallAssistant = IAccountSelectionPreference::Probe(preference);

        mSelectCallAssistant->SetListener(this);
        IDialogPreference::Probe(mSelectCallAssistant)->SetDialogTitle(
                R::string::wifi_calling_select_call_assistant_summary);

        AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(CONFIGURE_CALL_ASSISTANT_PREF_KEY);
        IPreferenceGroup::Probe(preferencescreen)->FindPreference(cchar2, (IPreference**)&mConfigureCallAssistant);
        mConfigureCallAssistant->SetOnPreferenceClickListener(this);
        UpdateCallAssistantModel();
    }
    else {
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(CALL_ASSISTANT_CATEGORY_PREF_KEY);
        AutoPtr<IPreference> preference;
        IPreferenceGroup::Probe(preferencescreen)->FindPreference(cchar, (IPreference**)&preference);
        Boolean res;
        IPreferenceGroup::Probe(preferencescreen)->RemovePreference(preference, &res);
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (SipUtil::IsVoipSupported(IContext::Probe(activity))) {
        mSipSharedPreferences = new SipSharedPreferences(IContext::Probe(activity));

        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(USE_SIP_PREF_KEY);
        AutoPtr<IPreference> preference;
        IPreferenceGroup::Probe(preferencescreen)->FindPreference(cchar, (IPreference**)&preference);
        mUseSipCalling = IListPreference::Probe(preference);
        assert(0);
        // mUseSipCalling->SetEntries(!SipManager.isSipWifiOnly(activity)
        //         ? R::array::sip_call_options_wifi_only_entries
        //         : R::array::sip_call_options_entries);
        IPreference::Probe(mUseSipCalling)->SetOnPreferenceChangeListener((IPreferenceOnPreferenceChangeListener*)this);

        String option;
        mSipSharedPreferences->GetSipCallOption(&option);
        Int32 optionsValueIndex;
        mUseSipCalling->FindIndexOfValue(option, &optionsValueIndex);
        if (optionsValueIndex == -1) {
            // If the option is invalid (eg. deprecated value), default to SIP_ADDRESS_ONLY.
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            String str;
            resources->GetString(R::string::sip_address_only, &str);
            mSipSharedPreferences->SetSipCallOption(str);
            mSipSharedPreferences->GetSipCallOption(&option);
            mUseSipCalling->FindIndexOfValue(option, &optionsValueIndex);
        }
        mUseSipCalling->SetValueIndex(optionsValueIndex);
        AutoPtr<ICharSequence> entry;
        mUseSipCalling->GetEntry((ICharSequence**)&entry);
        IPreference::Probe(mUseSipCalling)->SetSummary(entry);

        AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(SIP_RECEIVE_CALLS_PREF_KEY);
        AutoPtr<IPreference> preference2;
        IPreferenceGroup::Probe(preferencescreen)->FindPreference(cchar2, (IPreference**)&preference2);
        mSipReceiveCallsPreference = ICheckBoxPreference::Probe(preference2);
        IPreference::Probe(mSipReceiveCallsPreference)->SetEnabled(SipUtil::IsPhoneIdle(IContext::Probe(activity)));
        Boolean res;
        ITwoStatePreference::Probe(mSipReceiveCallsPreference)->SetChecked((
                mSipSharedPreferences->IsReceivingCallsEnabled(&res), res));
        IPreference::Probe(mSipReceiveCallsPreference)->SetOnPreferenceChangeListener(
                (IPreferenceOnPreferenceChangeListener*)this);
    }
    else {
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(SIP_SETTINGS_CATEGORY_PREF_KEY);
        AutoPtr<IPreference> preference;
        IPreferenceGroup::Probe(preferencescreen)->FindPreference(cchar, (IPreference**)&preference);
        Boolean res;
        IPreferenceGroup::Probe(preferencescreen)->RemovePreference(preference, &res);
    }
    return NOERROR;
}

ECode PhoneAccountSettingsFragment::OnPreferenceChange(
    /* [in] */ IPreference* pref,
    /* [in] */ IInterface* objValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(pref) == TO_IINTERFACE(mUseSipCalling)) {
        String option;
        IObject::Probe(objValue)->ToString(&option);
        mSipSharedPreferences->SetSipCallOption(option);

        Int32 index;
        mUseSipCalling->FindIndexOfValue(option, &index);
        mUseSipCalling->SetValueIndex(index);
        AutoPtr<ICharSequence> cchar;
        mUseSipCalling->GetEntry((ICharSequence**)&cchar);
        IPreference::Probe(mUseSipCalling)->SetSummary(cchar);
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(pref) == TO_IINTERFACE(mSipReceiveCallsPreference)) {
        Boolean isEnabled;
        ITwoStatePreference::Probe(mSipReceiveCallsPreference)->IsChecked(&isEnabled);
        isEnabled = !isEnabled;

        AutoPtr<IRunnable> r = new MyRunnable(this, isEnabled);
        AutoPtr<IThread> t;
        CThread::New(r, (IThread**)&t);
        t->Start();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode PhoneAccountSettingsFragment::OnPreferenceClick(
    /* [in] */ IPreference* pref,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(pref) == TO_IINTERFACE(mConfigureCallAssistant)) {
        //try {
        ECode ec = StartActivity(CONNECTION_SERVICE_CONFIGURE_INTENT);
        //} catch (ActivityNotFoundException e) {
        if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
            Logger::D(TAG, "Could not resolve telecom connection service configure intent.");
        }
        //}
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode PhoneAccountSettingsFragment::OnAccountSelected(
    /* [in] */ IAccountSelectionPreference* pref,
    /* [in] */ IPhoneAccountHandle* account,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(pref) == TO_IINTERFACE(mDefaultOutgoingAccount)) {
        mTelecomManager->SetUserSelectedOutgoingPhoneAccount(account);
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(pref) == TO_IINTERFACE(mSelectCallAssistant)) {
        mTelecomManager->SetSimCallManager(account);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode PhoneAccountSettingsFragment::OnAccountSelectionDialogShow(
    /* [in] */ IAccountSelectionPreference* pref)
{
    if (TO_IINTERFACE(pref) == TO_IINTERFACE(mDefaultOutgoingAccount)) {
        UpdateDefaultOutgoingAccountsModel();
    }
    else if (TO_IINTERFACE(pref) == TO_IINTERFACE(mSelectCallAssistant)) {
        UpdateCallAssistantModel();
    }
    return NOERROR;
}

ECode PhoneAccountSettingsFragment::OnAccountChanged(
    /* [in] */ IAccountSelectionPreference* pref)
{
    if (TO_IINTERFACE(pref) == TO_IINTERFACE(mSelectCallAssistant)) {
        UpdateConfigureCallAssistantSummary();
    }
    return NOERROR;
}

void PhoneAccountSettingsFragment::HandleSipReceiveCallsOption(
    /* [in] */ Boolean isEnabled)
{
    AutoLock syncLock(this);

    mSipSharedPreferences->SetReceivingCallsEnabled(isEnabled);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    SipUtil::UseSipToReceiveIncomingCalls(IContext::Probe(activity), isEnabled);

    // Restart all Sip services to ensure we reflect whether we are receiving calls.
    AutoPtr<SipAccountRegistry> sipAccountRegistry = SipAccountRegistry::GetInstance();
    sipAccountRegistry->RestartSipService(IContext::Probe(activity));
}

void PhoneAccountSettingsFragment::UpdateDefaultOutgoingAccountsModel()
{
    AutoPtr<IList> list;
    mTelecomManager->GetCallCapablePhoneAccounts((IList**)&list);
    AutoPtr<IPhoneAccountHandle> handle;
    mTelecomManager->GetUserSelectedOutgoingPhoneAccount((IPhoneAccountHandle**)&handle);
    String str;
    GetString(R::string::phone_accounts_ask_every_time, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    mDefaultOutgoingAccount->SetModel(mTelecomManager, list, handle, cchar);
}

ECode PhoneAccountSettingsFragment::UpdateCallAssistantModel()
{
    AutoPtr<IList> simCallManagers;
    mTelecomManager->GetSimCallManagers((IList**)&simCallManagers);
    AutoPtr<IPhoneAccountHandle> handle;
    mTelecomManager->GetSimCallManager((IPhoneAccountHandle**)&handle);

    String str;
    GetString(R::string::wifi_calling_call_assistant_none, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    mSelectCallAssistant->SetModel(mTelecomManager, simCallManagers, handle, cchar);

    UpdateConfigureCallAssistantSummary();
    return NOERROR;
}

void PhoneAccountSettingsFragment::UpdateConfigureCallAssistantSummary()
{
    AutoPtr<ArrayOf<ICharSequence*> > array;
    IListPreference::Probe(mSelectCallAssistant)->GetEntries((ArrayOf<ICharSequence*>**)&array);

    String value;
    IListPreference::Probe(mSelectCallAssistant)->GetValue(&value);
    Int32 index;
    IListPreference::Probe(mSelectCallAssistant)->FindIndexOfValue(value, &index);

    if (array->GetLength() - 1 == index) {
        mConfigureCallAssistant->SetSummary(R::string::wifi_calling_call_assistant_configure_no_selection);
        mConfigureCallAssistant->SetEnabled(FALSE);
    }
    else {
        mConfigureCallAssistant->SetSummary((ICharSequence*)NULL);
        mConfigureCallAssistant->SetEnabled(TRUE);
    }
}

} // namespace Settings
} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos