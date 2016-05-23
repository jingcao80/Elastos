/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_DEFAULT_SUBSCRIPTION;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::ILocalServerSocket;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Telephony::Cdma::ICDMALTEPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManager;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhoneFactory;
using Elastos::Droid::Internal::Telephony::Sip::ISipPhone;
using Elastos::Droid::Internal::Telephony::Sip::ISipPhoneFactory;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Lang::Reflect::IConstructor;

/**
 * {@hide}
 */
public class PhoneFactory {
    static const String LOG_TAG = "PhoneFactory";
    static const Int32 SOCKET_OPEN_RETRY_MILLIS = 2 * 1000;
    static const Int32 SOCKET_OPEN_MAX_RETRY = 3;

    //***** Class Variables

    // lock sLockProxyPhones protects both sProxyPhones and sProxyPhone
    static const Object sLockProxyPhones = new Object();
    static private PhoneProxy[] sProxyPhones = NULL;
    static private PhoneProxy sProxyPhone = NULL;

    static private CommandsInterface[] sCommandsInterfaces = NULL;

    static private ProxyController mProxyController;
    static private UiccController mUiccController;

    static private CommandsInterface sCommandsInterface = NULL;
    static private SubInfoRecordUpdater sSubInfoRecordUpdater = NULL;
    static private ModemBindingPolicyHandler sModemBindingPolicyHandler;
    static private ModemStackController sModemStackController;

    static private Boolean sMadeDefaults = FALSE;
    static private PhoneNotifier sPhoneNotifier;
    static private Context sContext;

    //***** Class Methods

    public static void MakeDefaultPhones(Context context) {
        MakeDefaultPhone(context);
    }

