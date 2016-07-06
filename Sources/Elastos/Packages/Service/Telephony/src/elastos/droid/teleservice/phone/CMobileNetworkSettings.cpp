
#include "elastos/droid/teleservice/phone/CMobileNetworkSettings.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/phone/CCdmaOptions.h"
#include "elastos/droid/teleservice/phone/CGsmUmtsOptions.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CMobileNetworkSettings::MyHandler::MyHandler(
    CMobileNetworkSettings* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CMobileNetworkSettings::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_GET_PREFERRED_NETWORK_TYPE:
            HandleGetPreferredNetworkTypeResponse(msg);
            break;

        case MESSAGE_SET_PREFERRED_NETWORK_TYPE:
            HandleSetPreferredNetworkTypeResponse(msg);
            break;
    }
    return NOERROR;
}

void CMobileNetworkSettings::MyHandler::HandleGetPreferredNetworkTypeResponse(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ar = (AsyncResult*)IObject::Probe(obj);

    if (ar->mException == NULL) {
        AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
        AutoPtr<IInterface> obj;
        array->Get(0, (IInterface**)&obj);
        AutoPtr<IInteger32> num = IInteger32::Probe(obj);
        Int32 modemNetworkMode;
        num->GetValue(&modemNetworkMode);

        if (DBG) {
            Log(String("handleGetPreferredNetworkTypeResponse: modemNetworkMode = ") +
                    StringUtils::ToString(modemNetworkMode));
        }

        AutoPtr<IContext> context;
        mHost->mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 settingsNetworkMode;
        helper->GetInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                sPreferredNetworkMode, &settingsNetworkMode);

        if (DBG) {
            Log(String("handleGetPreferredNetworkTypeReponse: settingsNetworkMode = ") +
                    StringUtils::ToString(settingsNetworkMode));
        }

        //check that modemNetworkMode is from an accepted value
        if (modemNetworkMode == IPhone::NT_MODE_WCDMA_PREF ||
                modemNetworkMode == IPhone::NT_MODE_GSM_ONLY ||
                modemNetworkMode == IPhone::NT_MODE_WCDMA_ONLY ||
                modemNetworkMode == IPhone::NT_MODE_GSM_UMTS ||
                modemNetworkMode == IPhone::NT_MODE_CDMA ||
                modemNetworkMode == IPhone::NT_MODE_CDMA_NO_EVDO ||
                modemNetworkMode == IPhone::NT_MODE_EVDO_NO_CDMA ||
                modemNetworkMode == IPhone::NT_MODE_GLOBAL ||
                modemNetworkMode == IPhone::NT_MODE_LTE_CDMA_AND_EVDO ||
                modemNetworkMode == IPhone::NT_MODE_LTE_GSM_WCDMA ||
                modemNetworkMode == IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA ||
                modemNetworkMode == IPhone::NT_MODE_LTE_ONLY ||
                modemNetworkMode == IPhone::NT_MODE_LTE_WCDMA) {
            if (DBG) {
                Log(String("handleGetPreferredNetworkTypeResponse: if 1: modemNetworkMode = ") +
                        StringUtils::ToString(modemNetworkMode));
            }

            //check changes in modemNetworkMode
            if (modemNetworkMode != settingsNetworkMode) {
                if (DBG) {
                    Log(String("handleGetPreferredNetworkTypeResponse: if 2: ") +
                            String("modemNetworkMode != settingsNetworkMode"));
                }

                settingsNetworkMode = modemNetworkMode;

                if (DBG) {
                    StringBuilder sb;
                    sb += "handleGetPreferredNetworkTypeResponse: if 2: ";
                    sb += "settingsNetworkMode = ";
                    sb += settingsNetworkMode;
                    Log(sb.ToString());
                }
            }

            mHost->UpdatePreferredNetworkModeSummary(modemNetworkMode);
            mHost->UpdateEnabledNetworksValueAndSummary(modemNetworkMode);
            // changes the mButtonPreferredNetworkMode accordingly to modemNetworkMode
            mHost->mButtonPreferredNetworkMode->SetValue(StringUtils::ToString(modemNetworkMode));
        }
        else {
            if (DBG) Log(String("handleGetPreferredNetworkTypeResponse: else: reset to default"));
            ResetNetworkModeToDefault();
        }
    }
}

void CMobileNetworkSettings::MyHandler::HandleSetPreferredNetworkTypeResponse(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ar = (AsyncResult*)IObject::Probe(obj);

    if (ar->mException == NULL) {
        String value;
        mHost->mButtonPreferredNetworkMode->GetValue(&value);
        Int32 networkMode = StringUtils::ParseInt32(value);

        AutoPtr<IContext> context;
        mHost->mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Boolean res;
        helper->PutInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                networkMode, &res);

        String value2;
        mHost->mButtonEnabledNetworks->GetValue(&value2);
        networkMode = StringUtils::ParseInt32(value2);

        helper->PutInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                networkMode, &res);
    }
    else {
        AutoPtr<IMessage> m;
        ObtainMessage(MESSAGE_GET_PREFERRED_NETWORK_TYPE, (IMessage**)&m);
        mHost->mPhone->GetPreferredNetworkType(m);
    }
}

