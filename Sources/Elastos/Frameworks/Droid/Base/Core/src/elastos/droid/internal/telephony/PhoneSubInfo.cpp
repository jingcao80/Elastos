/*
 * Copyright (C) 2007 The Android Open Source Project
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

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;

public class PhoneSubInfo {
    static const String LOG_TAG = "PhoneSubInfo";
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE; // STOPSHIP if TRUE

    private Phone mPhone;
    private Context mContext;
    private static const String READ_PHONE_STATE =
        Manifest::permission::READ_PHONE_STATE;
    // TODO: change GetCompleteVoiceMailNumber() to require READ_PRIVILEGED_PHONE_STATE
    private static const String CALL_PRIVILEGED =
        Manifest::permission::CALL_PRIVILEGED;
    private static const String READ_PRIVILEGED_PHONE_STATE =
        Manifest::permission::READ_PRIVILEGED_PHONE_STATE;

    public PhoneSubInfo(Phone phone) {
        mPhone = phone;
        mContext = phone->GetContext();
    }

    CARAPI Dispose() {
    }

    //@Override
    protected void Finalize() {
        try {
            super->Finalize();
        } Catch (Throwable throwable) {
            Loge("Error while finalizing:", throwable);
        }
        If (DBG) Log("PhoneSubInfo finalized");
    }

    /**
     * Retrieves the unique device ID, e.g., IMEI for GSM phones and MEID for CDMA phones.
     */
    public String GetDeviceId() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetDeviceId();
    }

    /**
     * Retrieves the IMEI.
     */
    public String GetImei() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetImei();
    }

    /**
     * Retrieves the software version number for the device, e.g., IMEI/SV
     * for GSM phones.
     */
    public String GetDeviceSvn() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetDeviceSvn();
    }

    /**
     * Retrieves the unique subscriber ID, e.g., IMSI for GSM phones.
     */
    public String GetSubscriberId() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetSubscriberId();
    }

    /**
     * Retrieves the Group Identifier Level1 for GSM phones.
     */
    public String GetGroupIdLevel1() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetGroupIdLevel1();
    }

    /**
     * Retrieves the serial number of the ICC, if applicable.
     */
    public String GetIccSerialNumber() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetIccSerialNumber();
    }

    /**
     * Retrieves the phone number string for line 1.
     */
    public String GetLine1Number() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetLine1Number();
    }

    /**
     * Retrieves the alpha identifier for line 1.
     */
    public String GetLine1AlphaTag() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetLine1AlphaTag();
    }

    /**
     * Retrieves the MSISDN string.
     */
    public String GetMsisdn() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetMsisdn();
    }

    /**
     * Retrieves the voice mail number.
     */
    public String GetVoiceMailNumber() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        String number = PhoneNumberUtils->ExtractNetworkPortion(mPhone->GetVoiceMailNumber());
        If (VDBG) Log("VM: PhoneSubInfo.getVoiceMailNUmber: " + number);
        return number;
    }

    /**
     * Retrieves the complete voice mail number.
     *
     * @hide
     */
    public String GetCompleteVoiceMailNumber() {
        mContext->EnforceCallingOrSelfPermission(CALL_PRIVILEGED,
                "Requires CALL_PRIVILEGED");
        String number = mPhone->GetVoiceMailNumber();
        If (VDBG) Log("VM: PhoneSubInfo.getCompleteVoiceMailNUmber: " + number);
        return number;
    }

    /**
     * Retrieves the alpha identifier associated with the voice mail number.
     */
    public String GetVoiceMailAlphaTag() {
        mContext->EnforceCallingOrSelfPermission(READ_PHONE_STATE, "Requires READ_PHONE_STATE");
        return mPhone->GetVoiceMailAlphaTag();
    }

    /**
     * Returns the IMS private user Identity (IMPI) that was loaded from the ISIM.
     * @return the IMPI, or NULL if not present or not loaded
     */
    public String GetIsimImpi() {
        mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
                "Requires READ_PRIVILEGED_PHONE_STATE");
        IsimRecords isim = mPhone->GetIsimRecords();
        If (isim != NULL) {
            return isim->GetIsimImpi();
        } else {
            return NULL;
        }
    }

    /**
     * Returns the IMS home network domain name that was loaded from the ISIM.
     * @return the IMS domain name, or NULL if not present or not loaded
     */
    public String GetIsimDomain() {
        mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
                "Requires READ_PRIVILEGED_PHONE_STATE");
        IsimRecords isim = mPhone->GetIsimRecords();
        If (isim != NULL) {
            return isim->GetIsimDomain();
        } else {
            return NULL;
        }
    }

    /**
     * Returns the IMS public user Identities (IMPU) that were loaded from the ISIM.
     * @return an array of IMPU strings, with one IMPU per string, or NULL if
     *      not present or not loaded
     */
    public String[] GetIsimImpu() {
        mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
                "Requires READ_PRIVILEGED_PHONE_STATE");
        IsimRecords isim = mPhone->GetIsimRecords();
        If (isim != NULL) {
            return isim->GetIsimImpu();
        } else {
            return NULL;
        }
    }

    /**
     * Returns the IMS Service Table (IST) that was loaded from the ISIM.
     * @return IMS Service Table or NULL if not present or not loaded
     */
    public String GetIsimIst(){
        mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
                "Requires READ_PRIVILEGED_PHONE_STATE");
        IsimRecords isim = mPhone->GetIsimRecords();
        If (isim != NULL) {
            return isim->GetIsimIst();
        } else {
            return NULL;
        }
     }

    /**
     * Returns the IMS Proxy Call Session Control Function(PCSCF) that were loaded from the ISIM.
     * @return an array of  PCSCF strings with one PCSCF per string, or NULL if
     *      not present or not loaded
     */
    public String[] GetIsimPcscf() {
        mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
                "Requires READ_PRIVILEGED_PHONE_STATE");
        IsimRecords isim = mPhone->GetIsimRecords();
        If (isim != NULL) {
            return isim->GetIsimPcscf();
        } else {
            return NULL;
        }
    }

    /**
     * Returns the response of ISIM Authetification through RIL.
     * Returns NULL if the Authentification hasn't been successed or isn't present iphonesubinfo.
     * @return the response of ISIM Authetification, or NULL if not available
     */
    public String GetIsimChallengeResponse(String nonce){
        mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
                "Requires READ_PRIVILEGED_PHONE_STATE");
        IsimRecords isim = mPhone->GetIsimRecords();
        If (isim != NULL) {
            return isim->GetIsimChallengeResponse(nonce);
        } else {
            return NULL;
        }
    }

    /**
     * Returns the response of the SIM application on the UICC to authentication
     * challenge/response algorithm. The data string and challenge response are
     * Base64 encoded Strings.
     * Can support EAP-SIM, EAP-AKA with results encoded per 3GPP TS 31.102.
     *
     * @param appType ICC application Family (@see com.android.internal.telephony.PhoneConstants#APPTYPE_xxx)
     * @param data authentication challenge data
     * @return challenge response
     */
    public String GetIccSimChallengeResponse(Int64 subId, Int32 appType, String data) {
        // FIXME: use subId!!
        mContext->EnforceCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE,
                "Requires READ_PRIVILEGED_PHONE_STATE");

        UiccCard uiccCard = mPhone->GetUiccCard();
        If (uiccCard == NULL) {
            Rlog->E(LOG_TAG, "GetIccSimChallengeResponse() UiccCard is NULL");
            return NULL;
        }

        UiccCardApplication uiccApp = uiccCard->GetApplicationByType(appType);
        If (uiccApp == NULL) {
            Rlog->E(LOG_TAG, "GetIccSimChallengeResponse() no app with specified type -- " +
                    appType);
            return NULL;
        } else {
            Rlog->E(LOG_TAG, "GetIccSimChallengeResponse() found app " + uiccApp->GetAid()
                    + "specified type -- " + appType);
        }

        Int32 authContext = uiccApp->GetAuthContext();

        If (data->Length() < 32) {
            /* must use EAP_SIM context */
            Rlog->E(LOG_TAG, "data is too small to use EAP_AKA, using EAP_SIM instead");
            authContext = UiccCardApplication.AUTH_CONTEXT_EAP_SIM;
        }

        If(authContext == UiccCardApplication.AUTH_CONTEXT_UNDEFINED) {
            Rlog->E(LOG_TAG, "GetIccSimChallengeResponse() authContext undefined for app type " +
                    appType);
            return NULL;
        }

        return uiccApp->GetIccRecords()->GetIccSimChallengeResponse(authContext, data);
    }

    private void Log(String s) {
        Rlog->D(LOG_TAG, s);
    }

    private void Loge(String s, Throwable e) {
        Rlog->E(LOG_TAG, s, e);
    }

    protected void Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        If (mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
                != PackageManager.PERMISSION_GRANTED) {
            pw->Println("Permission Denial: can't dump PhoneSubInfo from from pid="
                    + Binder->GetCallingPid()
                    + ", uid=" + Binder->GetCallingUid());
            return;
        }

        pw->Println("Phone Subscriber Info:");
        pw->Println("  Phone Type = " + mPhone->GetPhoneName());
        pw->Println("  Device ID = " + mPhone->GetDeviceId());
    }
}