    /**
     * FIXME replace this with some other way of making these
     * instances
     */
    public static void MakeDefaultPhone(Context context) {
        {    AutoLock syncLock(sLockProxyPhones);
            If (!sMadeDefaults) {
                sContext = context;

                // create the telephony device controller.
                TelephonyDevController->Create();

                Int32 retryCount = 0;
                For(;;) {
                    Boolean hasException = FALSE;
                    retryCount ++;

                    try {
                        // use UNIX domain socket to
                        // prevent subsequent initialization
                        new LocalServerSocket("com.android.internal.telephony");
                    } Catch (java.io.IOException ex) {
                        hasException = TRUE;
                    }

                    If ( !hasException ) {
                        break;
                    } else If (retryCount > SOCKET_OPEN_MAX_RETRY) {
                        throw new RuntimeException("PhoneFactory probably already running");
                    } else {
                        try {
                            Thread->Sleep(SOCKET_OPEN_RETRY_MILLIS);
                        } Catch (InterruptedException er) {
                        }
                    }
                }

                sPhoneNotifier = new DefaultPhoneNotifier();

                // Get preferred network mode
                Int32 preferredNetworkMode = RILConstants.PREFERRED_NETWORK_MODE;
                If (TelephonyManager->GetLteOnCdmaModeStatic() == PhoneConstants.LTE_ON_CDMA_TRUE) {
                    preferredNetworkMode = Phone.NT_MODE_GLOBAL;
                }
                If (TelephonyManager->GetLteOnGsmModeStatic() != 0) {
                    preferredNetworkMode = Phone.NT_MODE_LTE_GSM_WCDMA;
                }

                Int32 cdmaSubscription = CdmaSubscriptionSourceManager->GetDefault(context);
                Rlog->I(LOG_TAG, "Cdma Subscription set to " + cdmaSubscription);

                /* In case of multi SIM mode two instances of PhoneProxy, RIL are created,
                   where as in single SIM mode only instance. IsMultiSimEnabled() function checks
                   whether it is single SIM or multi SIM mode */
                Int32 numPhones = TelephonyManager->GetDefault()->GetPhoneCount();
                Int32[] networkModes = new Int32[numPhones];
                sProxyPhones = new PhoneProxy[numPhones];
                sCommandsInterfaces = new RIL[numPhones];
                String sRILClassname = SystemProperties->Get("ro.telephony.ril_class", "RIL").Trim();
                Rlog->I(LOG_TAG, "RILClassname is " + sRILClassname);

                For (Int32 i = 0; i < numPhones; i++) {
                    //reads the system properties and makes commandsinterface
                    try {
//                        // Get preferred network type.
//                        TODO: Sishir added this code to but we need a new technique for MSim
//                        Int32 networkType = CalculatePreferredNetworkType(context);
//                        Rlog->I(LOG_TAG, "Network Type set to " + Integer->ToString(networkType));

                        networkModes[i]  = TelephonyManager->GetIntAtIndex(
                                context->GetContentResolver(),
                                Settings::Global::PREFERRED_NETWORK_MODE, i);
                    } Catch (SettingNotFoundException snfe) {
                        Rlog->E(LOG_TAG, "Settings Exception Reading Value At Index for"+
                                " Settings::Global::PREFERRED_NETWORK_MODE");
                        networkModes[i] = preferredNetworkMode;
                    }

                    If (sContext->GetResources()->GetBoolean(
                            R.bool.config_global_phone_enabled) &&
                            i == PhoneConstants.PHONE_ID1) {
                        networkModes[i] = Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA;
                        TelephonyManager->PutIntAtIndex( context->GetContentResolver(),
                                Settings::Global::PREFERRED_NETWORK_MODE, i, networkModes[i]);
                    }

                    Rlog->I(LOG_TAG, "Network Mode set to " + Integer->ToString(networkModes[i]));
                    // Use reflection to construct the RIL Class (defaults to RIL)
                    try {
                        sCommandsInterfaces[i] = InstantiateCustomRIL(
                                                     sRILClassname, context, networkModes[i], cdmaSubscription, i);
                    } Catch (Exception e) {
                        // 6 different types of exceptions are thrown here that it's
                        // easier to just catch Exception as our "error handling" is the same.
                        // Yes, we're blocking the whole thing and making the radio unusable. That's by design.
                        // The log message should make it clear why the radio is broken
                        While (TRUE) {
                            Rlog->E(LOG_TAG, "Unable to construct custom RIL class", e);
                            try {Thread->Sleep(10000);} Catch (InterruptedException ie) {}
                        }
                    }
                }
                Rlog->I(LOG_TAG, "Creating SubscriptionController");
                SubscriptionController->Init(context, sCommandsInterfaces);

                // Instantiate UiccController so that all other classes can just
                // call GetInstance()
                mUiccController = UiccController->Make(context, sCommandsInterfaces);
                sModemStackController = ModemStackController->Make(context,
                        mUiccController, sCommandsInterfaces);
                sModemBindingPolicyHandler = ModemBindingPolicyHandler->Make(context,
                        mUiccController, sCommandsInterfaces);

                For (Int32 i = 0; i < numPhones; i++) {
                    PhoneBase phone = NULL;
                    Int32 phoneType = TelephonyManager->GetPhoneType(networkModes[i]);
                    If (phoneType == PhoneConstants.PHONE_TYPE_GSM) {
                        phone = new GSMPhone(context,
                                sCommandsInterfaces[i], sPhoneNotifier, i);
                    } else If (phoneType == PhoneConstants.PHONE_TYPE_CDMA) {
                        phone = new CDMALTEPhone(context,
                                sCommandsInterfaces[i], sPhoneNotifier, i);
                    }
                    Rlog->I(LOG_TAG, "Creating Phone with type = " + phoneType + " sub = " + i);

                    sProxyPhones[i] = new PhoneProxy(phone);
                }
                mProxyController = ProxyController->GetInstance(context, sProxyPhones,
                        mUiccController, sCommandsInterfaces);

                // Set the default phone in base class.
                // FIXME: This is a first best guess at what the defaults will be. It
                // FIXME: needs to be done in a more controlled manner in the future.
                sProxyPhone = sProxyPhones[0];
                sCommandsInterface = sCommandsInterfaces[0];

                // Ensure that we have a default SMS app. Requesting the app with
                // updateIfNeeded set to TRUE is enough to configure a default SMS app.
                ComponentName componentName =
                        SmsApplication->GetDefaultSmsApplication(context, TRUE /* updateIfNeeded */);
                String packageName = "NONE";
                If (componentName != NULL) {
                    packageName = componentName->GetPackageName();
                }
                Rlog->I(LOG_TAG, "defaultSmsApplication: " + packageName);

                // Set up monitor to watch for changes to SMS packages
                SmsApplication->InitSmsPackageMonitor(context);

                sMadeDefaults = TRUE;

                Rlog->I(LOG_TAG, "Creating SubInfoRecordUpdater ");
                sSubInfoRecordUpdater = new SubInfoRecordUpdater(context,
                        sProxyPhones, sCommandsInterfaces);
                SubscriptionController->GetInstance()->UpdatePhonesAvailability(sProxyPhones);
            }
        }
    }

