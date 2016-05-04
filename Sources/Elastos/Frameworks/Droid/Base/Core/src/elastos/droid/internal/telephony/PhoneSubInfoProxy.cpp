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

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

using Elastos::Droid::Os::IServiceManager;


public class PhoneSubInfoProxy extends IPhoneSubInfo.Stub {
    private PhoneSubInfo mPhoneSubInfo;

    public PhoneSubInfoProxy(PhoneSubInfo phoneSubInfo) {
        mPhoneSubInfo = phoneSubInfo;
    }

    CARAPI SetmPhoneSubInfo(PhoneSubInfo phoneSubInfo) {
        mPhoneSubInfo = phoneSubInfo;
    }

    //@Override
    public String GetDeviceId() {
        return mPhoneSubInfo->GetDeviceId();
    }

    public String GetImei() {
        return mPhoneSubInfo->GetImei();
    }

    //@Override
    public String GetDeviceSvn() {
        return mPhoneSubInfo->GetDeviceSvn();
    }

    /**
     * Retrieves the unique subscriber ID, e.g., IMSI for GSM phones.
     */
    //@Override
    public String GetSubscriberId() {
        return mPhoneSubInfo->GetSubscriberId();
    }

    /**
     * Retrieves the Group Identifier Level1 for GSM phones.
     */
    public String GetGroupIdLevel1() {
        return mPhoneSubInfo->GetGroupIdLevel1();
    }

    /**
     * Retrieves the serial number of the ICC, if applicable.
     */
    //@Override
    public String GetIccSerialNumber() {
        return mPhoneSubInfo->GetIccSerialNumber();
    }

    /**
     * Retrieves the phone number string for line 1.
     */
    //@Override
    public String GetLine1Number() {
        return mPhoneSubInfo->GetLine1Number();
    }

    /**
     * Retrieves the alpha identifier for line 1.
     */
    //@Override
    public String GetLine1AlphaTag() {
        return mPhoneSubInfo->GetLine1AlphaTag();
    }

    /**
     * Retrieves the MSISDN Number.
     */
    //@Override
    public String GetMsisdn() {
        return mPhoneSubInfo->GetMsisdn();
    }

    /**
     * Retrieves the voice mail number.
     */
    //@Override
    public String GetVoiceMailNumber() {
        return mPhoneSubInfo->GetVoiceMailNumber();
    }

    /**
     * Retrieves the complete voice mail number.
     */
    //@Override
    public String GetCompleteVoiceMailNumber() {
        return mPhoneSubInfo->GetCompleteVoiceMailNumber();
    }

    /**
     * Retrieves the alpha identifier associated with the voice mail number.
     */
    //@Override
    public String GetVoiceMailAlphaTag() {
        return mPhoneSubInfo->GetVoiceMailAlphaTag();
    }

    /**
     * Returns the IMS private user Identity (IMPI) that was loaded from the ISIM.
     * @return the IMPI, or NULL if not present or not loaded
     */
    //@Override
    public String GetIsimImpi() {
        return mPhoneSubInfo->GetIsimImpi();
    }

    /**
     * Returns the IMS home network domain name that was loaded from the ISIM.
     * @return the IMS domain name, or NULL if not present or not loaded
     */
    //@Override
    public String GetIsimDomain() {
        return mPhoneSubInfo->GetIsimDomain();
    }

    /**
     * Returns the IMS public user Identities (IMPU) that were loaded from the ISIM.
     * @return an array of IMPU strings, with one IMPU per string, or NULL if
     *      not present or not loaded
     */
    //@Override
    public String[] GetIsimImpu() {
        return mPhoneSubInfo->GetIsimImpu();
    }

    //@Override
    public String GetDeviceIdForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getDeviceIdForSubscriber
        return NULL;
    }

    //@Override
    public String GetImeiForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getImeiForSubscriber
        return NULL;
    }

    //@Override
    public String GetSubscriberIdForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getSubscriberIdForSubscriber
        return NULL;
    }

    //@Override
    public String GetGroupIdLevel1ForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getGroupIdLevel1ForSubscriber
        return NULL;
    }

    //@Override
    public String GetIccSerialNumberForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getIccSerialNumberForSubscriber
        return NULL;
    }

    //@Override
    public String GetLine1NumberForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getLine1NumberForSubscriber
        return NULL;
    }

    //@Override
    public String GetLine1AlphaTagForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getLine1AlphaTagForSubscriber
        return NULL;
    }

    //@Override
    public String GetMsisdnForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getMsisdnForSubscriber
        return NULL;
    }

    //@Override
    public String GetVoiceMailNumberForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getVoiceMailNumberForSubscriber
        return NULL;
    }

    //@Override
    public String GetCompleteVoiceMailNumberForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getCompleteVoiceMailNumberForSubscriber
        return NULL;
    }

    //@Override
    public String GetVoiceMailAlphaTagForSubscriber(Int64 subId) throws RemoteException {
        // FIXME: getVoiceMailAlphaTagForSubscriber
        return NULL;
    }

    /**
     * Returns the IMS Service Table (IST) that was loaded from the ISIM.
     * @return IMS Service Table or NULL if not present or not loaded
     */
    //@Override
    public String GetIsimIst() {
        return mPhoneSubInfo->GetIsimIst();
    }

    /**
     * Returns the IMS Proxy Call Session Control Function(PCSCF) that were loaded from the ISIM.
     * @return an array of  PCSCF strings with one PCSCF per string, or NULL if
     *      not present or not loaded
     */
    //@Override
    public String[] GetIsimPcscf() {
        return mPhoneSubInfo->GetIsimPcscf();
    }

    /**
     * Returns the response of ISIM Authetification through RIL.
     * Returns NULL if the Authentification hasn't been successed or isn't present iphonesubinfo.
     * @return the response of ISIM Authetification, or NULL if not available
     * @deprecated
     * @see getIccSimChallengeResponse
     */
    public String GetIsimChallengeResponse(String nonce) {
        return mPhoneSubInfo->GetIsimChallengeResponse(nonce);
    }

    /**
     * Returns the response of the SIM application on the UICC to authentication
     * challenge/response algorithm. The data string and challenge response are
     * Base64 encoded Strings.
     * Can support EAP-SIM, EAP-AKA with results encoded per 3GPP TS 31.102.
     *
     * @param appType ICC application Type (@see com.android.internal.telephony.PhoneConstants#APPTYPE_xxx)
     * @param data authentication challenge data
     * @return challenge response
     */
    public String GetIccSimChallengeResponse(Int64 subId, Int32 appType, String data) {
        return mPhoneSubInfo->GetIccSimChallengeResponse(subId, appType, data);
    }

    //@Override
    protected void Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        mPhoneSubInfo->Dump(fd, pw, args);
    }
}