void CMobileNetworkSettings::MyHandler::ResetNetworkModeToDefault()
{
    //set the mButtonPreferredNetworkMode
    mHost->mButtonPreferredNetworkMode->SetValue(StringUtils::ToString(sPreferredNetworkMode));
    mHost->mButtonEnabledNetworks->SetValue(StringUtils::ToString(sPreferredNetworkMode));

    //set the Settings.System
    AutoPtr<IContext> context;
    mHost->mPhone->GetContext((IContext**)&context);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);

    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Boolean res;
    helper->PutInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
            sPreferredNetworkMode, &res);

    //Set the Modem
    AutoPtr<IMessage> m;
    ObtainMessage(MyHandler::MESSAGE_SET_PREFERRED_NETWORK_TYPE, (IMessage**)&m);
    mHost->mPhone->SetPreferredNetworkType(sPreferredNetworkMode, m);
}

const String CMobileNetworkSettings::TAG("NetworkSettings");
const Boolean CMobileNetworkSettings::DBG = TRUE;

const String CMobileNetworkSettings::BUTTON_PREFERED_NETWORK_MODE("preferred_network_mode_key");
const String CMobileNetworkSettings::BUTTON_ROAMING_KEY("button_roaming_key");
const String CMobileNetworkSettings::BUTTON_CDMA_LTE_DATA_SERVICE_KEY("cdma_lte_data_service_key");
const String CMobileNetworkSettings::BUTTON_ENABLED_NETWORKS_KEY("enabled_networks_key");
const String CMobileNetworkSettings::BUTTON_4G_LTE_KEY("enhanced_4g_lte");
const String CMobileNetworkSettings::BUTTON_CELL_BROADCAST_SETTINGS("cell_broadcast_settings");

const Int32 CMobileNetworkSettings::sPreferredNetworkMode = IPhone::PREFERRED_NT_MODE;

const String CMobileNetworkSettings::UP_ACTIVITY_PACKAGE("com.android.settings");
const String CMobileNetworkSettings::UP_ACTIVITY_CLASS("com.android.settings.Settings$WirelessSettingsActivity");

const String CMobileNetworkSettings::sIface("rmnet0"); //TODO: this will go away

CAR_INTERFACE_IMPL_4(CMobileNetworkSettings, PreferenceActivity, IMobileNetworkSettings,
        IDialogInterfaceOnClickListener, IDialogInterfaceOnDismissListener,
        IPreferenceOnPreferenceChangeListener)

CAR_OBJECT_IMPL(CMobileNetworkSettings)

CMobileNetworkSettings::CMobileNetworkSettings()
    : mOkClicked(FALSE)
    , mShow4GForLTE(FALSE)
    , mIsGlobalCdma(FALSE)
    , mUnavailable(FALSE)
{
}

ECode CMobileNetworkSettings::constructor()
{
    return PreferenceActivity::constructor();
}

ECode CMobileNetworkSettings::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == IDialogInterface::BUTTON_POSITIVE) {
        mPhone->SetDataRoamingEnabled(TRUE);
        mOkClicked = TRUE;
    }
    else {
        // Reset the toggle
        ITwoStatePreference::Probe(mButtonDataRoam)->SetChecked(FALSE);
    }
    return NOERROR;
}

ECode CMobileNetworkSettings::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    // Assuming that onClick gets called first
    return ITwoStatePreference::Probe(mButtonDataRoam)->SetChecked(mOkClicked);
}