    public static Phone GetCdmaPhone(Int32 phoneId) {
        Phone phone;
        {    AutoLock syncLock(PhoneProxy.lockForRadioTechnologyChange);
            phone = new CDMALTEPhone(sContext, sCommandsInterfaces[phoneId],
                    sPhoneNotifier, phoneId);
        }
        return phone;
    }

    public static Phone GetGsmPhone(Int32 phoneId) {
        {    AutoLock syncLock(PhoneProxy.lockForRadioTechnologyChange);
            Phone phone = new GSMPhone(sContext, sCommandsInterfaces[phoneId],
                    sPhoneNotifier, phoneId);
            return phone;
        }
    }

    private static <T> T InstantiateCustomRIL(
                      String sRILClassname, Context context, Int32 networkMode, Int32 cdmaSubscription, Integer instanceId)
                      throws Exception {
        Class<?> clazz = Class->ForName("com.android.internal.telephony." + sRILClassname);
        Constructor<?> constructor = clazz->GetConstructor(Context.class, Int32.class, Int32.class, Integer.class);
        Return (T) clazz->Cast(constructor->NewInstance(context, networkMode, cdmaSubscription, instanceId));
    }

    public static Phone GetDefaultPhone() {
        {    AutoLock syncLock(sLockProxyPhones);
            If (!sMadeDefaults) {
                throw new IllegalStateException("Default phones haven't been made yet!");
            }
            return sProxyPhone;
        }
    }

    public static Phone GetPhone(Int32 phoneId) {
        Phone phone;
        {    AutoLock syncLock(sLockProxyPhones);
            If (!sMadeDefaults) {
                throw new IllegalStateException("Default phones haven't been made yet!");
                // CAF_MSIM FIXME need to introduce default phone id ?
            } else If (phoneId == SubscriptionManager.DEFAULT_PHONE_ID) {
                Rlog->D(LOG_TAG, "getPhone: phoneId == DEFAULT_PHONE_ID");
                phone = sProxyPhone;
            } else {
                Rlog->D(LOG_TAG, "getPhone: phoneId != DEFAULT_PHONE_ID");
                phone = (((phoneId >= 0)
                                && (phoneId < TelephonyManager->GetDefault()->GetPhoneCount()))
                        ? sProxyPhones[phoneId] : NULL);
            }
            Rlog->D(LOG_TAG, "getPhone:- phone=" + phone);
            return phone;
        }
    }

    public static Phone[] GetPhones() {
        {    AutoLock syncLock(sLockProxyPhones);
            If (!sMadeDefaults) {
                throw new IllegalStateException("Default phones haven't been made yet!");
            }
            return sProxyPhones;
        }
    }