ECode CMobileNetworkSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    /** TODO: Refactor and get rid of the if's using subclasses */
    String key;
    preference->GetKey(&key);
    Boolean res;
    if (key.Equals(BUTTON_4G_LTE_KEY)) {
        *result = TRUE;
        return NOERROR;
    }
    else if (mGsmUmtsOptions != NULL &&
            (mGsmUmtsOptions->PreferenceTreeClick(preference, &res), res)) {
        *result = TRUE;
        return NOERROR;
    }
    else if (mCdmaOptions != NULL &&
            (mCdmaOptions->PreferenceTreeClick(preference, &res), res)) {
        AutoPtr<ISystemProperties> helper;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
        String obj;
        helper->Get(ITelephonyProperties::PROPERTY_INECM_MODE, &obj);
        if (StringUtils::ParseBoolean(obj)) {
            mClickedPreference = preference;

            // In ECM mode launch ECM app dialog
            AutoPtr<IIntent> intent;
            CIntent::New(ITelephonyIntents::ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS, NULL,
                    (IIntent**)&intent);
            StartActivityForResult(intent, REQUEST_CODE_EXIT_ECM);
        }
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonPreferredNetworkMode)) {
        //displays the value taken from the Settings.System
        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 settingsNetworkMode;
        helper->GetInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                sPreferredNetworkMode, &settingsNetworkMode);

        mButtonPreferredNetworkMode->SetValue(StringUtils::ToString(settingsNetworkMode));
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mLteDataServicePref)) {
        AutoPtr<IContentResolver> contentResolver;
        GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        String tmpl;
        helper->GetString(contentResolver, ISettingsGlobal::SETUP_PREPAID_DATA_SERVICE_URL,
                &tmpl);
        if (!TextUtils::IsEmpty(tmpl)) {
            AutoPtr<IInterface> obj;
            GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
            AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(obj);
            String imsi;
            tm->GetSubscriberId(&imsi);
            if (imsi.IsNull()) {
                imsi = String("");
            }
            String url;
            if (!TextUtils::IsEmpty(tmpl)) {
                AutoPtr<ICharSequence> tmplChar = CoreUtils::Convert(tmpl);
                AutoPtr<ArrayOf<ICharSequence*> > array = ArrayOf<ICharSequence*>::Alloc(1);
                AutoPtr<ICharSequence> imsiChar = CoreUtils::Convert(imsi);
                array->Set(0, imsiChar);
                AutoPtr<ICharSequence> cchar = TextUtils::ExpandTemplate(tmplChar, array);
                cchar->ToString(&url);
            }

            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            AutoPtr<IUri> _data;
            helper->Parse(url, (IUri**)&_data);
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_VIEW, _data, (IIntent**)&intent);
            StartActivity(intent);
        }
        else {
            Logger::E(TAG, "Missing SETUP_PREPAID_DATA_SERVICE_URL");
        }
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonEnabledNetworks)) {
        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 settingsNetworkMode;
        helper->GetInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                sPreferredNetworkMode, &settingsNetworkMode);

        mButtonEnabledNetworks->SetValue(StringUtils::ToString(settingsNetworkMode));
        *result = TRUE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonDataRoam)) {
        // Do not disable the preference screen if the user clicks Data roaming.
        *result = TRUE;
        return NOERROR;
    }
    else {
        // if the button is anything but the simple toggle preference,
        // we'll need to disable all preferences to reject all click
        // events until the sub-activity's UI comes up.
        IPreference::Probe(preferenceScreen)->SetEnabled(FALSE);
        // Let the intents be launched by the Preference manager
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

void CMobileNetworkSettings::SetIMS(
    /* [in] */ Boolean turnOn)
{
    AutoPtr<ISharedPreferences> imsPref;
    assert(0);
    // GetSharedPreferences(IImsManager::IMS_SHARED_PREFERENCES, IContext::MODE_WORLD_READABLE,
    //         (ISharedPreferences**)&imsPref);

    AutoPtr<ISharedPreferencesEditor> edit;
    imsPref->Edit((ISharedPreferencesEditor**)&edit);
    assert(0);
    //edit->PutBoolean(IImsManager::KEY_IMS_ON, turnOn);
    Boolean tmp;
    edit->Commit(&tmp);
}

ECode CMobileNetworkSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SetTheme(Elastos::Droid::TeleService::R::style::Theme_Material_Settings);
    PreferenceActivity::OnCreate(icicle);

    mPhone = PhoneGlobals::GetPhone();
    mHandler = new MyHandler(this);

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUm = IUserManager::Probe(obj);

    Boolean res;
    if (mUm->HasUserRestriction(IUserManager::DISALLOW_CONFIG_MOBILE_NETWORKS, &res), res) {
        mUnavailable = TRUE;
        SetContentView(Elastos::Droid::TeleService::R::layout::telephony_disallowed_preference_screen);
        return NOERROR;
    }

    AddPreferencesFromResource(Elastos::Droid::TeleService::R::xml::network_setting);

    AutoPtr<IPreference> preference;
    FindPreference(BUTTON_4G_LTE_KEY, (IPreference**)&preference);
    mButton4glte = ISwitchPreference::Probe(preference);

    IPreference::Probe(mButton4glte)->SetOnPreferenceChangeListener(this);
    assert(0);
    //mButton4glte->SetChecked(ImsManager::IsEnhanced4gLteModeSettingEnabledByUser(this));

    //try
    ECode ec = NOERROR;
    {
        AutoPtr<IContext> con;
        FAIL_GOTO(ec = CreatePackageContext(String("com.android.systemui"), 0, (IContext**)&con), ERROR)

        AutoPtr<IResources> resources;
        FAIL_GOTO(ec = con->GetResources((IResources**)&resources), ERROR)
        Int32 id;
        FAIL_GOTO(ec = resources->GetIdentifier(String("config_show4GForLTE"),
                String("bool"), String("com.android.systemui"), &id), ERROR)
        FAIL_GOTO(ec = resources->GetBoolean(id, &mShow4GForLTE), ERROR)
    }
    // catch (NameNotFoundException e) {
ERROR:
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Loge(String("NameNotFoundException for show4GFotLTE"));
        mShow4GForLTE = FALSE;
    }
    //}

    //get UI object references
    AutoPtr<IPreferenceScreen> prefSet;
    GetPreferenceScreen((IPreferenceScreen**)&prefSet);

    AutoPtr<IPreference> preference2;
    AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(BUTTON_ROAMING_KEY);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cchar2, (IPreference**)&preference2);
    mButtonDataRoam = ISwitchPreference::Probe(preference2);

    AutoPtr<IPreference> preference3;
    AutoPtr<ICharSequence> cchar3 = CoreUtils::Convert(BUTTON_PREFERED_NETWORK_MODE);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cchar3, (IPreference**)&preference3);
    mButtonPreferredNetworkMode = IListPreference::Probe(preference3);

    AutoPtr<IPreference> preference4;
    AutoPtr<ICharSequence> cchar4 = CoreUtils::Convert(BUTTON_ENABLED_NETWORKS_KEY);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cchar4, (IPreference**)&preference4);
    mButtonEnabledNetworks = IListPreference::Probe(preference4);

    IPreference::Probe(mButtonDataRoam)->SetOnPreferenceChangeListener(this);
    AutoPtr<ICharSequence> cchar5 = CoreUtils::Convert(BUTTON_CDMA_LTE_DATA_SERVICE_KEY);
    IPreferenceGroup::Probe(prefSet)->FindPreference(cchar5, (IPreference**)&mLteDataServicePref);

    Int32 mode;
    mPhone->GetLteOnCdmaMode(&mode);
    Boolean isLteOnCdma = mode == IPhoneConstants::LTE_ON_CDMA_TRUE;

    Int32 phoneId;
    mPhone->GetPhoneId(&phoneId);
    if (isLteOnCdma) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr< ArrayOf<Int32> > ar;
        resources->GetInt32Array(Elastos::Droid::TeleService::R::array::config_show_cdma, (ArrayOf<Int32>**)&ar);
        mIsGlobalCdma = (*ar)[phoneId] != 0;

    }
    else {
        mIsGlobalCdma = FALSE;
    }

    AutoPtr<IInterface> obj2;
    GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj2);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(obj2);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);

    AutoPtr<ISubscriptionManager> helper;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&helper);
    Int64 id;
    helper->GetDefaultSubId(&id);
    if (tm->GetSimplifiedNetworkSettingsEnabledForSubscriber(id, &res), res) {
        Boolean tmp;
        IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mButtonPreferredNetworkMode), &tmp);
        IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mButtonEnabledNetworks), &tmp);
        IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mLteDataServicePref), &tmp);
    }
    else if (resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::world_phone, &res), res) {
        Boolean tmp;
        IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mButtonEnabledNetworks), &tmp);
        // set the listener for the mButtonPreferredNetworkMode list preference so we can issue
        // change Preferred Network Mode.
        IPreference::Probe(mButtonPreferredNetworkMode)->SetOnPreferenceChangeListener(this);

        //Get the networkMode from Settings.System and displays it
        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 settingsNetworkMode;
        helper->GetInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                sPreferredNetworkMode, &settingsNetworkMode);

        mButtonPreferredNetworkMode->SetValue(StringUtils::ToString(settingsNetworkMode));
        CCdmaOptions::New(this, prefSet, mPhone, (ICdmaOptions**)&mCdmaOptions);
        CGsmUmtsOptions::New(this, prefSet, (IGsmUmtsOptions**)&mGsmUmtsOptions);
    }
    else {
        Boolean tmp;
        IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mButtonPreferredNetworkMode), &tmp);
        Int32 phoneType;
        mPhone->GetPhoneType(&phoneType);
        if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
            if (isLteOnCdma) {
                mButtonEnabledNetworks->SetEntries(
                        Elastos::Droid::TeleService::R::array::enabled_networks_cdma_choices);
                mButtonEnabledNetworks->SetEntryValues(
                        Elastos::Droid::TeleService::R::array::enabled_networks_cdma_values);
            }
            CCdmaOptions::New(this, prefSet, mPhone, (ICdmaOptions**)&mCdmaOptions);
        }
        else if (phoneType == IPhoneConstants::PHONE_TYPE_GSM) {
            Boolean res1, res2;
            if ((resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_prefer_2g, &res1), !res1)
                    && (resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_enabled_lte, &res2), !res2)) {
                mButtonEnabledNetworks->SetEntries(
                        Elastos::Droid::TeleService::R::array::enabled_networks_except_gsm_lte_choices);
                mButtonEnabledNetworks->SetEntryValues(
                        Elastos::Droid::TeleService::R::array::enabled_networks_except_gsm_lte_values);
            }
            else if ((resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_prefer_2g, &res1), !res1)) {
                Int32 select = (mShow4GForLTE == TRUE) ?
                    Elastos::Droid::TeleService::R::array::enabled_networks_except_gsm_4g_choices
                    : Elastos::Droid::TeleService::R::array::enabled_networks_except_gsm_choices;
                mButtonEnabledNetworks->SetEntries(select);
                mButtonEnabledNetworks->SetEntryValues(
                        Elastos::Droid::TeleService::R::array::enabled_networks_except_gsm_values);
            }
            else if ((resources->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_enabled_lte, &res1), !res1)) {
                mButtonEnabledNetworks->SetEntries(
                        Elastos::Droid::TeleService::R::array::enabled_networks_except_lte_choices);
                mButtonEnabledNetworks->SetEntryValues(
                        Elastos::Droid::TeleService::R::array::enabled_networks_except_lte_values);
            }
            else if (mIsGlobalCdma) {
                mButtonEnabledNetworks->SetEntries(
                        Elastos::Droid::TeleService::R::array::enabled_networks_cdma_choices);
                mButtonEnabledNetworks->SetEntryValues(
                        Elastos::Droid::TeleService::R::array::enabled_networks_cdma_values);
            }
            else {
                Int32 select = (mShow4GForLTE == TRUE) ? Elastos::Droid::TeleService::R::array::enabled_networks_4g_choices
                    : Elastos::Droid::TeleService::R::array::enabled_networks_choices;
                mButtonEnabledNetworks->SetEntries(select);
                mButtonEnabledNetworks->SetEntryValues(
                        Elastos::Droid::TeleService::R::array::enabled_networks_values);
            }
            CGsmUmtsOptions::New(this, prefSet, (IGsmUmtsOptions**)&mGsmUmtsOptions);
        }
        else {
            //throw new IllegalStateException("Unexpected phone type: " + phoneType);
            Logger::E(TAG, "Unexpected phone type: %d", phoneType);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        IPreference::Probe(mButtonEnabledNetworks)->IPreference::Probe((this));

        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 settingsNetworkMode;
        helper->GetInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                sPreferredNetworkMode, &settingsNetworkMode);

        if (DBG) Log(String("settingsNetworkMode: ") + StringUtils::ToString(settingsNetworkMode));
        mButtonEnabledNetworks->SetValue(StringUtils::ToString(settingsNetworkMode));
    }

    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);

    AutoPtr<ISettingsGlobal> helper2;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper2);
    String str;
    helper2->GetString(contentResolver, ISettingsGlobal::SETUP_PREPAID_DATA_SERVICE_URL, &str);
    Boolean missingDataServiceUrl = TextUtils::IsEmpty(str);
    if (!isLteOnCdma || missingDataServiceUrl) {
        Boolean tmp;
        IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(mLteDataServicePref), &tmp);
    }
    else {
        Logger::D(TAG, "keep ltePref");
    }

    // Enable enhanced 4G LTE mode settings depending on whether exists on platform
    assert(0);
    // if (!ImsManager::IsEnhanced4gLteModeSettingEnabledByPlatform(this)) {
    //     AutoPtr<IPreference> pref;
    //     IPreferenceGroup::Probe(prefSet)->FindPreference(BUTTON_4G_LTE_KEY, (IPreference**)&pref);
    //     if (pref != NULL) {
    //         Boolean tmp;
    //         IPreferenceGroup::Probe(prefSet)->RemovePreference(IPreference::Probe(pref), &tmp);
    //     }
    // }

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        // android.R.id.home will be triggered in onOptionsItemSelected()
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }

    AutoPtr<IUserHandleHelper> helper3;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper3);
    Int32 uid;
    helper3->GetMyUserId(&uid);
    Boolean isSecondaryUser = uid != IUserHandle::USER_OWNER;
    // Enable link to CMAS app settings depending on the value in config.xml.
    Boolean isCellBroadcastAppLinkEnabled;
    resources->GetBoolean(
            Elastos::Droid::R::bool_::config_cellBroadcastAppLinks, &isCellBroadcastAppLinkEnabled);
    Boolean tmp;
    if (isSecondaryUser || !isCellBroadcastAppLinkEnabled
            || (mUm->HasUserRestriction(IUserManager::DISALLOW_CONFIG_CELL_BROADCASTS, &tmp), tmp)) {
        AutoPtr<IPreferenceScreen> root;
        GetPreferenceScreen((IPreferenceScreen**)&root);
        AutoPtr<IPreference> ps;
        FindPreference(BUTTON_CELL_BROADCAST_SETTINGS, (IPreference**)&ps);
        if (ps != NULL) {
            Boolean tmp;
            IPreferenceGroup::Probe(root)->RemovePreference(ps, &tmp);
        }
    }
    return NOERROR;
}

ECode CMobileNetworkSettings::OnResume()
{
    PreferenceActivity::OnResume();

    if (mUnavailable) {
        return NOERROR;
    }

    // upon resumption from the sub-activity, make sure we re-enable the
    // preferences.
    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    IPreference::Probe(preferenceScreen)->SetEnabled(TRUE);

    // Set UI state in onResume because a user could go home, launch some
    // app to change this setting's backend, and re-launch this settings app
    // and the UI state would be inconsistent with actual state
    Boolean res;
    ITwoStatePreference::Probe(mButtonDataRoam)->SetChecked((mPhone->GetDataRoamingEnabled(&res), res));

    AutoPtr<IPreference> preference;
    AutoPtr<ICharSequence> cchar1 = CoreUtils::Convert(BUTTON_PREFERED_NETWORK_MODE);
    IPreferenceGroup::Probe(preferenceScreen)->FindPreference(cchar1, (IPreference**)&preference);
    if (preference != NULL)  {
        AutoPtr<IMessage> m;
        mHandler->ObtainMessage(
                MyHandler::MESSAGE_GET_PREFERRED_NETWORK_TYPE, (IMessage**)&m);
        mPhone->GetPreferredNetworkType(m);
    }

    AutoPtr<IPreference> preference2;
    AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(BUTTON_ENABLED_NETWORKS_KEY);
    IPreferenceGroup::Probe(preferenceScreen)->FindPreference(cchar2, (IPreference**)&preference2);
    if (preference2 != NULL)  {
        AutoPtr<IMessage> m;
        mHandler->ObtainMessage(
                MyHandler::MESSAGE_GET_PREFERRED_NETWORK_TYPE, (IMessage**)&m);
        mPhone->GetPreferredNetworkType(m);
    }
    return NOERROR;
}