    public static Phone GetCdmaPhone() {
        If (!sMadeDefaults) {
            throw new IllegalStateException("Default phones haven't been made yet!");
        }
        Phone phone;
        {    AutoLock syncLock(PhoneProxy.lockForRadioTechnologyChange);
            Switch (TelephonyManager->GetLteOnCdmaModeStatic()) {
                case PhoneConstants.LTE_ON_CDMA_TRUE: {
                    phone = new CDMALTEPhone(sContext, sCommandsInterface, sPhoneNotifier);
                    break;
                }
                case PhoneConstants.LTE_ON_CDMA_FALSE:
                case PhoneConstants.LTE_ON_CDMA_UNKNOWN:
                default: {
                    phone = new CDMAPhone(sContext, sCommandsInterface, sPhoneNotifier);
                    break;
                }
            }
        }
        return phone;
    }

    public static Phone GetGsmPhone() {
        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(GetDefaultSubscription());
        If (phoneId < 0 || phoneId >= TelephonyManager->GetDefault()->GetPhoneCount()) {
            phoneId = 0;
        }
        return GetGsmPhone(phoneId);
    }

    public static Context GetContext() {
        return sContext;
    }

    /**
     * Makes a {@link SipPhone} object.
     * @param sipUri the local SIP URI the phone runs on
     * @return the {@code SipPhone} object or NULL if the SIP URI is not valid
     */
    public static SipPhone MakeSipPhone(String sipUri) {
        return SipPhoneFactory->MakePhone(sipUri, sContext, sPhoneNotifier);
    }

    /* Sets the default subscription. If only one phone instance is active that
     * subscription is set as default subscription. If both phone instances
     * are active the first instance "0" is set as default subscription
     */
    public static void SetDefaultSubscription(Int32 subId) {
        SystemProperties->Set(PROPERTY_DEFAULT_SUBSCRIPTION, Integer->ToString(subId));
        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);

        {    AutoLock syncLock(sLockProxyPhones);
            // Set the default phone in base class
            If (phoneId >= 0 && phoneId < sProxyPhones.length) {
                sProxyPhone = sProxyPhones[phoneId];
                sCommandsInterface = sCommandsInterfaces[phoneId];
                sMadeDefaults = TRUE;
            }
        }

        // Update MCC MNC device configuration information
        String defaultMccMnc = TelephonyManager->GetDefault()->GetSimOperator(phoneId);
        Rlog->D(LOG_TAG, "update mccmnc=" + defaultMccMnc);
        MccTable->UpdateMccMncConfiguration(sContext, defaultMccMnc, FALSE);