ECode CMobileNetworkSettings::OnPause()
{
    return PreferenceActivity::OnPause();
}

ECode CMobileNetworkSettings::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* objValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonPreferredNetworkMode)) {
        //NOTE onPreferenceChange seems to be called even if there is no change
        //Check if the button value is changed from the System.Setting
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(objValue);
        String str;
        cchar->ToString(&str);
        mButtonPreferredNetworkMode->SetValue(str);
        Int32 buttonNetworkMode = StringUtils::ParseInt32(str);

        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 settingsNetworkMode;
        helper->GetInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                sPreferredNetworkMode, &settingsNetworkMode);

        if (buttonNetworkMode != settingsNetworkMode) {
            Int32 modemNetworkMode;
            // if new mode is invalid ignore it
            switch (buttonNetworkMode) {
                case IPhone::NT_MODE_WCDMA_PREF:
                case IPhone::NT_MODE_GSM_ONLY:
                case IPhone::NT_MODE_WCDMA_ONLY:
                case IPhone::NT_MODE_GSM_UMTS:
                case IPhone::NT_MODE_CDMA:
                case IPhone::NT_MODE_CDMA_NO_EVDO:
                case IPhone::NT_MODE_EVDO_NO_CDMA:
                case IPhone::NT_MODE_GLOBAL:
                case IPhone::NT_MODE_LTE_CDMA_AND_EVDO:
                case IPhone::NT_MODE_LTE_GSM_WCDMA:
                case IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                case IPhone::NT_MODE_LTE_ONLY:
                case IPhone::NT_MODE_LTE_WCDMA:
                    // This is one of the modes we recognize
                    modemNetworkMode = buttonNetworkMode;
                    break;
                default:
                    StringBuilder sb;
                    sb += "Invalid Network Mode (";
                    sb += buttonNetworkMode;
                    sb += ") chosen. Ignore.";
                    Loge(sb.ToString());
                    *result = TRUE;
                    return NOERROR;
            }

            UpdatePreferredNetworkModeSummary(buttonNetworkMode);

            AutoPtr<ISettingsGlobal> helper;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
            Boolean res;
            helper->PutInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                    buttonNetworkMode, &res);

            //Set the modem network mode
            AutoPtr<IMessage> m;
            mHandler->ObtainMessage(
                    MyHandler::MESSAGE_SET_PREFERRED_NETWORK_TYPE, (IMessage**)&m);
            mPhone->SetPreferredNetworkType(modemNetworkMode, m);
        }
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonEnabledNetworks)) {
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(objValue);
        String str;
        cchar->ToString(&str);

        mButtonEnabledNetworks->SetValue(str);
        Int32 buttonNetworkMode = StringUtils::ParseInt32(str);

        if (DBG) Log(String("buttonNetworkMode: ") + StringUtils::ToString(buttonNetworkMode));

        AutoPtr<IContext> context;
        mPhone->GetContext((IContext**)&context);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsGlobal> helper;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
        Int32 settingsNetworkMode;
        helper->GetInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                sPreferredNetworkMode, &settingsNetworkMode);

        if (buttonNetworkMode != settingsNetworkMode) {
            Int32 modemNetworkMode;
            // if new mode is invalid ignore it
            switch (buttonNetworkMode) {
                case IPhone::NT_MODE_WCDMA_PREF:
                case IPhone::NT_MODE_GSM_ONLY:
                case IPhone::NT_MODE_LTE_GSM_WCDMA:
                case IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                case IPhone::NT_MODE_CDMA:
                case IPhone::NT_MODE_CDMA_NO_EVDO:
                case IPhone::NT_MODE_LTE_CDMA_AND_EVDO:
                    // This is one of the modes we recognize
                    modemNetworkMode = buttonNetworkMode;
                    break;
                default:
                    StringBuilder sb;
                    sb += "Invalid Network Mode (";
                    sb += buttonNetworkMode;
                    sb += ") chosen. Ignore.";
                    Loge(sb.ToString());
                    *result = TRUE;
                    return NOERROR;
            }

            UpdateEnabledNetworksValueAndSummary(buttonNetworkMode);

            AutoPtr<ISettingsGlobal> helper;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
            Boolean res;
            helper->PutInt32(contentResolver, ISettingsGlobal::PREFERRED_NETWORK_MODE,
                    buttonNetworkMode, &res);

            //Set the modem network mode
            AutoPtr<IMessage> m;
            mHandler->ObtainMessage(
                    MyHandler::MESSAGE_SET_PREFERRED_NETWORK_TYPE, (IMessage**)&m);
            mPhone->SetPreferredNetworkType(modemNetworkMode, m);
        }
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButton4glte)) {
        AutoPtr<ISwitchPreference> ltePref = ISwitchPreference::Probe(preference);
        Boolean res;
        ITwoStatePreference::Probe(ltePref)->SetChecked((ITwoStatePreference::Probe(ltePref)->IsChecked(&res), !res));
        SetIMS((ITwoStatePreference::Probe(ltePref)->IsChecked(&res), res));

        assert(0);
        // AutoPtr<IImsManager> imsMan = ImsManager::GetInstance(getBaseContext(),
        //         SubscriptionManager::GetDefaultVoiceSubId());
        // if (imsMan != NULL) {
        //     //try {
        //     imsMan->SetAdvanced4GMode(ltePref->IsChecked(&res), res);
        //     //} catch (ImsException ie) {
        //         // do nothing
        //     //}
        // }
    }
    else if (TO_IINTERFACE(preference) == TO_IINTERFACE(mButtonDataRoam)) {
        if (DBG) Log(String("onPreferenceTreeClick: preference == mButtonDataRoam."));

        //normally called on the toggle click
        Boolean res;
        if ((ITwoStatePreference::Probe(mButtonDataRoam)->IsChecked(&res), !res)) {
            // First confirm with a warning dialog about charges
            mOkClicked = FALSE;
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            String str;
            resources->GetString(Elastos::Droid::TeleService::R::string::roaming_warning, &str);
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
            builder->SetMessage(cchar);
            builder->SetTitle(Elastos::Droid::R::string::dialog_alert_title);
            builder->SetIconAttribute(Elastos::Droid::R::attr::alertDialogIcon);
            builder->SetPositiveButton(Elastos::Droid::R::string::yes, this);
            builder->SetNegativeButton(Elastos::Droid::R::string::no, this);
            AutoPtr<IAlertDialog> dialog;
            builder->Show((IAlertDialog**)&dialog);
            builder->SetOnDismissListener(this);
        }
        else {
            mPhone->SetDataRoamingEnabled(FALSE);
        }
        *result = TRUE;
        return NOERROR;
    }

    // always let the preference setting proceed.
    *result = TRUE;
    return NOERROR;
}

void CMobileNetworkSettings::UpdatePreferredNetworkModeSummary(
    /* [in] */ Int32 NetworkMode)
{
    switch(NetworkMode) {
        case IPhone::NT_MODE_WCDMA_PREF:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_wcdma_perf_summary);
            break;
        case IPhone::NT_MODE_GSM_ONLY:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_gsm_only_summary);
            break;
        case IPhone::NT_MODE_WCDMA_ONLY:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_wcdma_only_summary);
            break;
        case IPhone::NT_MODE_GSM_UMTS:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_gsm_wcdma_summary);
            break;
        case IPhone::NT_MODE_CDMA:
            Int32 mode;
            mPhone->GetLteOnCdmaMode(&mode);
            switch (mode) {
                case IPhoneConstants::LTE_ON_CDMA_TRUE:
                    IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                        Elastos::Droid::TeleService::R::string::preferred_network_mode_cdma_summary);
                break;
                case IPhoneConstants::LTE_ON_CDMA_FALSE:
                default:
                    IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                        Elastos::Droid::TeleService::R::string::preferred_network_mode_cdma_evdo_summary);
                    break;
            }
            break;
        case IPhone::NT_MODE_CDMA_NO_EVDO:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_cdma_only_summary);
            break;
        case IPhone::NT_MODE_EVDO_NO_CDMA:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_evdo_only_summary);
            break;
        case IPhone::NT_MODE_LTE_ONLY:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_lte_summary);
            break;
        case IPhone::NT_MODE_LTE_GSM_WCDMA:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_lte_gsm_wcdma_summary);
            break;
        case IPhone::NT_MODE_LTE_CDMA_AND_EVDO:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_lte_cdma_evdo_summary);
            break;
        case IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_global_summary);
            break;
        case IPhone::NT_MODE_GLOBAL:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_cdma_evdo_gsm_wcdma_summary);
            break;
        case IPhone::NT_MODE_LTE_WCDMA:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_lte_wcdma_summary);
            break;
        default:
            IPreference::Probe(mButtonPreferredNetworkMode)->SetSummary(
                    Elastos::Droid::TeleService::R::string::preferred_network_mode_global_summary);
    }
}