        // Broadcast an Intent for default sub change
        Intent intent = new Intent(TelephonyIntents.ACTION_DEFAULT_SUBSCRIPTION_CHANGED);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, phoneId);
        Rlog->D(LOG_TAG, "setDefaultSubscription : " + subId
                + " Broadcasting Default Subscription Changed...");
        sContext->SendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    public static Int32 CalculatePreferredNetworkType(Context context) {
        return CalculatePreferredNetworkType(context, GetDefaultPhoneId());
    }

    /**
     * Returns the preferred network type that should be set in the modem.
     *
     * @param context The current {@link Context}.
     * @return the preferred network mode that should be set.
     */
    // TODO: Fix when we "properly" have TelephonyDevController/SubscriptionController ..
    public static Int32 CalculatePreferredNetworkType(Context context, Int32 phoneId) {
        Int32 networkType;
        Int32 preferredNetworkType = RILConstants.PREFERRED_NETWORK_MODE;
        If (TelephonyManager->GetLteOnCdmaModeStatic(phoneId) == PhoneConstants.LTE_ON_CDMA_TRUE) {
            preferredNetworkType = Phone.NT_MODE_GLOBAL;
        }
         try {
            networkType = TelephonyManager->GetIntAtIndex(context->GetContentResolver(),
                    Settings::Global::PREFERRED_NETWORK_MODE, phoneId);
            } Catch (SettingNotFoundException snfe) {
                Rlog->E(LOG_TAG, "Settings Exception Reading Value At Index " + phoneId +
                        " for Settings::Global::PREFERRED_NETWORK_MODE");
                networkType = preferredNetworkType;
            }
        return networkType;
    }

    /* Gets the default subscription */
    public static Int64 GetDefaultSubscription() {
        return SubscriptionController->GetInstance()->GetDefaultSubId();
    }

    /* Gets User preferred Voice subscription setting*/
    public static Int32 GetVoiceSubscription() {
        Int32 subId = 0;

        try {
            subId = Settings::Global::>GetInt(sContext->GetContentResolver(),
                    Settings::Global::MULTI_SIM_VOICE_CALL_SUBSCRIPTION);
        } Catch (SettingNotFoundException snfe) {
            Rlog->E(LOG_TAG, "Settings Exception Reading Dual Sim Voice Call Values");
        }

        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);
        // Set subscription to 0 if current subscription is invalid.
        // Ex: multisim.config property is TSTS and subscription is 2.
        // If user is trying to set multisim.config to DSDS and reboots
        // in this case index 2 is invalid so need to set to 0.
        If (phoneId < 0 || phoneId >= TelephonyManager->GetDefault()->GetPhoneCount()) {
            Rlog->I(LOG_TAG, "Subscription is invalid..." + subId + " Set to 0");
            subId = 0;
            SetVoiceSubscription(subId);
        }

        return subId;
    }

    /* Returns User Prompt property,  enabed or not */
    public static Boolean IsPromptEnabled() {
        Boolean prompt = FALSE;
        Int32 value = 0;
        try {
            value = Settings::Global::>GetInt(sContext->GetContentResolver(),
                    Settings::Global::MULTI_SIM_VOICE_PROMPT);
        } Catch (SettingNotFoundException snfe) {
            Rlog->E(LOG_TAG, "Settings Exception Reading Dual Sim Voice Prompt Values");
        }
        prompt = (value == 0) ? FALSE : TRUE ;
        Rlog->D(LOG_TAG, "Prompt option:" + prompt);

       return prompt;
    }

    /*Sets User Prompt property,  enabed or not */
    public static void SetPromptEnabled(Boolean enabled) {
        Int32 value = (enabled == FALSE) ? 0 : 1;
        Settings::Global::>PutInt(sContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_VOICE_PROMPT, value);
        Rlog->D(LOG_TAG, "setVoicePromptOption to " + enabled);
    }

    /* Returns User SMS Prompt property,  enabled or not */
    public static Boolean IsSMSPromptEnabled() {
        Boolean prompt = FALSE;
        Int32 value = 0;
        try {
            value = Settings::Global::>GetInt(sContext->GetContentResolver(),
                    Settings::Global::MULTI_SIM_SMS_PROMPT);
        } Catch (SettingNotFoundException snfe) {
            Rlog->E(LOG_TAG, "Settings Exception Reading Dual Sim SMS Prompt Values");
        }
        prompt = (value == 0) ? FALSE : TRUE ;
        Rlog->D(LOG_TAG, "SMS Prompt option:" + prompt);

       return prompt;
    }

    /*Sets User SMS Prompt property,  enable or not */
    public static void SetSMSPromptEnabled(Boolean enabled) {
        Int32 value = (enabled == FALSE) ? 0 : 1;
        Settings::Global::>PutInt(sContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_SMS_PROMPT, value);
        Rlog->D(LOG_TAG, "setSMSPromptOption to " + enabled);
    }

    /* Gets User preferred Data subscription setting*/
    public static Int64 GetDataSubscription() {
        Int64 subId = 1;

        try {
            subId = Settings::Global::>GetLong(sContext->GetContentResolver(),
                    Settings::Global::MULTI_SIM_DATA_CALL_SUBSCRIPTION);
        } Catch (SettingNotFoundException snfe) {
            Rlog->E(LOG_TAG, "Settings Exception Reading Dual Sim Data Call Values");
        }

        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);
        If (phoneId < 0 || phoneId >= TelephonyManager->GetDefault()->GetPhoneCount()) {
            subId = 1;
            Rlog->I(LOG_TAG, "Subscription is invalid..." + subId + " Set to 0");
            SetDataSubscription(subId);
        }

        return subId;
    }

    /* Gets User preferred SMS subscription setting*/
    public static Int32 GetSMSSubscription() {
        Int32 subId = 0;
        try {
            subId = Settings::Global::>GetInt(sContext->GetContentResolver(),
                    Settings::Global::MULTI_SIM_SMS_SUBSCRIPTION);
        } Catch (SettingNotFoundException snfe) {
            Rlog->E(LOG_TAG, "Settings Exception Reading Dual Sim SMS Values");
        }

        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);
        If (phoneId < 0 || phoneId >= TelephonyManager->GetDefault()->GetPhoneCount()) {
            Rlog->I(LOG_TAG, "Subscription is invalid..." + subId + " Set to 0");
            subId = 0;
            SetSMSSubscription(subId);
        }

        return subId;
    }

    static CARAPI SetVoiceSubscription(Int32 subId) {
        Settings::Global::>PutInt(sContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_VOICE_CALL_SUBSCRIPTION, subId);
        Rlog->D(LOG_TAG, "setVoiceSubscription : " + subId);
    }

    static CARAPI SetDataSubscription(Int64 subId) {
        Boolean enabled;

        Settings::Global::>PutLong(sContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_DATA_CALL_SUBSCRIPTION, subId);
        Rlog->D(LOG_TAG, "setDataSubscription: " + subId);

        // Update the current mobile data flag
        enabled = Settings::Global::>GetInt(sContext->GetContentResolver(),
                Settings::Global::MOBILE_DATA + subId, 0) != 0;
        Settings::Global::>PutInt(sContext->GetContentResolver(),
                Settings::Global::MOBILE_DATA, enabled ? 1 : 0);
        Rlog->D(LOG_TAG, "set mobile_data: " + enabled);

        // Update the current data roaming flag
        enabled = Settings::Global::>GetInt(sContext->GetContentResolver(),
                Settings::Global::DATA_ROAMING + subId, 0) != 0;
        Settings::Global::>PutInt(sContext->GetContentResolver(),
                Settings::Global::DATA_ROAMING, enabled ? 1 : 0);
        Rlog->D(LOG_TAG, "set data_roaming: " + enabled);
    }

    static CARAPI SetSMSSubscription(Int32 subId) {
        Settings::Global::>PutInt(sContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_SMS_SUBSCRIPTION, subId);

        Intent intent = new Intent("com.android.mms.transaction.SEND_MESSAGE");
        sContext->SendBroadcast(intent);

        // Change occured in SMS preferred sub, update the default
        // SMS interface Manager object with the new SMS preferred subscription.
        Rlog->D(LOG_TAG, "setSMSSubscription : " + subId);
    }

    /**
     * Makes a {@link ImsPhone} object.
     * @return the {@code ImsPhone} object or NULL if the exception occured
     */
    public static ImsPhone MakeImsPhone(PhoneNotifier phoneNotifier, Phone defaultPhone) {
        return ImsPhoneFactory->MakePhone(sContext, phoneNotifier, defaultPhone);
    }

    private static Boolean IsValidphoneId(Int32 phoneId) {
        Return (phoneId >= 0) && (phoneId < TelephonyManager->GetDefault()->GetPhoneCount());
    }

    private static Int32 GetDefaultPhoneId() {
        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(GetDefaultSubscription());
        If (!IsValidphoneId(phoneId)) {
            phoneId = 0;
        }
        return phoneId;
    }

    public static SubInfoRecordUpdater GetSubInfoRecordUpdater() {
        return sSubInfoRecordUpdater;
    }
}