void CMobileNetworkSettings::UpdateEnabledNetworksValueAndSummary(
    /* [in] */ Int32 NetworkMode)
{
    switch (NetworkMode) {
        case IPhone::NT_MODE_WCDMA_ONLY:
        case IPhone::NT_MODE_GSM_UMTS:
        case IPhone::NT_MODE_WCDMA_PREF:
            if (!mIsGlobalCdma) {
                mButtonEnabledNetworks->SetValue(
                        StringUtils::ToString(IPhone::NT_MODE_WCDMA_PREF));
                IPreference::Probe(mButtonEnabledNetworks)->SetSummary(Elastos::Droid::TeleService::R::string::network_3G);
            }
            else {
                mButtonEnabledNetworks->SetValue(
                        StringUtils::ToString(IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                IPreference::Probe(mButtonEnabledNetworks)->SetSummary(Elastos::Droid::TeleService::R::string::network_global);
            }
            break;
        case IPhone::NT_MODE_GSM_ONLY:
            if (!mIsGlobalCdma) {
                mButtonEnabledNetworks->SetValue(
                        StringUtils::ToString(IPhone::NT_MODE_GSM_ONLY));
                IPreference::Probe(mButtonEnabledNetworks)->SetSummary(Elastos::Droid::TeleService::R::string::network_2G);
            }
            else {
                mButtonEnabledNetworks->SetValue(
                        StringUtils::ToString(IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                IPreference::Probe(mButtonEnabledNetworks)->SetSummary(Elastos::Droid::TeleService::R::string::network_global);
            }
            break;
        case IPhone::NT_MODE_LTE_GSM_WCDMA:
        case IPhone::NT_MODE_LTE_ONLY:
        case IPhone::NT_MODE_LTE_WCDMA:
            if (!mIsGlobalCdma) {
                mButtonEnabledNetworks->SetValue(
                        StringUtils::ToString(IPhone::NT_MODE_LTE_GSM_WCDMA));
                IPreference::Probe(mButtonEnabledNetworks)->SetSummary((mShow4GForLTE == true)
                        ? Elastos::Droid::TeleService::R::string::network_4G :
                        Elastos::Droid::TeleService::R::string::network_lte);
            } else {
                mButtonEnabledNetworks->SetValue(
                        StringUtils::ToString(IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                IPreference::Probe(mButtonEnabledNetworks)->SetSummary(Elastos::Droid::TeleService::R::string::network_global);
            }
            break;
        case IPhone::NT_MODE_LTE_CDMA_AND_EVDO:
            mButtonEnabledNetworks->SetValue(
                    StringUtils::ToString(IPhone::NT_MODE_LTE_CDMA_AND_EVDO));
            IPreference::Probe(mButtonEnabledNetworks)->SetSummary(Elastos::Droid::TeleService::R::string::network_lte);
            break;
        case IPhone::NT_MODE_CDMA:
        case IPhone::NT_MODE_EVDO_NO_CDMA:
        case IPhone::NT_MODE_GLOBAL:
            mButtonEnabledNetworks->SetValue(
                    StringUtils::ToString(IPhone::NT_MODE_CDMA));
            IPreference::Probe(mButtonEnabledNetworks)->SetSummary(Elastos::Droid::TeleService::R::string::network_3G);
            break;
        case IPhone::NT_MODE_CDMA_NO_EVDO:
            mButtonEnabledNetworks->SetValue(
                    StringUtils::ToString(IPhone::NT_MODE_CDMA_NO_EVDO));
            IPreference::Probe(mButtonEnabledNetworks)->SetSummary(Elastos::Droid::TeleService::R::string::network_1x);
            break;
        case IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
            mButtonEnabledNetworks->SetValue(
                    StringUtils::ToString(IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
            IPreference::Probe(mButtonEnabledNetworks)->SetSummary(Elastos::Droid::TeleService::R::string::network_global);
            break;
        default:
        {
            StringBuilder sb;
            sb += "Invalid Network Mode (";
            sb += NetworkMode;
            sb += "). Ignore.";
            String errMsg = sb.ToString();
            Loge(errMsg);
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(errMsg);
            IPreference::Probe(mButtonEnabledNetworks)->SetSummary(cchar);
        }
    }
}

ECode CMobileNetworkSettings::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    switch(requestCode) {
        case REQUEST_CODE_EXIT_ECM:
            Boolean isChoiceYes;
            data->GetBooleanExtra(IEmergencyCallbackModeExitDialog::EXTRA_EXIT_ECM_RESULT,
                    FALSE, &isChoiceYes);
            if (isChoiceYes) {
                // If the phone exits from ECM mode, show the CDMA Options
                mCdmaOptions->ShowDialog(mClickedPreference);
            }
            else {
                // do nothing
            }
            break;

        default:
            break;
    }
    return NOERROR;
}

void CMobileNetworkSettings::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

void CMobileNetworkSettings::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(TAG, msg);
}

ECode CMobileNetworkSettings::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 itemId;
    item->GetItemId(&itemId);
    if (itemId == Elastos::Droid::R::id::home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
        // Commenting out "logical up" capability. This is a workaround for issue 5278083.
        //
        // Settings app may not launch this activity via UP_ACTIVITY_CLASS but the other
        // Activity that looks exactly same as UP_ACTIVITY_CLASS ("SubSettings" Activity).
        // At that moment, this Activity launches UP_ACTIVITY_CLASS on top of the Activity.
        // which confuses users.
        // TODO: introduce better mechanism for "up" capability here.
        /*Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.setClassName(UP_ACTIVITY_PACKAGE, UP_ACTIVITY_CLASS);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(intent);*/
        Finish();
        *result = TRUE;
        return NOERROR;
    }
    return PreferenceActivity::OnOptionsItemSelected(item, result);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
